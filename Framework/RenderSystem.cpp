#include "RenderSystem.h"
#include "ModelComponent.h"
#include "Actor.h"
#include "TransformComponent.h"
#include "d3dclass.h"


RenderSystem& RenderSystem::GetInstance()
{
    static RenderSystem instance;
    return instance;
}


bool RenderSystem::Initialize(ID3D11Device* device, HWND hwnd, D3DClass* d3d)
{
    m_Shader = new TextureShaderClass;
    m_D3D = d3d;
    if (!m_Shader) return false;
    return m_Shader->Initialize(device, hwnd);
}


void RenderSystem::Shutdown()
{
    if (m_Shader)
    {
        m_Shader->Shutdown();
        delete m_Shader;
        m_Shader = nullptr;
    }
    _components.clear();

    for (auto& pair : m_meshCache)
    {
        if (pair.second.vertexBuffer) pair.second.vertexBuffer->Release();
        if (pair.second.indexBuffer)  pair.second.indexBuffer->Release();
    }
    m_meshCache.clear();
}


bool RenderSystem::GetMesh(const wstring& path, ID3D11Buffer*& outVB, ID3D11Buffer*& outIB, int& outIndexCount)
{
    auto it = m_meshCache.find(path);
    if (it == m_meshCache.end()) return false;

    outVB         = it->second.vertexBuffer;
    outIB         = it->second.indexBuffer;
    outIndexCount = it->second.indexCount;
    return true;
}


void RenderSystem::RegisterMesh(const wstring& path, ID3D11Buffer* vb, ID3D11Buffer* ib, int indexCount)
{
    m_meshCache[path] = { vb, ib, indexCount };
}


void RenderSystem::Register(ModelComponent* component)
{
    auto transform = component->GetOwner().lock()->GetTransform().get();
    _components[component] = transform;
}


void RenderSystem::Unregister(ModelComponent* component)
{
    _components.erase(component);
}


bool RenderSystem::RenderAll(ID3D11DeviceContext* context, CXMMATRIX viewMatrix, CXMMATRIX projectionMatrix)
{
    m_polygonCount = 0;
    m_objectCount  = 0;

    for (auto& pair : _components)
    {
        ModelComponent* modelComponent = pair.first;
        TransformComponent* transform = pair.second;

        if (modelComponent->b_isBillboard)
        {
            m_D3D->TurnOnAlphaBlending();
        }

        modelComponent->Render(context);
        if (!m_Shader->Render(context, modelComponent->GetIndexCount(),
            transform->GetWorldMatrix(), viewMatrix, projectionMatrix, modelComponent->GetTexture()))
            return false;

        m_polygonCount += modelComponent->GetIndexCount() / 3;
        m_objectCount++;

        if (modelComponent->b_isBillboard)
        {
            m_D3D->TurnOffAlphaBlending();
        }
    }
    return true;
}


bool RenderSystem::RenderBitmap(ID3D11DeviceContext* context, int indexCount, CXMMATRIX world, CXMMATRIX view, CXMMATRIX proj, ID3D11ShaderResourceView* texture)
{
    return m_Shader->Render(context, indexCount, world, view, proj, texture);
}


void RenderSystem::SetFilter(ID3D11Device* device, D3D11_FILTER filter)
{
    if (m_Shader)
        m_Shader->SetSamplerFilter(device, filter);
}
