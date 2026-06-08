#pragma once
#include <map>
#include <string>
#include <d3d11.h>
#include <directxmath.h>
#include "textureshaderclass.h"
using namespace DirectX;
using namespace std;

class ModelComponent;
class TransformComponent;

class RenderSystem
{
private:
    RenderSystem() = default;
    ~RenderSystem() = default;
    RenderSystem(const RenderSystem&) = delete;
    RenderSystem& operator=(const RenderSystem&) = delete;

public:
    static RenderSystem& GetInstance();

    bool Initialize(ID3D11Device* device, HWND hwnd, class D3DClass* d3d);
    void Shutdown();

    void Register(ModelComponent* component);
    void Unregister(ModelComponent* component);

    bool RenderAll(ID3D11DeviceContext* context, CXMMATRIX viewMatrix, CXMMATRIX projectionMatrix);
    bool RenderBitmap(ID3D11DeviceContext* context, int indexCount, CXMMATRIX world, CXMMATRIX view, CXMMATRIX proj, ID3D11ShaderResourceView* texture);
    void SetFilter(ID3D11Device* device, D3D11_FILTER filter);

    int GetPolygonCount() const { return m_polygonCount; }
    int GetObjectCount()  const { return m_objectCount; }

    bool GetMesh(const wstring& path, ID3D11Buffer*& outVB, ID3D11Buffer*& outIB, int& outIndexCount);
    void RegisterMesh(const wstring& path, ID3D11Buffer* vb, ID3D11Buffer* ib, int indexCount);

private:
    struct MeshEntry {
        ID3D11Buffer* vertexBuffer = nullptr;
        ID3D11Buffer* indexBuffer  = nullptr;
        int           indexCount   = 0;
    };

    class D3DClass* m_D3D;
    TextureShaderClass* m_Shader = nullptr;
    map<ModelComponent*, TransformComponent*> _components;
    map<wstring, MeshEntry> m_meshCache;
    int m_polygonCount = 0;
    int m_objectCount  = 0;
};
