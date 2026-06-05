#pragma once
#include "Component.h"
#include <d3d11.h>
#include <directxmath.h>
#include "textureclass.h"
#include <fstream>
using namespace DirectX;
using namespace std;

class ModelComponent : public Component
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	struct FaceType
	{
		int vIndex1, vIndex2, vIndex3;
		int tIndex1, tIndex2, tIndex3;
		int nIndex1, nIndex2, nIndex3;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	ModelComponent();
	virtual ~ModelComponent();

	virtual void Update() override {}

	bool Initialize(ID3D11Device*, const WCHAR* modelFilename, const WCHAR* textureFilename);
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, const WCHAR*);
	void ReleaseTexture();

	bool LoadModel(const WCHAR*);
	void ReleaseModel();

	bool ReadFileCounts(const WCHAR*);
	bool LoadDataStructures(const WCHAR*, int, int, int, int);

public:
	bool b_isBillboard = false;

private:
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer  = nullptr;
	int m_vertexCount, m_indexCount, m_textureCount, m_normalCount, m_faceCount;
	TextureClass* m_Texture  = nullptr;
	ModelType*    m_model    = nullptr;
	wstring       m_meshPath;
};
