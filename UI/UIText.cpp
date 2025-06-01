#include "UIText.h"
#include <cmath> // Para std::ceil

UIText::UIText(Material* material, const std::string& text, const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT4& color, float fontSize)
    : m_material(material), m_text(text), m_position(position), m_color(color), m_fontSize(fontSize),
    m_vertexBuffer(nullptr), m_fontTextureView(nullptr), m_fontAtlasWidth(0), m_fontAtlasHeight(0) {}

UIText::~UIText() {
    Release();
}

HRESULT UIText::Init()
{
    return E_NOTIMPL;
}

HRESULT UIText::Init(ID3D11Device* device, int fontAtlasWidth, int fontAtlasHeight) {
    HRESULT hr = S_OK;
    
    m_fontAtlasWidth = fontAtlasWidth;
    m_fontAtlasHeight = fontAtlasHeight;

    // Calculate the number of vertices needed (4 per character)
    UINT numVertices = static_cast<UINT>(m_text.length() * 4);

    // Create the vertex buffer
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(Vertex) * numVertices;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.MiscFlags = 0;
    bd.StructureByteStride = 0;

    hr = device->CreateBuffer(&bd, nullptr, &m_vertexBuffer);
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}

void UIText::Render()
{
}

// UIText.cpp

#include "UIText.h"
#include <DirectXMath.h> // Para XMMATRIX
#include <cmath>         // Para round()

// Constructor y otros métodos de UIText (asegúrate de que m_vertexBuffer se inicialice a nullptr)
// UIText::UIText() : m_vertexBuffer(nullptr) { /* ... */ }
// UIText::~UIText() { SafeRelease(m_vertexBuffer); /* ... */ }

