#include "ModelComponent.h"
#include "RenderSystem.h"


ModelComponent::ModelComponent()
{
	m_vertexBuffer = 0;
	m_indexBuffer  = 0;
	m_Texture      = 0;
	m_model        = 0;

	m_vertexCount  = 0;
	m_indexCount   = 0;
	m_textureCount = 0;
	m_normalCount  = 0;
	m_faceCount    = 0;

}


ModelComponent::~ModelComponent()
{
	RenderSystem::GetInstance().Unregister(this);
	ReleaseTexture();
	ShutdownBuffers();
	ReleaseModel();
}


bool ModelComponent::Initialize(ID3D11Device* device, const WCHAR* modelFilename, const WCHAR* textureFilename)
{
	m_meshPath = modelFilename;

	if (!RenderSystem::GetInstance().GetMesh(m_meshPath, m_vertexBuffer, m_indexBuffer, m_indexCount))
	{
		if (!LoadModel(modelFilename))    return false;
		if (!InitializeBuffers(device))   return false;
		RenderSystem::GetInstance().RegisterMesh(m_meshPath, m_vertexBuffer, m_indexBuffer, m_indexCount);
	}

	if (!LoadTexture(device, textureFilename)) return false;

	RenderSystem::GetInstance().Register(this);
	return true;
}


void ModelComponent::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}


int ModelComponent::GetIndexCount()
{
	return m_indexCount;
}


ID3D11ShaderResourceView* ModelComponent::GetTexture()
{
	return m_Texture->GetTexture();
}



bool ModelComponent::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	vertices = new VertexType[m_vertexCount];
	if (!vertices) return false;

	indices = new unsigned long[m_indexCount];
	if (!indices) return false;

	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture  = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal   = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
		indices[i] = i;
	}

	vertexBufferDesc.Usage               = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth           = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags      = 0;
	vertexBufferDesc.MiscFlags           = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem          = vertices;
	vertexData.SysMemPitch      = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result)) return false;

	indexBufferDesc.Usage               = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth           = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags      = 0;
	indexBufferDesc.MiscFlags           = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem          = indices;
	indexData.SysMemPitch      = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result)) return false;

	delete[] vertices;
	delete[] indices;

	return true;
}


void ModelComponent::ShutdownBuffers()
{
	// 버퍼 소유권은 RenderSystem의 MeshCache에 있으므로 Release하지 않음
	m_indexBuffer  = 0;
	m_vertexBuffer = 0;
}


void ModelComponent::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


bool ModelComponent::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	m_Texture = new TextureClass;
	if (!m_Texture) return false;
	return m_Texture->Initialize(device, filename);
}


void ModelComponent::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}
}


bool ModelComponent::LoadModel(const WCHAR* filename)
{
	return ReadFileCounts(filename);
}


void ModelComponent::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}
}


bool ModelComponent::ReadFileCounts(const WCHAR* filename)
{
	ifstream fin;
	char input;
	int vertexCount  = 0;
	int textureCount = 0;
	int normalCount  = 0;
	int faceCount    = 0;

	fin.open(filename);
	if (fin.fail()) return false;

	fin.get(input);
	while (!fin.eof())
	{
		if (input == 'v')
		{
			fin.get(input);
			if (input == ' ') { vertexCount++; }
			if (input == 't') { textureCount++; }
			if (input == 'n') { normalCount++; }
		}

		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ') { faceCount++; }
		}

		while (input != '\n')
		{
			fin.get(input);
			if (fin.eof()) break;
		}

		fin.get(input);
	}

	fin.close();

	return LoadDataStructures(filename, vertexCount, textureCount, normalCount, faceCount);
}


