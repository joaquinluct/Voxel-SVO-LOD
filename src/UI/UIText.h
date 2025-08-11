#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <memory>
#include <wrl/client.h>
#include <Util/Utils.h>
#include <IService.h>
#include <Defines/VertexDefinition.h>
#include <Defines/UI.h>

// Forward declarations para managers que UIText utiliza
//class MeshAsset;
class UIManager;
class DeviceManager;
class Material;
class WorldMatrixManager;

// Estructura de vértice (asegúrate de que coincide con tu shader)
struct UIVertex {
    DirectX::XMFLOAT3 Position;
    DirectX::XMFLOAT2 TexCoord;
    DirectX::XMFLOAT4 Color;
};

class UIText : public IService {
public:
    // Constructor de UIText, recibe dependencias
    UIText() : m_color{}, m_fontSize{}, m_position{}, m_vertexBuffer{} {
        m_font = new Font(32, DirectX::XMFLOAT4{1.0f, 1.0f, 1.0f, 1.0f});
    };
    UIText(std::shared_ptr<DeviceManager> deviceManager,
        std::shared_ptr<WorldMatrixManager> worldMatrixManager,
        Material* material);
    ~UIText();

    HRESULT Init() override;
    HRESULT Init(float width, float height);
    void Render() override {};
    //void Render(const DirectX::XMMATRIX& orthoMatrix);
    void Update(float deltaTime) override {};
    void Release() { Shutdown(); };
    void Shutdown() override;

    const std::string& GetServiceName() const override {
        static const std::string name = "UIText"; // Esta cadena se crea una sola vez y vive durante toda la ejecución del programa.
        return name;
    }
    static const std::string& GetStaticServiceName()
    {
        static const std::string name = "UIText"; // Esta cadena se crea una sola vez y vive durante toda la ejecución del programa.
        return name;
    }

    // Setters para configurar el texto y su apariencia
    void SetText(const std::string& text);
    void SetPosition(float x, float y);
    DirectX::XMFLOAT3 GetPosition();
    void SetColor(float r, float g, float b, float a);
    void SetFontSize(float size);

    void CreateMesh(std::vector<std::shared_ptr<VertexDefinition::VertexVariant>>& pVertexData);

    // Render ahora solo toma la matriz ortográfica global (o específica para esta capa de UI)
    // Usará los miembros internos (m_text, m_position, etc.) para dibujar.

private:
	float m_screenWidth;
	float m_screenHeight;

    XMFLOAT3 m_screenOffset;
    
    std::string m_text;
    Font* m_font;
    DirectX::XMFLOAT3 m_position;
    DirectX::XMFLOAT4 m_color;
    float m_fontSize;

    std::shared_ptr<DeviceManager> m_device;
    //std::shared_ptr<WorldMatrixManager> m_worldMatrixManager;
    //std::shared_ptr<Material> m_material;
    std::shared_ptr<UIManager> m_uiManager;

    ID3D11Buffer* m_vertexBuffer; // Gestionado dinámicamente

    UINT CreateVertexBufferZZZ();
    void SetVertexBufferZZZ(ID3D11DeviceContext* context);
    UINT CreateVertexBuffer();
    void SetVertexBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
    //void ApplyMaterial(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Material> m_material, XMMATRIX orthoMatrix);
};