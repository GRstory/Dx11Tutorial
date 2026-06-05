#include "WindTurbineRotor.h"

static constexpr float DEG2RAD = 0.0175329f;

WindTurbineRotor::WindTurbineRotor(float scale, float xRot, float yRot, XMFLOAT3 pos, float rotationSpeed)
	: m_scale(scale), m_xRot(xRot), m_yRot(yRot), m_pos(pos), m_spinAngle(0.0f), m_rotationSpeed(rotationSpeed)
{
	// Set initial world matrix so it renders correctly before first Update
	XMMATRIX S  = XMMatrixScaling(m_scale, m_scale, m_scale);
	XMMATRIX Rx = XMMatrixRotationX(m_xRot * DEG2RAD);
	XMMATRIX Ry = XMMatrixRotationY(m_yRot * DEG2RAD);
	XMMATRIX T  = XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
	GetTransform()->SetWorldMatrix(S * Rx * Ry * T);
}

void WindTurbineRotor::Update()
{
	m_spinAngle += m_rotationSpeed;
	if (m_spinAngle >= 360.0f) m_spinAngle -= 360.0f;

	XMMATRIX S  = XMMatrixScaling(m_scale, m_scale, m_scale);
	XMMATRIX Rz = XMMatrixRotationZ(m_spinAngle * DEG2RAD);
	XMMATRIX Rx = XMMatrixRotationX(m_xRot * DEG2RAD);
	XMMATRIX Ry = XMMatrixRotationY(m_yRot * DEG2RAD);
	XMMATRIX T  = XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);

	// S * Rz(spin) * Rx * Ry * T : spin around model-local Z axis, then orient on sphere
	GetTransform()->SetWorldMatrix(S * Rz * Rx * Ry * T);
}
