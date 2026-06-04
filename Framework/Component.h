#pragma once
#include <iostream>

using namespace std;

class Component
{
public:
	Component();
	virtual ~Component();

public:
	virtual void Update() {};
	void SetOwner(shared_ptr<class Actor> owner) { _owner = owner; }
	weak_ptr<class Actor> GetOwner() const { return _owner; }

protected:
	weak_ptr<class Actor> _owner;

};

