#include <DirectXMath.h>
#include "UIText.h"
#include <Assets/Base/MeshAsset.h>
#include <ManagerLocator/ManagerLocator.h>
#include "DeviceManager.h"
#include <windows.h> // Para OutputDebugStringA
#include <algorithm> // Para std::max
#include <cmath>     // Para roundf
#include <Util/DirectXUtils.h>
#include <Util/Utils.h>
#include <Defines/Matrix/MatrixDefinitionBase.h>
#include <Defines/UI.h>
#include <REGISTER_SERVICE_MACRO.h>

REGISTER_SERVICE_TYPE(UIText, "UIText")

UIText::UIText() :
    m_color{}, m_fontSize{}, m_position{}, m_vertexBuffer{}, m_screenWidth{ 800.0f }, m_screenHeight{ 600.0f }, m_screenOffset{ 0.0f, 0.0f, 0.0f }, m_text("Sample Text")
{
    m_font = new Font(32, DirectX::XMFLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f });
};

UIText::UIText(std::shared_ptr<MeshAsset> mesh)
    : m_mesh(mesh),
    m_device(),
    //m_material(nullptr),
    m_screenHeight(0),
    m_screenWidth(0),
    m_vertexBuffer(nullptr),
    m_text(""),                  // Texto por defecto vacío
    m_position(0.0f, 0.0f, 0.0f), // Posición por defecto
    m_color(1.0f, 1.0f, 1.0f, 1.0f), // Color blanco por defecto
    m_fontSize(32.0f)             // Tamaño de fuente por defecto    
{
    m_font = new Font(32, DirectX::XMFLOAT4{ 1.0f , 1.0f, 1.0f, 1.0f });
    // No es necesario inicializar el vertex buffer aquí, se gestiona en Render
}

UIText::~UIText() {}

HRESULT UIText::Init() {
    m_device = ManagerLocator::GetDeviceManager();
    if (!m_device) {
        OutputDebugStringA("Error: UIText::Init - DeviceManager is null.\n");
        return E_FAIL;
	}	
    m_screenWidth = m_device->GetWidth();
	m_screenHeight = m_device->GetHeight();
    m_screenOffset = DirectX::XMFLOAT3(-(m_screenWidth / 2.0f) + 20.0f, -(m_screenHeight / 2.0f) + 20.0f, 0.0f);
    return S_OK;
}

void UIText::SetText(const std::string& text) {
    m_text = text;
    m_mesh->UpdateTextMesh(this, text);

	//m_uiManager->UpdateText(GetMesh()->GetName(), text);
    //m_text = text;
    //// Opcional: Podrías añadir una bandera aquí para indicar que el buffer necesita ser regenerado
    //// si el texto cambia y su tamaño afecta la capacidad del buffer.
    //m_mesh->InitMesh();
}

DirectX::XMFLOAT3 UIText::GetPosition() {
	return DirectXUtils::Add(m_position, m_screenOffset);
}

void UIText::SetPosition(float x, float y) {
    m_position = DirectX::XMFLOAT3(x, y, 0.0f);
}

void UIText::SetColor(float r, float g, float b, float a) {
    m_color = DirectX::XMFLOAT4(r, g, b, a);
	m_font->SetColor(m_color);
}

void UIText::SetFontSize(float size) {
    m_fontSize = size;
    m_font->SetSize(static_cast<int>(size));
}

