#include "Actor.h"
#include "TransformComponent.h"

Actor::Actor()
{
	m_transform = AddComponent<TransformComponent>(XMFLOAT3X3{
		1.0f,1.0f,1.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		});
}

Actor::~Actor()
{
}

void Actor::Update()
{
}
