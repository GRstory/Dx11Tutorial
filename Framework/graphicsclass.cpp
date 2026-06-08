////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"
#include "textclass.h"
#include "BitmapClass.h"
#include "WindTurbineRotor.h"
#include "BillBoardClass.h"
#include <random>
#include <cmath>

GraphicsClass::GraphicsClass()
{
	m_D3D    = 0;
	m_Text   = 0;
	m_Camera = 0;
	m_Player = 0;
	m_Input  = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd, InputClass* input)
{
	bool result;

	m_Input = input;

	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if(!m_D3D) return false;

	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera) return false;

	m_screenWidth  = screenWidth;
	m_screenHeight = screenHeight;

	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);
	m_Camera->Render();

	m_Camera->GetViewMatrix(m_baseViewMatrix);

	// Initialize the shared render system (one shader for all models).
	result = RenderSystem::GetInstance().Initialize(m_D3D->GetDevice(), hwnd, m_D3D);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize RenderSystem.", L"Error", MB_OK);
		return false;
	}

	// Model data (191 objects: 4 main + 12 uniform trees + 175 forest trees)
	vector<wstring> modelAddresses = {
		L"./data/Planet.obj", L"./data/Mountain_1.obj", L"./data/Volcano.obj", L"./data/TelecomTower.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj", L"./data/Tree_2.obj",
		L"./data/Tree_1.obj", L"./data/Tree_2.obj", L"./data/Tree_1.obj",
		L"./data/Home.obj", L"./data/Home.obj", L"./data/Home.obj", L"./data/Home.obj",
		L"./data/Home.obj", L"./data/Home.obj", L"./data/Home.obj", L"./data/Home.obj",
		L"./data/WindTurbineTower.obj", L"./data/WindTurbineTower.obj", L"./data/WindTurbineTower.obj", L"./data/WindTurbineTower.obj",
		L"./data/WindTurbineTower.obj",
	};
	vector<wstring> textureAddresses = {
		L"./data/grass.dds", L"./data/Mountain_1_Tex.dds", L"./data/Volcano_Tex.dds", L"./data/TelecomTower_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds",
		L"./data/Tree_1_Tex.dds", L"./data/Tree_2_Tex.dds", L"./data/Tree_1_Tex.dds",
		L"./data/Home_Tex.dds", L"./data/Home_Tex.dds", L"./data/Home_Tex.dds", L"./data/Home_Tex.dds",
		L"./data/Home_Tex.dds", L"./data/Home_Tex.dds", L"./data/Home_Tex.dds", L"./data/Home_Tex.dds",
		L"./data/WindTurbineTower_Tex.dds", L"./data/WindTurbineTower_Tex.dds", L"./data/WindTurbineTower_Tex.dds", L"./data/WindTurbineTower_Tex.dds",
		L"./data/WindTurbineTower_Tex.dds",
	};
	vector<float> sizes = {
		0.5f, 50.0f, 50.0f, 20.0f, 8.0f, 8.0f, 8.0f, 8.0f,
		8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f,
		8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f,
		8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f,
		8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f,
		8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f,
		8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f,
		8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f,
		8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f,
		8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f,
		8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f,
		8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f,
		8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f,
		8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f,
		8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f,
		8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f,
		8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f,
		8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f,
		8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f,
		8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f,
		8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f,
		8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f,
		8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f,
		8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f,
		8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f,
		8.0f, 8.0f, 20.0f, 20.0f, 20.0f, 20.0f,
		20.0f,
	};
	vector<float> xPositions = {
		0.0f, 0.0f, 0.0f, 50.0f, 58.2f, -24.1f, -58.2f, 24.1f,
		66.9f, -66.9f, -66.9f, 66.9f, 85.1f, -35.2f, -85.1f, 35.2f,
		23.2f, 12.0f, -8.9f, 21.4f, 21.5f, -0.2f, 11.6f, 8.6f,
		-22.4f, -2.4f, 6.7f, -18.8f, -9.5f, -7.4f, -8.9f, 19.8f,
		4.0f, -19.0f, -8.4f, 7.4f, 14.9f, -18.4f, -11.4f, -18.4f,
		1.9f, 13.5f, -84.8f, -84.8f, -83.0f, -74.5f, -88.7f, -78.3f,
		-73.0f, -94.6f, -85.4f, -65.3f, -85.4f, -73.8f, -64.9f, -80.3f,
		-75.3f, -78.8f, -93.6f, -90.2f, -81.3f, -89.6f, -92.4f, -70.1f,
		-80.5f, -68.2f, -82.4f, -88.6f, -74.9f, -81.3f, 93.2f, 97.1f,
		94.6f, 94.4f, 96.4f, 93.5f, 93.2f, 94.7f, 96.6f, 96.9f,
		92.9f, 96.0f, 98.0f, 97.4f, 94.8f, 90.7f, 94.1f, 91.0f,
		92.5f, 95.6f, 917.0f, 95.9f, 96.0f, 96.8f, 89.5f, 94.1f,
		93.8f, 94.1f, 95.4f, 95.8f, 97.5f, 91.8f, 91.6f, -76.4f,
		-78.5f, -68.2f, -70.1f, -53.9f, -62.4f, -68.2f, -74.2f, -71.7f,
		-57.5f, -83.3f, -81.5f, -64.1f, -70.3f, 2.4f, -74.2f, -61.5f,
		40.5f, 28.7f, 60.1f, 52.1f, 49.5f, 33.8f, 57.1f, 45.9f,
		56.8f, 26.5f, 62.3f, 52.1f, 64.1f, 43.8f, 46.8f, 29.5f,
		24.8f, 38.2f, -42.7f, -61.0f, -52.8f, -38.4f, -54.0f, -52.1f,
		-68.4f, -61.4f, -43.8f, -47.9f, -59.5f, -74.9f, -69.2f, -82.6f,
		-66.2f, -86.6f, -77.6f, -87.5f, -86.7f, -67.8f, -81.1f, -7.0f,
		-90.3f, -88.5f, -74.1f, -82.1f, -72.3f, -82.9f, -86.0f, -89.8f,
		-79.6f, 42.2f, 31.2f, 53.7f, 31.9f, 50.8f, 39.0f, 64.8f,
		30.4f, 46.2f, 40.3f, 58.7f, 52.2f, 36.8f, 57.0f, 29.1f,
		66.8f, 46.2f, 37.2f, 46.6f, 60.0f, 28.5f, 53.3f,
		39.3f, 76.3f, -32.1f, -19.4f, -3.9f, 61.8f,
		50.5f, 7.3f, 17.0f, -7.0f, -3.6f, 2.4f,
		10.0f,
	};
	vector<float> yPositions = {
		0.0f, 96.0f, -95.0f, 70.7f, 75.1f, 75.1f, 75.1f, 75.1f,
		25.4f, 25.4f, 25.4f, 25.4f, -33.5f, -33.5f, -33.5f, -33.5f,
		42.2f, 59.2f, 37.4f, 29.4f, 52.7f, 61.7f, 69.5f, 26.0f,
		47.3f, 54.6f, 42.4f, 59.6f, 63.7f, 24.3f, 46.3f, 61.2f,
		34.6f, 25.2f, 70.1f, 53.5f, 38.3f, 34.4f, 55.9f, 66.4f,
		70.4f, 47.4f, 32.9f, 41.6f, 11.8f, 5.3f, 15.5f, 15.4f,
		45.9f, 8.0f, 24.4f, 41.4f, 2.5f, 33.6f, 49.1f, 41.6f,
		25.3f, 49.8f, -0.3f, 25.8f, 31.8f, 5.9f, 17.0f, 12.8f,
		2.5f, 3.3f, 20.7f, 33.7f, 50.0f, 50.2f, -30.2f, -2.7f,
		-9.8f, -25.4f, 17.3f, 17.4f, -27.3f, -5.6f, 7.3f, -11.1f,
		-25.5f, -14.9f, 1.7f, 8.6f, -19.1f, -31.1f, 7.0f, -34.1f,
		-19.7f, -1.4f, -8.9f, -20.1f, 9.0f, -3.2f, -33.0f, 17.4f,
		-15.3f, 8.0f, 18.1f, 16.8f, 7.8f, -23.7f, -34.6f, 50.6f,
		40.9f, 63.9f, 38.4f, 71.1f, 63.9f, 46.4f, 59.7f, 49.8f,
		60.8f, 38.9f, 47.2f, 54.8f, 56.8f, 67.4f, 40.6f, 50.6f,
		-17.8f, -12.2f, -13.0f, -36.1f, -18.1f, -48.0f, -42.7f, -48.4f,
		-27.8f, -44.0f, -31.6f, -53.3f, -22.3f, -40.6f, -30.7f, -21.0f,
		-53.3f, -53.2f, 74.5f, 72.7f, 69.6f, 85.8f, 76.3f, 80.1f,
		65.5f, 65.4f, 85.8f, 75.9f, 70.6f, -63.1f, -68.7f, -50.0f,
		-72.2f, -44.7f, -57.3f, -39.4f, -44.7f, -70.3f, -54.4f, -56.6f,
		-36.8f, -37.1f, -64.0f, -53.1f, -64.2f, -49.2f, -46.9f, -38.8f,
		-57.2f, 30.3f, 0.8f, 15.7f, 32.1f, 28.4f, 13.5f, 8.3f,
		12.2f, -2.8f, 3.5f, -0.5f, 4.1f, 21.8f, 31.4f, -8.1f,
		-2.9f, 21.8f, -5.9f, 10.9f, 18.6f, 24.4f, -8.1f,
		-40.1f, -58.6f, -66.0f, -79.7f, -39.1f, 62.5f,
		-77.9f, -79.8f, 74.5f, 72.8f, 79.3f, 85.8f,
		69.1f,
	};
	vector<float> zPositions = {
		0.0f, 0.0f, 0.0f, 50.03f, 24.1f, 58.2f, -24.1f, -58.2f,
		66.9f, 66.9f, -66.9f, -66.9f, 35.2f, 85.1f, -35.2f, -85.1f,
		-85.3f, -77.1f, -90.1f, -91.0f, -79.8f, -76.2f, -68.1f, -94.1f,
		-82.9f, -81.3f, -88.1f, -75.5f, -73.9f, -94.7f, -85.9f, -73.9f,
		-91.6f, -92.8f, -68.0f, -81.8f, -89.0f, -89.9f, -79.7f, -69.7f,
		-68.2f, -84.7f, 36.5f, 26.0f, 50.7f, 63.5f, 38.8f, 56.9f,
		46.6f, 24.3f, 41.5f, 60.3f, 48.0f, 55.0f, 54.6f, 37.9f,
		57.4f, 30.2f, 28.9f, 28.2f, 44.5f, 39.3f, 28.0f, 67.3f,
		55.8f, 70.3f, 48.8f, 25.0f, 38.7f, 21.8f, 4.1f, -13.3f,
		-23.7f, -6.8f, -3.6f, -23.8f, 12.9f, 24.4f, -14.7f, 9.7f,
		-18.1f, -12.9f, 0.2f, 6.4f, 16.1f, 20.4f, 26.3f, -12.6f,
		-25.5f, -21.7f, 1.3f, 0.5f, 17.3f, 14.9f, -22.5f, 21.1f,
		24.0f, -26.3f, -13.1f, 11.7f, -5.9f, 24.8f, -3.1f, -34.8f,
		-42.1f, -29.5f, -56.8f, -40.7f, -40.3f, -52.9f, -23.0f, -44.6f,
		-51.0f, -34.0f, -27.1f, -50.0f, -38.0f, -21.3f, -49.5f, -57.2f,
		-87.4f, -92.9f, -76.3f, -74.7f, -82.6f, -78.5f, -67.2f, -71.8f,
		-74.9f, -83.5f, -68.7f, -63.6f, -70.7f, -77.7f, -80.4f, -91.1f,
		-78.4f, -72.9f, 34.5f, 24.5f, 44.4f, 27.6f, 29.4f, 21.5f,
		25.2f, 39.5f, 17.9f, 39.4f, 32.9f, -4.7f, -9.5f, -16.5f,
		-1.4f, 10.5f, -17.2f, 19.9f, -9.1f, 8.3f, 8.9f, 17.3f,
		-9.5f, -19.8f, 5.3f, -7.3f, 16.1f, 17.7f, 1.3f, 4.8f,
		-0.1f, 83.1f, 92.9f, 80.4f, 86.9f, 78.8f, 88.9f, 73.1f,
		92.4f, 86.4f, 89.3f, 78.5f, 82.9f, 88.2f, 73.3f, 93.2f,
		71.7f, 83.6f, 90.5f, 85.5f, 75.2f, 90.5f, 81.8f,
		80.3f, -18.6f, -65.0f, 53.6f, 89.8f, -43.4f,
		31.4f, -56.5f, 61.4f, 65.2f, 57.4f, 47.2f,
		68.7f,
	};
	vector<float> xRotations = {
		0.0f, 0.0f, 180.0f, 45.0f, 40.0f, 40.0f, 40.0f, 40.0f,
		75.0f, 75.0f, 75.0f, 75.0f, 110.0f, 110.0f, 110.0f, 110.0f,
		64.5f, 52.8f, 67.6f, 72.5f, 57.5f, 51.0f, 44.8f, 74.6f,
		61.2f, 56.1f, 64.3f, 52.5f, 49.5f, 75.7f, 61.8f, 51.3f,
		69.3f, 75.1f, 44.4f, 56.9f, 67.0f, 69.5f, 55.3f, 47.4f,
		44.1f, 61.1f, 70.4f, 64.9f, 83.1f, 86.9f, 80.9f, 80.9f,
		62.1f, 85.3f, 75.6f, 65.0f, 88.5f, 69.9f, 59.9f, 64.9f,
		75.0f, 59.4f, 90.2f, 74.7f, 71.0f, 86.5f, 80.0f, 82.5f,
		88.5f, 88.1f, 77.8f, 69.9f, 59.3f, 59.2f, 107.9f, 91.6f,
		95.7f, 105.0f, 79.8f, 79.8f, 106.2f, 93.3f, 85.7f, 96.5f,
		105.1f, 98.7f, 89.0f, 85.0f, 101.3f, 108.5f, 85.9f, 110.3f,
		101.6f, 90.8f, 95.2f, 101.8f, 84.7f, 91.8f, 109.7f, 79.8f,
		99.0f, 85.3f, 79.4f, 80.1f, 85.5f, 104.0f, 110.7f, 58.9f,
		65.3f, 49.3f, 66.9f, 43.5f, 49.3f, 61.7f, 52.4f, 59.4f,
		51.6f, 66.6f, 61.2f, 56.0f, 54.6f, 46.6f, 65.5f, 58.9f,
		100.4f, 97.1f, 97.6f, 111.6f, 100.6f, 119.3f, 115.8f, 119.6f,
		106.5f, 116.7f, 108.8f, 122.9f, 103.1f, 114.5f, 108.2f, 102.4f,
		122.9f, 122.9f, 40.5f, 42.1f, 44.7f, 28.9f, 38.9f, 35.1f,
		48.0f, 48.1f, 28.9f, 39.2f, 43.9f, 130.1f, 134.5f, 120.7f,
		137.5f, 117.1f, 125.8f, 113.7f, 117.1f, 135.8f, 123.7f, 125.3f,
		112.1f, 112.2f, 130.7f, 122.8f, 130.9f, 120.1f, 118.6f, 113.3f,
		125.7f, 72.0f, 89.6f, 80.8f, 70.9f, 73.2f, 82.1f, 85.1f,
		82.9f, 91.6f, 87.9f, 90.3f, 87.6f, 77.2f, 71.3f, 94.7f,
		91.7f, 77.1f, 93.4f, 83.6f, 79.1f, 75.6f, 94.8f,
		114.1f, 126.7f, 132.3f, 144.4f, 113.5f, 50.4f,
		142.6f, 144.5f, 40.5f, 42.0f, 35.9f, 28.8f,
		45.1f,
	};
	vector<float> yRotations = {
		0.0f, 0.0f, 0.0f, 45.0f, 67.5f, 337.5f, 247.5f, 157.5f,
		45.0f, 315.0f, 225.0f, 135.0f, 67.5f, 337.5f, 247.5f, 157.5f,
		164.8f, 171.1f, 185.7f, 166.8f, 165.0f, 180.2f, 170.4f, 174.8f,
		195.1f, 181.7f, 175.7f, 194.0f, 187.3f, 184.5f, 185.9f, 165.0f,
		177.5f, 191.5f, 187.1f, 174.9f, 170.5f, 191.5f, 188.1f, 194.7f,
		178.4f, 171.0f, 293.3f, 287.0f, 301.4f, 310.5f, 293.6f, 306.0f,
		302.6f, 284.4f, 295.9f, 312.7f, 299.3f, 306.7f, 310.1f, 295.2f,
		307.3f, 290.9f, 287.2f, 287.4f, 298.7f, 293.7f, 286.9f, 313.9f,
		304.7f, 315.9f, 300.6f, 285.7f, 297.3f, 285.0f, 87.5f, 97.8f,
		104.1f, 94.1f, 92.1f, 104.3f, 82.1f, 75.5f, 98.7f, 84.3f,
		101.0f, 97.7f, 89.9f, 86.2f, 80.4f, 77.3f, 74.4f, 97.9f,
		105.4f, 102.8f, 89.2f, 89.7f, 79.8f, 81.3f, 104.1f, 77.3f,
		75.6f, 105.6f, 97.8f, 83.1f, 93.4f, 74.9f, 91.9f, 245.5f,
		241.8f, 246.6f, 231.0f, 232.9f, 237.2f, 232.2f, 252.8f, 238.1f,
		228.5f, 247.8f, 251.6f, 232.0f, 241.6f, 252.6f, 236.3f, 227.1f,
		155.1f, 162.8f, 141.8f, 145.1f, 149.1f, 156.7f, 139.6f, 147.4f,
		142.8f, 162.4f, 137.8f, 140.7f, 137.8f, 150.6f, 149.8f, 162.0f,
		162.5f, 152.4f, 308.9f, 291.8f, 310.1f, 305.7f, 298.6f, 292.5f,
		290.2f, 302.7f, 292.2f, 309.4f, 298.9f, 266.4f, 262.2f, 258.7f,
		268.8f, 276.9f, 257.5f, 282.8f, 264.0f, 277.0f, 276.3f, 282.5f,
		264.0f, 257.4f, 274.1f, 264.9f, 282.5f, 282.1f, 270.8f, 273.1f,
		269.9f, 26.9f, 18.6f, 33.7f, 20.2f, 32.8f, 23.7f, 41.5f,
		18.2f, 28.1f, 24.3f, 36.8f, 32.2f, 22.6f, 37.8f, 17.3f,
		43.0f, 29.0f, 22.3f, 28.6f, 38.6f, 17.5f, 33.1f,
		26.1f, 103.7f, 206.3f, 340.1f, 357.5f, 125.1f,
		58.1f, 172.6f, 15.5f, 353.9f, 356.4f, 2.9f,
		8.3f,
	};
	vector<float> zRotations = {
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f,
	};

	for (int i = 0; i < (int)modelAddresses.size(); i++)
	{
		auto actor = make_shared<Actor>();
		auto mc    = actor->AddComponent<ModelComponent>();

		auto& t = actor->GetTransform()->m_Transform;
		t._11 = sizes[i];      t._12 = sizes[i];      t._13 = sizes[i];
		t._21 = xRotations[i]; t._22 = yRotations[i]; t._23 = zRotations[i];
		t._31 = xPositions[i]; t._32 = yPositions[i]; t._33 = zPositions[i];

		// Initialize registers the component with RenderSystem automatically.
		result = mc->Initialize(m_D3D->GetDevice(), modelAddresses[i].c_str(), textureAddresses[i].c_str());
		if (!result)
		{
			MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
			return false;
		}

		m_ActorList.push_back(actor);
	}

	// Wind Turbine Rotors
	{
		const float towerData[5][5] = {
			{ 17.0f,  74.5f, 61.4f, 40.5f,  15.5f },
			{ -7.0f,  72.8f, 65.2f, 42.0f, 353.9f },
			{ -3.6f,  79.3f, 57.4f, 35.9f, 356.4f },
			{  2.4f,  85.8f, 47.2f, 28.8f,   2.9f },
			{ 10.0f,  69.1f, 68.7f, 45.1f,   8.3f },
		};
		for (int i = 0; i < 5; i++)
		{
			float px = towerData[i][0], py = towerData[i][1], pz = towerData[i][2];
			float len = sqrtf(px * px + py * py + pz * pz);
			float rx = px + (px / len) * 20.0f;
			float ry = py + (py / len) * 20.0f;
			float rz = pz + (pz / len) * 20.0f;

			auto rotor = make_shared<WindTurbineRotor>(
				20.0f, towerData[i][3], towerData[i][4], XMFLOAT3(rx, ry, rz), 2.0f);
			auto mc = rotor->AddComponent<ModelComponent>();

			result = mc->Initialize(m_D3D->GetDevice(), L"./data/WindTurbineRotor.obj", L"./data/WindTurbineRotor_Tex.dds");
			if (!result)
			{
				MessageBox(hwnd, L"Could not initialize rotor model.", L"Error", MB_OK);
				return false;
			}
			m_ActorList.push_back(rotor);
		}
	}

	// Moon billboard
	{
		auto bb = make_shared<BillBoardClass>(XMFLOAT3(0.0f, 424.0f, 424.0f), 100.0f, m_Camera);
		auto mc = bb->AddComponent<ModelComponent>();
		result = mc->Initialize(m_D3D->GetDevice(), L"./data/Quad.obj", L"./data/Moon.dds");
		if (!result)
		{
			MessageBox(hwnd, L"Could not initialize moon.", L"Error", MB_OK);
			return false;
		}
		mc->b_isBillboard = true;
		m_ActorList.push_back(bb);
	}

	// Player
	{
		m_Player = make_shared<PlayerClass>(input);

		auto playerMC = m_Player->AddComponent<ModelComponent>();
		result = playerMC->Initialize(m_D3D->GetDevice(), L"./data/Spaceship.obj", L"./data/Spaceship_Tex.dds");
		if (!result)
		{
			MessageBox(hwnd, L"Could not initialize the player model.", L"Error", MB_OK);
			return false;
		}
	}
	
	//UI
	{
		m_Bitmap = new BitmapClass;
		if (!m_Bitmap)
		{
			return false;
		}

		// Initialize the bitmap object.
		result = m_Bitmap->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"./data/title.dds", screenWidth, screenHeight);
		if (!result)
		{
			MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
			return false;
		}

		m_Text = new TextClass;
		if (!m_Text) return false;

		result = m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, m_baseViewMatrix);
		if (!result)
		{
			MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
			return false;
		}
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	m_Player = nullptr;
	m_ActorList.clear();


	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	if (m_Bitmap)
	{
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = 0;
	}

	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}
	RenderSystem::GetInstance().Shutdown();
}


