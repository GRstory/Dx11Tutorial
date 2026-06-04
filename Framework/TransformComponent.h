#pragma once
#include "Component.h"
#include <directxmath.h>

using namespace DirectX;

class TransformComponent : public Component
{
public:
	TransformComponent(const XMFLOAT3X3& transform);
	virtual ~TransformComponent();

	XMMATRIX GetWorldMatrix() const;
	void SetWorldMatrix(const XMMATRIX& world);

public:
	XMFLOAT3X3 m_Transform;

private:
	bool m_hasWorldOverride = false;
	XMFLOAT4X4 m_worldOverride;
};

