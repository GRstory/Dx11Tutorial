#include "playerclass.h"
#include "ModelComponent.h"


PlayerClass::PlayerClass(InputClass* input) : m_input(input)
{
	m_position        = XMFLOAT3(0.0f, 55.0f, 0.0f);
	m_velocity        = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_angularVelocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	m_right   = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_up      = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_forward = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_gravityCenter   = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_sphereRadius    = 100.0f;
	m_gravityStrength = 0.01f;
	m_thrustPower     = 0.015f;
	m_maxSpeed        = 2.0f;
	m_rotSensitivity  = 0.001f;
	m_modelScale      = 0.4f;
}


PlayerClass::~PlayerClass()
{
}


void PlayerClass::Reorthogonalize()
{
	XMVECTOR fwd   = XMVector3Normalize(XMLoadFloat3(&m_forward));
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&m_up), fwd));
	XMVECTOR up    = XMVector3Cross(fwd, right);

	XMStoreFloat3(&m_forward, fwd);
	XMStoreFloat3(&m_right,   right);
	XMStoreFloat3(&m_up,      up);
}


void PlayerClass::Update()
{
	bool w    = m_input->IsKeyPressed(DIK_W);
	bool s    = m_input->IsKeyPressed(DIK_S);
	bool a    = m_input->IsKeyPressed(DIK_A);
	bool d    = m_input->IsKeyPressed(DIK_D);
	bool up   = m_input->IsKeyPressed(DIK_LSHIFT);
	bool down = m_input->IsKeyPressed(DIK_LCONTROL);
	int  mouseDX, mouseDY;
	m_input->GetMouseDelta(mouseDX, mouseDY);

	// 표면 접촉 여부를 먼저 판단 (이전 프레임 위치 기준)
	XMVECTOR pos    = XMLoadFloat3(&m_position);
	XMVECTOR center = XMLoadFloat3(&m_gravityCenter);
	float dist = XMVectorGetX(XMVector3Length(XMVectorSubtract(pos, center)));
	bool grounded = (dist <= m_sphereRadius + 0.5f);

	if (!grounded)
	{
		// 마우스/키 입력으로 각속도를 가속 (직접 회전 아님)
		m_angularVelocity.x += -mouseDY * m_rotSensitivity;
		m_angularVelocity.y +=  mouseDX * m_rotSensitivity;

		// 우주 공간: 각속도를 천천히 감쇠 (관성 유지)
		m_angularVelocity.x *= 0.95f;
		m_angularVelocity.y *= 0.95f;
		m_angularVelocity.z *= 0.95f;

		// 최대 각속도 제한 (3 deg/frame = ~180 deg/s @ 60fps)
		XMVECTOR av = XMLoadFloat3(&m_angularVelocity);
		av = XMVector3ClampLength(av, 0.0f, 3.0f);
		XMStoreFloat3(&m_angularVelocity, av);

		XMVECTOR rightVec = XMLoadFloat3(&m_right);
		XMVECTOR upVec    = XMLoadFloat3(&m_up);
		XMVECTOR fwdVec   = XMLoadFloat3(&m_forward);

		if (m_angularVelocity.x != 0.0f)
		{
			XMMATRIX mat = XMMatrixRotationAxis(rightVec, XMConvertToRadians(m_angularVelocity.x));
			upVec  = XMVector3TransformNormal(upVec,  mat);
			fwdVec = XMVector3TransformNormal(fwdVec, mat);
		}
		if (m_angularVelocity.y != 0.0f)
		{
			XMMATRIX mat = XMMatrixRotationAxis(upVec, XMConvertToRadians(m_angularVelocity.y));
			rightVec = XMVector3TransformNormal(rightVec, mat);
			fwdVec   = XMVector3TransformNormal(fwdVec,   mat);
		}
		if (m_angularVelocity.z != 0.0f)
		{
			XMMATRIX mat = XMMatrixRotationAxis(fwdVec, XMConvertToRadians(m_angularVelocity.z));
			rightVec = XMVector3TransformNormal(rightVec, mat);
			upVec    = XMVector3TransformNormal(upVec,    mat);
		}

		XMStoreFloat3(&m_right,   rightVec);
		XMStoreFloat3(&m_up,      upVec);
		XMStoreFloat3(&m_forward, fwdVec);

		Reorthogonalize();
	}
	else
	{
		// 표면 접촉 시 각속도 소멸 + 자세를 표면 수평으로 정렬
		m_angularVelocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

		// 새 up = 구 표면 법선 (행성 중심에서 플레이어 방향)
		XMVECTOR surfaceUp = XMVector3Normalize(XMVectorSubtract(pos, center));
		XMVECTOR fwdVec    = XMLoadFloat3(&m_forward);

		// 현재 forward를 표면 접선면에 투영해 facing 방향 최대한 유지
		float proj = XMVectorGetX(XMVector3Dot(fwdVec, surfaceUp));
		fwdVec = XMVectorSubtract(fwdVec, XMVectorScale(surfaceUp, proj));

		XMVECTOR newRight;
		if (XMVectorGetX(XMVector3LengthSq(fwdVec)) > 0.001f)
		{
			fwdVec   = XMVector3Normalize(fwdVec);
			newRight = XMVector3Normalize(XMVector3Cross(surfaceUp, fwdVec));
		}
		else
		{
			// forward가 법선과 평행한 극단적 경우 — right 축으로 fallback
			XMVECTOR rightVec = XMLoadFloat3(&m_right);
			proj     = XMVectorGetX(XMVector3Dot(rightVec, surfaceUp));
			newRight = XMVector3Normalize(XMVectorSubtract(rightVec, XMVectorScale(surfaceUp, proj)));
		}

		// right와 up으로 forward 재계산 (완전 직교 보장)
		fwdVec = XMVector3Cross(newRight, surfaceUp);

		XMStoreFloat3(&m_up,      surfaceUp);
		XMStoreFloat3(&m_right,   newRight);
		XMStoreFloat3(&m_forward, fwdVec);
	}

	// 세 축 로드 (회전 이후 또는 grounded 분기 이후 공통)
	XMVECTOR rightVec = XMLoadFloat3(&m_right);
	XMVECTOR upVec    = XMLoadFloat3(&m_up);
	XMVECTOR fwdVec   = XMLoadFloat3(&m_forward);

	// 추력: W/S = 전/후, Shift/Ctrl = 상/하 (A/D는 롤)
	XMVECTOR vel = XMLoadFloat3(&m_velocity);

	if (w)    vel = XMVectorAdd(vel, XMVectorScale(fwdVec,   m_thrustPower));
	if (s)    vel = XMVectorAdd(vel, XMVectorScale(fwdVec,  -m_thrustPower));
	if (a)    vel = XMVectorAdd(vel, XMVectorScale(rightVec, -m_thrustPower));
	if (d)    vel = XMVectorAdd(vel, XMVectorScale(rightVec,  m_thrustPower));
	if (up)   vel = XMVectorAdd(vel, XMVectorScale(upVec,    m_thrustPower));
	if (down) vel = XMVectorAdd(vel, XMVectorScale(upVec,   -m_thrustPower));

	// 중력
	XMVECTOR gravDir = XMVector3Normalize(XMVectorSubtract(center, pos));
	vel = XMVectorAdd(vel, XMVectorScale(gravDir, m_gravityStrength));

	// 관성
	vel = XMVectorScale(vel, 0.97f);

	// 최대 속도
	vel = XMVector3ClampLength(vel, 0.0f, m_maxSpeed);

	pos = XMVectorAdd(pos, vel);

	XMStoreFloat3(&m_velocity, vel);
	XMStoreFloat3(&m_position, pos);

	// 행성 콜라이더
	XMVECTOR toPlayer = XMVectorSubtract(pos, center);
	dist = XMVectorGetX(XMVector3Length(toPlayer));

	if (dist < m_sphereRadius)
	{
		XMVECTOR surfaceNormal = XMVector3Normalize(toPlayer);
		pos = XMVectorAdd(center, XMVectorScale(surfaceNormal, m_sphereRadius));
		XMStoreFloat3(&m_position, pos);

		vel = XMLoadFloat3(&m_velocity);
		float radialV = XMVectorGetX(XMVector3Dot(vel, surfaceNormal));
		if (radialV < 0.0f)
			vel = XMVectorSubtract(vel, XMVectorScale(surfaceNormal, radialV));

		vel = XMVectorScale(vel, 0.88f);
		XMStoreFloat3(&m_velocity, vel);
	}

	// 물리 기반 월드 행렬을 TransformComponent에 전달 → RenderSystem이 올바른 변환으로 렌더링
	GetTransform()->SetWorldMatrix(GetWorldMatrix());
}


// 로컬 3축으로 월드 행렬 구성: v_world = v_local * (Scale * Rot * Translation)
XMMATRIX PlayerClass::GetWorldMatrix() const
{
	XMMATRIX rot;
	rot.r[0] = XMVectorSetW(XMLoadFloat3(&m_right),   0.0f);
	rot.r[1] = XMVectorSetW(XMLoadFloat3(&m_up),      0.0f);
	rot.r[2] = XMVectorSetW(XMLoadFloat3(&m_forward), 0.0f);
	rot.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	return XMMatrixScaling(m_modelScale, m_modelScale, m_modelScale)
		* rot
		* XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
}


XMFLOAT3 PlayerClass::GetPosition()     const { return m_position; }
XMFLOAT3 PlayerClass::GetLocalUp()      const { return m_up; }
XMFLOAT3 PlayerClass::GetLocalForward() const { return m_forward; }

void PlayerClass::SetGravityCenter(float x, float y, float z)
{
	m_gravityCenter = XMFLOAT3(x, y, z);
}

void PlayerClass::SetSphereRadius(float radius)
{
	m_sphereRadius = radius;
}
