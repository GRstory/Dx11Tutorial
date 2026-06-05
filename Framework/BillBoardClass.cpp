#include "BillBoardClass.h"
#include <cmath>

BillBoardClass::BillBoardClass(XMFLOAT3 pos, float scale, CameraClass* camera)
	: m_pos(pos), m_scale(scale), m_camera(camera)
{
	XMMATRIX S = XMMatrixScaling(m_scale, m_scale, m_scale);
	XMMATRIX T = XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
	GetTransform()->SetWorldMatrix(XMMatrixMultiply(S, T));
}

void BillBoardClass::Update()
{
	XMFLOAT3 camF = m_camera->GetPosition();
	XMVECTOR pos  = XMLoadFloat3(&m_pos);
	XMVECTOR cam  = XMLoadFloat3(&camF);

	// forward = pos - cam (model +Z away from camera, so face normal (-Z) looks toward camera)
	XMVECTOR forward = XMVector3Normalize(pos - cam);
	XMVECTOR worldUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR right   = XMVector3Normalize(XMVector3Cross(worldUp, forward));
	XMVECTOR up      = XMVector3Normalize(XMVector3Cross(forward, right));

	XMFLOAT3 r, u, f;
	XMStoreFloat3(&r, right);
	XMStoreFloat3(&u, up);
	XMStoreFloat3(&f, forward);

	XMFLOAT4X4 m = {};
	m._11 = m_scale * r.x; m._12 = m_scale * r.y; m._13 = m_scale * r.z; m._14 = 0.0f;
	m._21 = m_scale * u.x; m._22 = m_scale * u.y; m._23 = m_scale * u.z; m._24 = 0.0f;
	m._31 = m_scale * f.x; m._32 = m_scale * f.y; m._33 = m_scale * f.z; m._34 = 0.0f;
	m._41 = m_pos.x;       m._42 = m_pos.y;       m._43 = m_pos.z;       m._44 = 1.0f;

	GetTransform()->SetWorldMatrix(XMLoadFloat4x4(&m));
}
