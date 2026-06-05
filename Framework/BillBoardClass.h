#pragma once
#include "Actor.h"
#include "ModelComponent.h"
#include "cameraclass.h"
#include <directxmath.h>
using namespace DirectX;

class BillBoardClass : public Actor
{
public:
	BillBoardClass(XMFLOAT3 pos, float scale, CameraClass* camera);
	virtual ~BillBoardClass() = default;

	virtual void Update() override;

private:
	XMFLOAT3     m_pos;
	float        m_scale;
	CameraClass* m_camera;
};
