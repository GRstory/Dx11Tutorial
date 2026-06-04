////////////////////////////////////////////////////////////////////////////////
// Filename: playerclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _PLAYERCLASS_H_
#define _PLAYERCLASS_H_

#include <directxmath.h>
using namespace DirectX;

#include "Actor.h"
#include "inputclass.h"


class PlayerClass : public Actor
{
public:
	PlayerClass(InputClass*);
	~PlayerClass();

	virtual void Update() override;

	XMFLOAT3 GetPosition()     const;
	XMFLOAT3 GetLocalUp()      const;
	XMFLOAT3 GetLocalForward() const;
	XMMATRIX  GetWorldMatrix() const;

	void SetGravityCenter(float x, float y, float z);
	void SetSphereRadius(float radius);

private:
	void Reorthogonalize();

private:
	XMFLOAT3 m_position;
	XMFLOAT3 m_velocity;
	XMFLOAT3 m_angularVelocity;  // (pitch, yaw, roll) 각속도 — 도/프레임 단위

	XMFLOAT3 m_right;    // 로컬 X축
	XMFLOAT3 m_up;       // 로컬 Y축
	XMFLOAT3 m_forward;  // 로컬 Z축 (바라보는 방향)

	InputClass* m_input = nullptr;

	XMFLOAT3 m_gravityCenter;
	float    m_sphereRadius;
	float    m_gravityStrength;
	float    m_thrustPower;
	float    m_maxSpeed;
	float    m_rotSensitivity;
	float    m_modelScale;
};

#endif