void UIText::Render(DeviceManager* device, ShaderManager* shaderManager, WorldMatrixManager* worldMatrixManager, const DirectX::XMMATRIX& orthoMatrix) {
    // Verificación de punteros esenciales
    if (!m_material) { // m_vertexBuffer se gestiona dinámicamente ahora
        return;
    }

    ID3D11DeviceContext* context = device->GetContext();

    // Aplica el material (establece shaders, texturas, sampler, etc.)
    m_material->Apply(context);

    HRESULT hr = S_OK;

    // --- Gestión del tamaño del Vertex Buffer ---
    UINT requiredVertexCount = static_cast<UINT>(m_text.length() * 6); // 6 vértices por carácter para TRIANGLELIST
    UINT currentBufferSize = 0;

    // Si m_vertexBuffer ya existe, obtenemos su tamaño actual
    if (m_vertexBuffer) {
        D3D11_BUFFER_DESC desc;
        m_vertexBuffer->GetDesc(&desc);
        currentBufferSize = desc.ByteWidth / sizeof(Vertex);
    }

    // Si el buffer no existe, o no es lo suficientemente grande, lo recreamos
    if (!m_vertexBuffer || requiredVertexCount > currentBufferSize) {
        if (m_vertexBuffer) {
            m_vertexBuffer->Release(); // Libera el buffer antiguo si existe
            m_vertexBuffer = nullptr;
        }

        // Definimos una capacidad mínima para evitar recreaciones frecuentes para textos muy cortos
        // Opcional, pero buena práctica para eficiencia
        UINT bufferCapacity = max(requiredVertexCount, 6 * 10); // Mínimo para 10 caracteres, o lo que se necesite

        D3D11_BUFFER_DESC vertexBufferDesc = {};
        vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        vertexBufferDesc.ByteWidth = bufferCapacity * sizeof(Vertex); // Crea el buffer con la capacidad adecuada
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        vertexBufferDesc.MiscFlags = 0;
        vertexBufferDesc.StructureByteStride = 0;

        hr = device->GetDevice()->CreateBuffer(&vertexBufferDesc, nullptr, &m_vertexBuffer);
        if (FAILED(hr)) {
            // Manejar error (e.g., log y retornar)
            OutputDebugStringA("Error al crear/redimensionar el vertex buffer para UIText.\n");
            return;
        }
    }
    // --- Fin de la gestión del Vertex Buffer ---

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    Vertex* pVertexData;

    // Mapea el vertex buffer (ahora sabemos que es del tamaño correcto)
    hr = context->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(hr)) {
        OutputDebugStringA("Error al mapear el vertex buffer para UIText.\n");
        return;
    }
    pVertexData = (Vertex*)mappedResource.pData;

    // Dimensiones de la fuente y del atlas
    m_fontSize = 32.0f; // Tamaño deseado de la altura de la fuente en pantalla
    float charWidth = m_fontSize; // Ancho del cuadrilátero en pantalla (asume 32x32 píxeles por glifo en atlas)
    float charHeight = m_fontSize; // Alto del cuadrilátero en pantalla

    int atlasColumns = 16; // Número de columnas en el atlas (DejaVuSansMono.jpg)
    int atlasRows = 16;    // Número de filas en el atlas (DejaVuSansMono.jpg)

    // Posición inicial del texto (redondeada para alineación de píxeles)
    float x = round(m_position.x);
    float y = round(m_position.y);

    // Bucle para poblar los datos de los vértices para cada carácter
    for (char c : m_text) {
        int charIndex = static_cast<int>(c);
        // Calcula la columna y fila del carácter en el atlas
        int charColumn = charIndex % atlasColumns;
        int charRow = charIndex / atlasColumns; // Correcto: dividir por el número de columnas

        // Calcula las coordenadas de textura (UVs) para el glifo
        float u0 = static_cast<float>(charColumn) / static_cast<float>(atlasColumns);
        float v0 = static_cast<float>(charRow) / static_cast<float>(atlasRows);
        float u1 = static_cast<float>(charColumn + 1) / static_cast<float>(atlasColumns);
        float v1 = static_cast<float>(charRow + 1) / static_cast<float>(atlasRows);

        // Definimos las esquinas del cuadrilátero para mayor claridad (ya redondeadas por x,y inicial)
        // Top Left (TL)
        DirectX::XMFLOAT3 posTL = { x, y, 0.0f };
        DirectX::XMFLOAT2 texTL = { u0, v0 };
        // Top Right (TR)
        DirectX::XMFLOAT3 posTR = { x + charWidth, y, 0.0f };
        DirectX::XMFLOAT2 texTR = { u1, v0 };
        // Bottom Left (BL)
        DirectX::XMFLOAT3 posBL = { x, y + charHeight, 0.0f };
        DirectX::XMFLOAT2 texBL = { u0, v1 };
        // Bottom Right (BR)
        DirectX::XMFLOAT3 posBR = { x + charWidth, y + charHeight, 0.0f };
        DirectX::XMFLOAT2 texBR = { u1, v1 };

        // Primer triángulo del cuadrilátero: TL, TR, BL
        pVertexData->Position = posTL;
        pVertexData->TexCoord = texTL;
        pVertexData->Color = m_color;
        pVertexData++;

        pVertexData->Position = posTR;
        pVertexData->TexCoord = texTR;
        pVertexData->Color = m_color;
        pVertexData++;

        pVertexData->Position = posBL;
        pVertexData->TexCoord = texBL;
        pVertexData->Color = m_color;
        pVertexData++;

        // Segundo triángulo del cuadrilátero: BL, TR, BR
        pVertexData->Position = posBL; // Reutiliza BL
        pVertexData->TexCoord = texBL;
        pVertexData->Color = m_color;
        pVertexData++;

        pVertexData->Position = posTR; // Reutiliza TR
        pVertexData->TexCoord = texTR;
        pVertexData->Color = m_color;
        pVertexData++;

        pVertexData->Position = posBR;
        pVertexData->TexCoord = texBR;
        pVertexData->Color = m_color;
        pVertexData++;

        x += charWidth * 0.9f; // Avance para el siguiente carácter en la línea
		x = round(x); // Redondea para evitar problemas de precisión
    }

    context->Unmap(m_vertexBuffer, 0);

    // --- Gestión del Constant Buffer para la matriz ortográfica ---
    // Este código está bien si el matrixBuffer se crea y libera en cada frame.
    // Para mayor eficiencia, podrías considerar crear este buffer una vez y
    // solo mapearlo y actualizarlo en cada frame.

    struct MatrixBuffer
    {
        DirectX::XMMATRIX projectionMatrix;
    };

    D3D11_BUFFER_DESC matrixBufferDesc = {};
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    ID3D11Buffer* matrixBuffer = nullptr;
    hr = device->GetDevice()->CreateBuffer(&matrixBufferDesc, nullptr, &matrixBuffer);
    if (FAILED(hr)) {
        OutputDebugStringA("Error al crear el constant buffer para la matriz.\n");
        return;
    }

    D3D11_MAPPED_SUBRESOURCE mappedResourceCB;
    hr = context->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResourceCB);
    if (FAILED(hr)) {
        matrixBuffer->Release();
        OutputDebugStringA("Error al mapear el constant buffer para la matriz.\n");
        return;
    }

    MatrixBuffer* dataPtr = (MatrixBuffer*)mappedResourceCB.pData;
    dataPtr->projectionMatrix = DirectX::XMMatrixTranspose(orthoMatrix); // Transpose para HLSL
    context->Unmap(matrixBuffer, 0);

    // Enlazar el constant buffer al vertex shader en el slot 1
    worldMatrixManager->SetVertexShaderConstantBuffer(context, 1, matrixBuffer); // Asumiendo slot 1

    // Enlazar el vertex buffer
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Establecer la topología de la primitiva a TRIANGLELIST
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // <-- CAMBIO CONFIRMADO

    // Dibujar el texto: Número total de vértices (6 por carácter)
    context->Draw(requiredVertexCount, 0); // Usa requiredVertexCount directamente

    // Liberar el constant buffer de la matriz
    matrixBuffer->Release();
}

