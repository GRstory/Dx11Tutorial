#pragma once
#include "Actor.h"
#include <directxmath.h>
using namespace DirectX;

class WindTurbineRotor : public Actor
{
public:
	WindTurbineRotor(float scale, float xRot, float yRot, XMFLOAT3 pos, float rotationSpeed = 2.0f);
	virtual ~WindTurbineRotor() = default;

	virtual void Update() override;

private:
	float    m_scale;
	float    m_xRot;
	float    m_yRot;
	XMFLOAT3 m_pos;
	float    m_spinAngle;
	float    m_rotationSpeed;
};
