#include "TransformComponent.h"

TransformComponent::TransformComponent(const XMFLOAT3X3& transform) : m_Transform(transform)
{
	
}

TransformComponent::~TransformComponent()
{
}


XMMATRIX TransformComponent::GetWorldMatrix() const
{
	if (m_hasWorldOverride)
		return XMLoadFloat4x4(&m_worldOverride);

	XMMATRIX world = XMMatrixIdentity();
	world *= XMMatrixScaling(m_Transform._11, m_Transform._12, m_Transform._13);
	world *= XMMatrixRotationX(m_Transform._21 * 0.0175329f);
	world *= XMMatrixRotationY(m_Transform._22 * 0.0175329f);
	world *= XMMatrixRotationZ(m_Transform._23 * 0.0175329f);
	world *= XMMatrixTranslation(m_Transform._31, m_Transform._32, m_Transform._33);
	return world;
}


void TransformComponent::SetWorldMatrix(const XMMATRIX& world)
{
	XMStoreFloat4x4(&m_worldOverride, world);
	m_hasWorldOverride = true;
}