void UIText::CreateMesh(std::vector<std::shared_ptr<VertexDefinition::VertexVariant>>& pVertexData) {

	//m_position = DirectX::XMFLOAT3(-970.0f, -450.0f, 0.0f); // Aseguramos que la posición se inicializa a (0, 0, 0)
    m_fontSize = static_cast<float>(m_font->GetSize()); // Aseguramos que el tamaño de fuente se obtiene del objeto Font
	m_color = m_font->GetColor(); // Aseguramos que el color se obtiene del objeto Font

    float charWidth =  m_fontSize;
    float charHeight = m_fontSize;

    int atlasColumns = 16;
    int atlasRows = 16;
        
    float x = roundf(GetPosition().x);
    float y = roundf(GetPosition().y);

    if (m_text.empty()) {
        return;
    }

    for (char c : m_text) {
        int charIndex = static_cast<int>(c);
        int charColumn = charIndex % atlasColumns;
        int charRow = charIndex / atlasColumns;

        float u0 = static_cast<float>(charColumn) / static_cast<float>(atlasColumns);
        float v0 = static_cast<float>(charRow) / static_cast<float>(atlasRows);
        float u1 = static_cast<float>(charColumn + 1) / static_cast<float>(atlasColumns);
        float v1 = static_cast<float>(charRow + 1) / static_cast<float>(atlasRows);

        DirectX::XMFLOAT3 p1 = { x, y, 0.0f };
        DirectX::XMFLOAT3 p2 = { x + charWidth, y, 0.0f };
        DirectX::XMFLOAT3 p3 = { x, y + charHeight, 0.0f };

        pVertexData.push_back(std::make_shared<VertexDefinition::VertexVariant>(VertexDefinition::TextVertex(p1, DirectX::XMFLOAT2(u0, v0), m_color)));
        pVertexData.push_back(std::make_shared<VertexDefinition::VertexVariant>(VertexDefinition::TextVertex(p2, DirectX::XMFLOAT2(u1, v0), m_color)));
        pVertexData.push_back(std::make_shared<VertexDefinition::VertexVariant>(VertexDefinition::TextVertex(p3, DirectX::XMFLOAT2(u0, v1), m_color)));

        pVertexData.push_back(std::make_shared<VertexDefinition::VertexVariant>(VertexDefinition::TextVertex(DirectX::XMFLOAT3(x, y + charHeight, 0.0f), DirectX::XMFLOAT2(u0, v1), m_color)));
        pVertexData.push_back(std::make_shared<VertexDefinition::VertexVariant>(VertexDefinition::TextVertex(DirectX::XMFLOAT3(x + charWidth, y, 0.0f), DirectX::XMFLOAT2(u1, v0), m_color)));
        pVertexData.push_back(std::make_shared<VertexDefinition::VertexVariant>(VertexDefinition::TextVertex(DirectX::XMFLOAT3(x + charWidth, y + charHeight, 0.0f), DirectX::XMFLOAT2(u1, v1), m_color)));

        x += charWidth * 0.3f;
        x = roundf(x);
    }
}

UINT UIText::CreateVertexBufferZZZ() {

    // --- Gestión del tamaño del Vertex Buffer ---
    // ¡Usamos m_text, m_fontSize, m_position, m_color!
    UINT requiredVertexCount = static_cast<UINT>(3);
    UINT currentBufferSize = 0;

    if (m_vertexBuffer) {
        D3D11_BUFFER_DESC desc;
        m_vertexBuffer->GetDesc(&desc);
        currentBufferSize = desc.ByteWidth / sizeof(UIVertex);
    }

    if (!m_vertexBuffer || requiredVertexCount > currentBufferSize) {
        if (m_vertexBuffer) {
            m_vertexBuffer->Release();
            m_vertexBuffer = nullptr;
        }

        UINT bufferCapacity = (std::max)(requiredVertexCount, 6U * 10U); // Mínimo para 10 caracteres

        //Añadido para debug
        OutputDebugStringA(("bufferCapacity: " + std::to_string(bufferCapacity) + "\n").c_str());

        D3D11_BUFFER_DESC vertexBufferDesc = {};
        vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        vertexBufferDesc.ByteWidth = bufferCapacity * sizeof(UIVertex);
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        vertexBufferDesc.MiscFlags = 0;
        vertexBufferDesc.StructureByteStride = 0;

        HRESULT hr = m_device->GetDevice()->CreateBuffer(&vertexBufferDesc, nullptr, &m_vertexBuffer);
        if (FAILED(hr)) {
            OutputDebugStringA("Error al crear/redimensionar el vertex buffer para UIText.\n");
            return 0;
        }
    }

    return requiredVertexCount;
}