//void UIText::Render(DeviceManager* device, ShaderManager* shaderManager, WorldMatrixManager* worldMatrixManager, const DirectX::XMMATRIX& orthoMatrix) {
//    if (!m_vertexBuffer || !m_material) {
//        return;
//    }
//
//    worldMatrixManager->SetVertexShaderConstantBuffer(device->GetContext(), 1, &orthoMatrix); // Suponiendo que el buffer está en slot 1
//
//    // Apply the material (sets shaders, textures, etc.)
//    m_material->Apply(device->GetContext());
//
//    HRESULT hr = S_OK;
//    D3D11_MAPPED_SUBRESOURCE mappedResource;
//    Vertex* pVertexData;
//	ID3D11DeviceContext* context = device->GetContext();
//
//    // Map the vertex buffer
//    hr = context->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
//    if (FAILED(hr)) {
//        return;
//    }
//    pVertexData = (Vertex*)mappedResource.pData;
//
//    // Calculate character dimensions
//    float charWidth = m_fontSize / 2.0f; // Approximate character width (adjust as needed)
//    float charHeight = m_fontSize;
//
//    // Assuming 16x16 font atlas
//    int atlasColumns = 16;
//    int atlasRows = 16;
//
//    // Populate vertex data
//    float x = m_position.x;
//    float y = m_position.y;
//
//    for (char c : m_text) {
//        int charIndex = static_cast<int>(c);
//        int charColumn = charIndex % atlasColumns;
//        int charRow = charIndex / atlasRows;
//
//        float u0 = static_cast<float>(charColumn) / atlasColumns;
//        float v0 = static_cast<float>(charRow) / atlasRows;
//        float u1 = static_cast<float>(charColumn + 1) / atlasColumns;
//        float v1 = static_cast<float>(charRow + 1) / atlasRows;
//
//        // Vertex 1 (Top Left)
//        pVertexData->Position = { x, y, 0.0f };
//        pVertexData->TexCoord = { u0, v0 };
//        pVertexData->Color = m_color;
//        pVertexData++;
//
//        // Vertex 2 (Top Right)
//        pVertexData->Position = { x + charWidth, y, 0.0f };
//        pVertexData->TexCoord = { u1, v0 };
//        pVertexData->Color = m_color;
//        pVertexData++;
//
//        // Vertex 3 (Bottom Left)
//        pVertexData->Position = { x, y + charHeight, 0.0f };
//        pVertexData->TexCoord = { u0, v1 };
//        pVertexData->Color = m_color;
//        pVertexData++;
//
//        // Vertex 4 (Bottom Right)
//        pVertexData->Position = { x + charWidth, y + charHeight, 0.0f };
//        pVertexData->TexCoord = { u1, v1 };
//        pVertexData++;
//
//        x += charWidth;
//    }
//
//    context->Unmap(m_vertexBuffer, 0);
//
//    // Set vertex buffer
//    UINT stride = sizeof(Vertex);
//    UINT offset = 0;
//    context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
//
//    // Set primitive topology
//    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
//
//    // Draw the text
//    context->Draw(static_cast<UINT>(m_text.length() * 4), 0);
//}

void UIText::Release() {
    if (m_vertexBuffer) {
        m_vertexBuffer->Release();
        m_vertexBuffer = nullptr;
    }
    TextureLoader::ReleaseTextureView(m_fontTextureView);
}