bool ModelComponent::LoadDataStructures(const WCHAR* filename, int vertexCount, int textureCount, int normalCount, int faceCount)
{
	XMFLOAT3* vertices  = new XMFLOAT3[vertexCount];
	XMFLOAT3* texcoords = new XMFLOAT3[textureCount];
	XMFLOAT3* normals   = new XMFLOAT3[normalCount];
	FaceType*  faces    = new FaceType[faceCount];

	if (!vertices || !texcoords || !normals || !faces) return false;

	int vertexIndex   = 0;
	int texcoordIndex = 0;
	int normalIndex   = 0;
	int faceIndex     = 0;

	ifstream fin;
	fin.open(filename);
	if (fin.fail()) return false;

	char input, input2;
	fin.get(input);
	while (!fin.eof())
	{
		if (input == 'v')
		{
			fin.get(input);

			if (input == ' ')
			{
				fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;
				vertices[vertexIndex].z *= -1.0f;
				vertexIndex++;
			}

			if (input == 't')
			{
				fin >> texcoords[texcoordIndex].x >> texcoords[texcoordIndex].y;
				texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
				texcoordIndex++;
			}

			if (input == 'n')
			{
				fin >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;
				normals[normalIndex].z *= -1.0f;
				normalIndex++;
			}
		}

		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ')
			{
				fin >> faces[faceIndex].vIndex3 >> input2 >> faces[faceIndex].tIndex3 >>
					input2 >> faces[faceIndex].nIndex3 >> faces[faceIndex].vIndex2 >> input2 >>
					faces[faceIndex].tIndex2 >> input2 >> faces[faceIndex].nIndex2 >>
					faces[faceIndex].vIndex1 >> input2 >> faces[faceIndex].tIndex1 >> input2 >>
					faces[faceIndex].nIndex1;
				faceIndex++;
			}
		}

		while (input != '\n')
		{
			fin.get(input);
			if (fin.eof()) break;
		}

		fin.get(input);
	}

	fin.close();

	m_vertexCount = faceCount * 3;
	m_indexCount  = m_vertexCount;

	m_model = new ModelType[m_vertexCount];
	if (!m_model) return false;

	for (int i = 0; i < faceIndex; i++)
	{
		int vIndex, tIndex, nIndex;

		vIndex = faces[i].vIndex1 - 1; tIndex = faces[i].tIndex1 - 1; nIndex = faces[i].nIndex1 - 1;
		m_model[i * 3].x  = vertices[vIndex].x;  m_model[i * 3].y  = vertices[vIndex].y;  m_model[i * 3].z  = vertices[vIndex].z;
		m_model[i * 3].tu = texcoords[tIndex].x;  m_model[i * 3].tv = texcoords[tIndex].y;
		m_model[i * 3].nx = normals[nIndex].x;    m_model[i * 3].ny = normals[nIndex].y;   m_model[i * 3].nz = normals[nIndex].z;

		vIndex = faces[i].vIndex2 - 1; tIndex = faces[i].tIndex2 - 1; nIndex = faces[i].nIndex2 - 1;
		m_model[i * 3 + 1].x  = vertices[vIndex].x;  m_model[i * 3 + 1].y  = vertices[vIndex].y;  m_model[i * 3 + 1].z  = vertices[vIndex].z;
		m_model[i * 3 + 1].tu = texcoords[tIndex].x;  m_model[i * 3 + 1].tv = texcoords[tIndex].y;
		m_model[i * 3 + 1].nx = normals[nIndex].x;    m_model[i * 3 + 1].ny = normals[nIndex].y;   m_model[i * 3 + 1].nz = normals[nIndex].z;

		vIndex = faces[i].vIndex3 - 1; tIndex = faces[i].tIndex3 - 1; nIndex = faces[i].nIndex3 - 1;
		m_model[i * 3 + 2].x  = vertices[vIndex].x;  m_model[i * 3 + 2].y  = vertices[vIndex].y;  m_model[i * 3 + 2].z  = vertices[vIndex].z;
		m_model[i * 3 + 2].tu = texcoords[tIndex].x;  m_model[i * 3 + 2].tv = texcoords[tIndex].y;
		m_model[i * 3 + 2].nx = normals[nIndex].x;    m_model[i * 3 + 2].ny = normals[nIndex].y;   m_model[i * 3 + 2].nz = normals[nIndex].z;
	}

	delete[] vertices;
	delete[] texcoords;
	delete[] normals;
	delete[] faces;

	return true;
}