bool GraphicsClass::Frame(int fps, int cpu)
{
	bool result;

	auto* ctx = m_D3D->GetDeviceContext();
	m_Text->SetFPS(fps, ctx);
	m_Text->SetCPU(cpu, ctx);
	m_Text->SetObjectCount(RenderSystem::GetInstance().GetObjectCount(), ctx);
	m_Text->SetPolygonCount(RenderSystem::GetInstance().GetPolygonCount(), ctx);
	m_Text->SetResolution(m_screenWidth, m_screenHeight, ctx);

	bool enterNow = m_Input->IsKeyPressed(DIK_RETURN);
	if (enterNow && !m_enterWasPressed)
		m_showBitmap = !m_showBitmap;
	m_enterWasPressed = enterNow;

	if (!m_showBitmap)
	{
		m_Player->Update();

		for (auto& actor : m_ActorList)
			actor->Update();

		m_Camera->FollowTarget(
			m_Player->GetPosition(),
			m_Player->GetLocalUp(),
			m_Player->GetLocalForward(),
			3.0f,
			1.5f
		);
	}

	result = Render();
	if (!result) return false;

	return true;
}


void GraphicsClass::SetFilter(D3D11_FILTER filter)
{
	RenderSystem::GetInstance().SetFilter(m_D3D->GetDevice(), filter);
}


bool GraphicsClass::Render()
{
	XMMATRIX viewMatrix, projectionMatrix, worldMatrix, orthoMatrix;
	bool result;

	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	result = RenderSystem::GetInstance().RenderAll(m_D3D->GetDeviceContext(), viewMatrix, projectionMatrix);
	if (!result) return false;

	m_D3D->TurnZBufferOff();
	if (m_showBitmap)
	{
		result = m_Bitmap->Render(m_D3D->GetDeviceContext(), 0, 0);
		if (!result) return false;

		result = RenderSystem::GetInstance().RenderBitmap(
			m_D3D->GetDeviceContext(), m_Bitmap->GetIndexCount(),
			worldMatrix, m_baseViewMatrix, orthoMatrix, m_Bitmap->GetTexture());
		if (!result) return false;
	}
	m_D3D->TurnOnAlphaBlending();
	result = m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if (!result) return false;
	m_D3D->TurnOffAlphaBlending();
	m_D3D->TurnZBufferOn();


	m_D3D->EndScene();

	return true;
}