UINT UIText::CreateVertexBuffer() {

    // --- Gestión del tamaño del Vertex Buffer ---
    // ¡Usamos m_text, m_fontSize, m_position, m_color!
    UINT requiredVertexCount = static_cast<UINT>(m_text.length() * 6);
    UINT currentBufferSize = 0;

    if (m_vertexBuffer) {
        D3D11_BUFFER_DESC desc;
        m_vertexBuffer->GetDesc(&desc);
        currentBufferSize = desc.ByteWidth / sizeof(UIVertex);
    }

    if (!m_vertexBuffer || requiredVertexCount > currentBufferSize) {
        if (m_vertexBuffer) {
            m_vertexBuffer->Release();
            m_vertexBuffer = nullptr;
        }

        UINT bufferCapacity = (std::max)(requiredVertexCount, 6U * 10U); // Mínimo para 10 caracteres

        //Añadido para debug
        OutputDebugStringA(("bufferCapacity: " + std::to_string(bufferCapacity) + "\n").c_str());

        D3D11_BUFFER_DESC vertexBufferDesc = {};
        vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        vertexBufferDesc.ByteWidth = bufferCapacity * sizeof(UIVertex);
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        vertexBufferDesc.MiscFlags = 0;
        vertexBufferDesc.StructureByteStride = 0;

        HRESULT hr = m_device->GetDevice()->CreateBuffer(&vertexBufferDesc, nullptr, &m_vertexBuffer);
        if (FAILED(hr)) {
            OutputDebugStringA("Error al crear/redimensionar el vertex buffer para UIText.\n");
            return 0;
        }
    }

    return requiredVertexCount;
}

void UIText::SetVertexBufferZZZ(ID3D11DeviceContext* context) {
    UIVertex vertices[] = {
    { XMFLOAT3(0.0f,  0.5f, 0.0f), XMFLOAT2(.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, .0f, 1.0f)},
    { XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT2(.05f, .3f), XMFLOAT4(1.0f, 1.0f, .0f, 1.0f) },
    { XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT2(.8f, 1.0f), XMFLOAT4(1.0f, 1.0f, .0f, 1.0f) }
    };
    

    // Vertex Buffer
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.ByteWidth = sizeof(vertices);
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = vertices;

    HRESULT hr = m_device->GetDevice()->CreateBuffer(&vbDesc, &vbData, &m_vertexBuffer);
}

void DebugVertextData(UIVertex* pInitialVertexData, int textLenght) {
    int numVerticesWritten = textLenght * 6;

    char debugBuffer[256]; // Buffer para formatear la cadena de depuración

    for (int i = 0; i < numVerticesWritten; ++i) {
        const UIVertex& vertex = pInitialVertexData[i];

        // Formato para OutputDebugStringA (necesitas convertir floats a string)
        // Puedes usar sprintf_s o std::to_string
        // Nota: std::to_string requiere C++11 o superior.
        // Asegúrate de que UIVertex tenga miembros position, texCoord, color.
        sprintf_s(debugBuffer, sizeof(debugBuffer),
            "Vertex %d: Pos(%.2f, %.2f, %.2f) Tex(%.2f, %.2f) Color(%.2f, %.2f, %.2f, %.2f)\n",
            i,
            vertex.Position.x, vertex.Position.y, vertex.Position.z,
            vertex.TexCoord.x, vertex.TexCoord.y,
            vertex.Color.x, vertex.Color.y, vertex.Color.z, vertex.Color.w);
        OutputDebugStringA(debugBuffer);
    }
}

