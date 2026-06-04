#pragma once
#include <iostream>
#include <unordered_map>
#include <typeindex>
#include "Component.h"
#include "TransformComponent.h"
using namespace std;

class Actor : public enable_shared_from_this<Actor>
{
public:
	Actor();
	virtual ~Actor();

public: //Lifecycle
	virtual void Update();


public: //Transform
	shared_ptr<TransformComponent> GetTransform() const { return m_transform; }

public: //Component
	template <typename T, typename... Args>
	shared_ptr<T> AddComponent(Args&&... args);

	template <typename T>
	shared_ptr<T> GetComponent() const;

private:
	shared_ptr<TransformComponent> m_transform;
	unordered_map<type_index, shared_ptr<class Component>> _componentsDictionary;
};

template<typename T, typename ...Args>
shared_ptr<T> Actor::AddComponent(Args && ...args)
{
	shared_ptr<T> component = make_shared<T>(forward<Args>(args)...);
	_componentsDictionary[type_index(typeid(T))] = component;

	if (auto self = weak_from_this().lock())
		component->SetOwner(self);
	return component;
}

template<typename T>
shared_ptr<T> Actor::GetComponent() const
{
	auto it = _componentsDictionary.find(type_index(typeid(T)));
	if (it != _componentsDictionary.end())
	{
		return dynamic_pointer_cast<T>(it->second);
	}
	return nullptr;
}