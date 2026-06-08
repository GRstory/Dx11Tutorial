////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "cameraclass.h"
#include "Actor.h"
#include "ModelComponent.h"
#include "RenderSystem.h"
#include "playerclass.h"
#include "inputclass.h"
#include "AlignedAllocationPolicy.h"
#include <vector>


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass : public AlignedAllocationPolicy<16>
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND, InputClass*);
	void Shutdown();
	bool Frame(int fps, int cpu);
	void SetFilter(D3D11_FILTER);

private:
	bool Render();

private:
	D3DClass*        m_D3D;
	CameraClass*     m_Camera;
	InputClass*      m_Input;

	class BitmapClass* m_Bitmap;
	class TextClass*   m_Text;

	XMMATRIX m_baseViewMatrix;
	bool     m_showBitmap      = true;
	bool     m_enterWasPressed = false;

	int m_screenWidth  = 0;
	int m_screenHeight = 0;

private: //Game objects
	shared_ptr<PlayerClass> m_Player;
	std::vector<shared_ptr<Actor>> m_ActorList;
};

#endif
