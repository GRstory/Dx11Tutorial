#include "RenderSystem.h"
#include "ModelComponent.h"
#include "Actor.h"
#include "TransformComponent.h"


RenderSystem& RenderSystem::GetInstance()
{
    static RenderSystem instance;
    return instance;
}


bool RenderSystem::Initialize(ID3D11Device* device, HWND hwnd)
{
    m_Shader = new TextureShaderClass;
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


bool RenderSystem::RenderAll(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
    m_polygonCount = 0;
    m_objectCount  = 0;

    for (auto& pair : _components)
    {
        ModelComponent* modelComponent = pair.first;
        TransformComponent* transform = pair.second;

        modelComponent->Render(context);
        if (!m_Shader->Render(context, modelComponent->GetIndexCount(),
            transform->GetWorldMatrix(), viewMatrix, projectionMatrix, modelComponent->GetTexture()))
            return false;

        m_polygonCount += modelComponent->GetIndexCount() / 3;
        m_objectCount++;
    }
    return true;
}


bool RenderSystem::RenderBitmap(ID3D11DeviceContext* context, int indexCount, XMMATRIX world, XMMATRIX view, XMMATRIX proj, ID3D11ShaderResourceView* texture)
{
    return m_Shader->Render(context, indexCount, world, view, proj, texture);
}


void RenderSystem::SetFilter(ID3D11Device* device, D3D11_FILTER filter)
{
    if (m_Shader)
        m_Shader->SetSamplerFilter(device, filter);
}
