#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <memory>
#include <wrl/client.h>
#include <IService.h>
#include <Util/Utils.h>
#include <Defines/VertexDefinition.h>

// Forward declarations para managers que UIText utiliza
//class MeshAsset;
class Font;
class DeviceManager;
class MeshAsset;

// Estructura de vértice (asegúrate de que coincide con tu shader)
struct UIVertex {
    DirectX::XMFLOAT3 Position;
    DirectX::XMFLOAT2 TexCoord;
    DirectX::XMFLOAT4 Color;
};

class UIText : public IService {
public:
    // Constructor de UIText, recibe dependencias
    UIText();
    UIText(std::shared_ptr<MeshAsset> mesh);
    ~UIText();

    HRESULT Init() override;
    //HRESULT Init(float width, float height);
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

    virtual std::unique_ptr<UIText> Clone() const {
        // Crea una nueva instancia utilizando el constructor de copia
        // y la devuelve como un shared_ptr.
        return std::make_unique<UIText>(*this);
    }

	std::shared_ptr<MeshAsset> GetMesh() { return m_mesh; }

    // Setters para configurar el texto y su apariencia
    void SetText(const std::string& text);
    void SetPosition(float x, float y);
    DirectX::XMFLOAT3 GetPosition();
    void SetColor(float r, float g, float b, float a);
    void SetFontSize(float size);
    const std::string& GetText() const { return m_text; }

	void SetMesh(std::shared_ptr<MeshAsset> mesh) { m_mesh = mesh; }
    void CreateMesh(std::vector<std::shared_ptr<VertexDefinition::VertexVariant>>& pVertexData);

    // Render ahora solo toma la matriz ortográfica global (o específica para esta capa de UI)
    // Usará los miembros internos (m_text, m_position, etc.) para dibujar.

private:
	std::shared_ptr<MeshAsset> m_mesh;

	float m_screenWidth;
	float m_screenHeight;

    XMFLOAT3 m_screenOffset;
    
    std::string m_text;
    Font* m_font;
    DirectX::XMFLOAT3 m_position;
    DirectX::XMFLOAT4 m_color;
    float m_fontSize;

    std::shared_ptr<DeviceManager> m_device;    

    ID3D11Buffer* m_vertexBuffer; // Gestionado dinámicamente

    UINT CreateVertexBufferZZZ();
    void SetVertexBufferZZZ(ID3D11DeviceContext* context);
    UINT CreateVertexBuffer();
    void SetVertexBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
    //void ApplyMaterial(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Material> m_material, XMMATRIX orthoMatrix);
};
