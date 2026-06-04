////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"
#include "textclass.h"
#include "BitmapClass.h"

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
	result = RenderSystem::GetInstance().Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize RenderSystem.", L"Error", MB_OK);
		return false;
	}

	// Model data
	vector<wstring> modelAddresses   = { L"./data/Planet.obj", L"./data/Mountain_1.obj", L"./data/Volcano.obj" };
	vector<wstring> textureAddresses = { L"./data/grass.dds",  L"./data/Mountain_1_Tex.dds",  L"./data/Volcano_Tex.dds" };
	vector<float>   sizes            = { 1.0f, 30.0f, 30.0f, 1.0f };
	vector<float>   xPositions       = { 0.0f,  0.0f,   0.0f, 0.0f };
	vector<float>   yPositions       = { 0.0f, 98.0f, -98.0f, 0.0f };
	vector<float>   zPositions       = { 0.0f,  0.0f,   0.0f, 0.0f };
	vector<float>   xRotations       = { 0.0f,  0.0f, 180.0f, 0.0f };
	vector<float>   yRotations       = { 0.0f,  0.0f,   0.0f, 0.0f };
	vector<float>   zRotations       = { 0.0f,  0.0f,   0.0f, 0.0f };

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

	// Create and initialize the player.
	m_Player = make_shared<PlayerClass>(input);

	auto playerMC = m_Player->AddComponent<ModelComponent>();
	result = playerMC->Initialize(m_D3D->GetDevice(), L"./data/Spaceship.obj", L"./data/Spaceship_Tex.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the player model.", L"Error", MB_OK);
		return false;
	}

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

	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
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