void UIText::SetVertexBuffer(Microsoft::WRL::ComPtr < ID3D11DeviceContext> context) {

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    UIVertex* pVertexData;
    UIVertex* pInitialVertexData; // Nueva variable

    HRESULT hr = context->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(hr)) {
        OutputDebugStringA("Error al mapear el vertex buffer para UIText.\n");
        return;
    }
    pVertexData = (UIVertex*)mappedResource.pData;
    pInitialVertexData = pVertexData;

    // Usamos m_fontSize del miembro de clase
    float charWidth = m_fontSize;
    float charHeight = m_fontSize;

    int atlasColumns = 16;
    int atlasRows = 16;

    // Usamos m_position del miembro de clase
    float x = roundf(m_position.x);
    float y = roundf(m_position.y);

    // Bucle para poblar los datos de los vértices para cada carácter
    // ¡Usamos m_text del miembro de clase!
    float div = 100.0f;
    for (char c : m_text) {
        int charIndex = static_cast<int>(c);
        int charColumn = charIndex % atlasColumns;
        int charRow = charIndex / atlasColumns;

        float u0 = static_cast<float>(charColumn) / static_cast<float>(atlasColumns);
        float v0 = static_cast<float>(charRow) / static_cast<float>(atlasRows);
        float u1 = static_cast<float>(charColumn + 1) / static_cast<float>(atlasColumns);
        float v1 = static_cast<float>(charRow + 1) / static_cast<float>(atlasRows);

        // Definimos las esquinas del cuadrilátero
        // Usamos m_color del miembro de clase
        XMFLOAT3 p1 = { x, y, 0.0f };
        XMFLOAT3 p2 = { x + charWidth, y, 0.0f };
        XMFLOAT3 p3 = { x, y + charHeight, 0.0f };

        /*p1 = DirectXUtils::Divide(p1, div);
        p2 = DirectXUtils::Divide(p2, div);
        p3 = DirectXUtils::Divide(p3, div);*/

        // Inicializa Position y TexCoord con sus propias llaves,
        // y para Color, simplemente asigna m_color directamente.
        *pVertexData++ = { p1, { u0, v0 }, m_color };
        *pVertexData++ = { p2, { u1, v0 }, m_color };
        *pVertexData++ = { p3, { u0, v1 }, m_color };

        *pVertexData++ = { { x, y + charHeight, 0.0f}, {u0, v1}, m_color };
        *pVertexData++ = { { x + charWidth, y, 0.0f }, { u1, v0 }, m_color };
        *pVertexData++ = { { x + charWidth, y + charHeight, 0.0f }, { u1, v1 }, m_color };

        x += charWidth * 0.9f;
        x = roundf(x);
    }

	DebugVertextData(pInitialVertexData, static_cast<int>(m_text.length()));

    context->Unmap(m_vertexBuffer, 0);
}

//void UIText::ApplyMaterial(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Material> m_material, XMMATRIX orthoMatrix) {
//
//    m_material->Apply(context);
//
//    DirectX::XMMATRIX worldIdentity = DirectX::XMMatrixIdentity();
//    DirectX::XMMATRIX viewIdentity = DirectX::XMMatrixIdentity();
//
//    MatrixDefinitionBase::MatrixParams params = {};
//    params.worldMatrix = worldIdentity;
//	params.viewMatrix = viewIdentity;
//	params.projectionMatrix = orthoMatrix;
//
//    m_material->SetConstantBuffers(context, params, 1);
//}

//void UIText::Render() {
//	m_uiManager = ManagerLocator::GetManager<UIManager>();
//    if (!m_uiManager) {
//        OutputDebugStringA("Error: UIText::Render - no UIManager available.\n");
//        return;
//    }
//    XMMATRIX ortho = m_uiManager->GetOrthoMatrix();
//	Render(ortho);
//}

//void UIText::Render(const DirectX::XMMATRIX& orthoMatrix) {
//    // Verificación de punteros esenciales
//    if (!m_material || !m_device) {
//        OutputDebugStringA("Error: UIText::Render - Dependencia nula (material ó device).\n");
//        return;
//    }
//
//    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context = m_device->GetContext();
//
//	// Aplicar el material y las matrices
//	ApplyMaterial(context, m_material, orthoMatrix);
//
//    // Gestión del tamaño del Vertex Buffer
//    UINT requiredVertexCount = CreateVertexBuffer();
//
//	// Establecer el vertex buffer
//	SetVertexBuffer(context);
//    
//    // Enlazar el vertex buffer
//    UINT stride = sizeof(UIVertex);
//    UINT offset = 0;
//    context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
//
//    // Establecer la topología de la primitiva a TRIANGLELIST
//    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//    // Dibujar el texto
//    context->Draw(requiredVertexCount, 0);
//}

void UIText::Shutdown() {
    SafeRelease(m_vertexBuffer);
}