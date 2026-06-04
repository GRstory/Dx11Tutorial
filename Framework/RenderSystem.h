#pragma once
#include <map>
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

    bool Initialize(ID3D11Device* device, HWND hwnd);
    void Shutdown();

    void Register(ModelComponent* component);
    void Unregister(ModelComponent* component);

    bool RenderAll(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);
    bool RenderBitmap(ID3D11DeviceContext* context, int indexCount, XMMATRIX world, XMMATRIX view, XMMATRIX proj, ID3D11ShaderResourceView* texture);
    void SetFilter(ID3D11Device* device, D3D11_FILTER filter);

    int GetPolygonCount() const { return m_polygonCount; }
    int GetObjectCount()  const { return m_objectCount; }

private:
    TextureShaderClass* m_Shader = nullptr;
    map<ModelComponent*, TransformComponent*> _components;
    int m_polygonCount = 0;
    int m_objectCount  = 0;
};
