#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <format>
#include <type_traits>
#include <iostream>
#include <variant>
#include <IDefine/IVertex.h>

inline constexpr std::string_view VERTEX_TEXT = "TextVertex";
inline constexpr std::string_view VERTEX_SIMPLE = "SimpleVertex";
inline constexpr std::string_view VERTEX_SKYBOX = "SkyboxVertex";
inline constexpr std::string_view VERTEX_TEXTURE_BASIC = "TextureBasicVertex";
inline constexpr std::string_view VERTEX_TEXTURE_MAP = "TextureBasicMap";

namespace VertexDefinition {

    /*struct VertexBase: public IVertex {
        VertexBase() {};
        VertexBase(DirectX::XMFLOAT3 p, DirectX::XMFLOAT2 t, DirectX::XMFLOAT3 n, DirectX::XMFLOAT4 c) {};
        UINT GetByteWidth(size_t numVertex) override { return 0; };
        const void* GetRawData() const noexcept override { return nullptr; };
        D3D11_INPUT_ELEMENT_DESC* getInputLayout(unsigned int& numElements) override { return nullptr; };
        std::string ToOBJLine() const override { return ""; };
        void SetData(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT2 texCoord, DirectX::XMFLOAT3 normals, DirectX::XMFLOAT4 color) override {};
    };*/

    struct Vertex : public IVertex {
        float position[3];
        Vertex() : position{ 0.0f, 0.0f, 0.0f } {}
        Vertex(float x, float y, float z) : position{ x, y, z } {}
    };

    /*
    * SIMPLE VERTEX
    */
    struct SimpleVertex : public IVertex {
        float position[3];

        SimpleVertex() : position{} {}
        SimpleVertex(DirectX::XMFLOAT3 p, DirectX::XMFLOAT2 t, DirectX::XMFLOAT3 n, DirectX::XMFLOAT4 c) : position(p.x, p.y, p.z) {}
        SimpleVertex(float x, float y, float z) : position{ x, y, z } {}

        UINT Size() override {
            return sizeof(position); // Retorna el tamaño en bytes del vértice
        }

        UINT GetByteWidth(size_t numVertex) override {
            return sizeof(position) * static_cast<UINT>(numVertex);
        }

        const void* GetRawData() const noexcept override {
            return static_cast<const void*>(position); // Devuelve un puntero a sí mismo
        }

        const DirectX::XMFLOAT3 GetPosition() const override {
            return DirectX::XMFLOAT3{ position };
        }
        const DirectX::XMFLOAT3 GetNormal() const override {
            return DirectX::XMFLOAT3();
        }
        void SetNormal(const DirectX::XMFLOAT3& normal) override {}

        //const void* GetRawData(const std::vector<std::shared_ptr<VertexDefinition::VertexVariant>> vertex) const noexcept override {
        //    return nullptr;
        //    //return static_cast<const void*>(this); // Devuelve un puntero a sí mismo
        //}

        // Implementación de ToOBJLine
        std::string ToOBJLine() const override {
            return std::format("v {} {} {}", position[0], position[1], position[2]);
        }

        void SetData(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT2 texCoord, DirectX::XMFLOAT3 normals, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 tangent, DirectX::XMFLOAT4 debugColor) override {
            position[0] = pos.x;
            position[1] = pos.y;
            position[2] = pos.z;
        }

        // Implementación de getInputLayout
        D3D11_INPUT_ELEMENT_DESC* GetInputLayout(unsigned int& numElements) override {
            numElements = 1; // Número de elementos en el layout
            D3D11_INPUT_ELEMENT_DESC* layoutArray = new D3D11_INPUT_ELEMENT_DESC[numElements];
            layoutArray[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
            return layoutArray; // Devuelve el número de elementos en el layout
        }


        /*std::vector<SimpleVertex> convert(std::vector<IVertex> vertex) {
            std::vector<SimpleVertex> simpleVertices;
            for (const auto& v : vertex) {
                if (auto sv = dynamic_cast<const SimpleVertex*>(&v)) {
                    simpleVertices.push_back(*sv);
                }
            }
            return simpleVertices;
        }*/
    };

    struct SimpleNormalVertex : public IVertex {
        // El destructor no necesita ser sobrescrito si solo es default
        // ~SimpleNormalVertex() override {}; 
        float position[3];
        float normal[3];

        SimpleNormalVertex() : position{}, normal{} {}
        SimpleNormalVertex(DirectX::XMFLOAT3 p, DirectX::XMFLOAT2 t, DirectX::XMFLOAT3 n, DirectX::XMFLOAT4 c) : position(p.x, p.y, p.z), normal(n.x, n.y, n.z) {}
        SimpleNormalVertex(float x, float y, float z, float nx, float ny, float nz)
            : position{ x, y, z }, normal{ nx, ny, nz } {
        }

        UINT Size() override {
            return sizeof(position) + sizeof(normal); // Retorna el tamaño en bytes del vértice
        }

        UINT GetByteWidth(size_t numVertex) override {
            return sizeof(position) * static_cast<UINT>(numVertex);
        };
        const void* GetRawData() const noexcept override {
            return static_cast<const void*>(position); // Devuelve un puntero a sí mismo
        }
        const DirectX::XMFLOAT3 GetPosition() const override {
            return DirectX::XMFLOAT3{ position };
        }
        const DirectX::XMFLOAT3 GetNormal() const override {
            return DirectX::XMFLOAT3{ normal };
        }
        void SetNormal(const DirectX::XMFLOAT3& normal) override {
            this->normal[0] = normal.x;
            this->normal[1] = normal.y;
            this->normal[2] = normal.z;
        }

        std::string ToOBJLine() const override {
            return std::format("v {} {} {}\nvn {} {} {}",
                position[0], position[1], position[2],
                normal[0], normal[1], normal[2]);
        }

        void SetData(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT2 texCoord, DirectX::XMFLOAT3 normals, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 tangent, DirectX::XMFLOAT4 debugColor) override {
            position[0] = pos.x;
            position[1] = pos.y;
            position[2] = pos.z;
            normal[0] = normals.x;
            normal[1] = normals.y;
            normal[2] = normals.z;
        }

        D3D11_INPUT_ELEMENT_DESC* GetInputLayout(unsigned int& numElements) override {
            numElements = 2; // Número de elementos en el layout
            D3D11_INPUT_ELEMENT_DESC* layoutArray = new D3D11_INPUT_ELEMENT_DESC[numElements];
            layoutArray[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
            layoutArray[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }; // Corrección del índice
            return layoutArray; // Devuelve el número de elementos
        }
    };

    // SkyboxVertex debe heredar de BaseVertex para ser parte de la jerarquía
    struct SkyboxVertex : public IVertex {
        float position[4];

        SkyboxVertex() : position{ 0.0f, 0.0f, 0.0f, 1.0f } {}
        SkyboxVertex(DirectX::XMFLOAT3 p, DirectX::XMFLOAT2 t, DirectX::XMFLOAT3 n, DirectX::XMFLOAT4 c) : position(p.x, p.y, p.z, 1.0f) {}
        SkyboxVertex(float x, float y, float z, float nx, float ny, float nz, float u, float v)
            : position{ x, y, z, 1.0f } {
        }

        UINT Size() override {
            return sizeof(position);
        }

        UINT GetByteWidth(size_t numVertex) override {
            return sizeof(position) * static_cast<UINT>(numVertex);
        };
        const void* GetRawData() const noexcept override {
            return static_cast<const void*>(position); // Devuelve un puntero a sí mismo
        }
        const DirectX::XMFLOAT3 GetPosition() const override {
            return DirectX::XMFLOAT3{ position };
        }
        const DirectX::XMFLOAT3 GetNormal() const override {
            return DirectX::XMFLOAT3();
        }
        void SetNormal(const DirectX::XMFLOAT3& normal) override {}

        std::string ToOBJLine() const override {
            return std::format("v {} {} {} {}",
                position[0], position[1], position[2], position[3]);
        }

        void SetData(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT2 texCoord, DirectX::XMFLOAT3 normals, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 tangent, DirectX::XMFLOAT4 debugColor) override {
            position[0] = pos.x;
            position[1] = pos.y;
            position[2] = pos.z;
            position[3] = 1.0f;
        }

        D3D11_INPUT_ELEMENT_DESC* GetInputLayout(unsigned int& numElements) override {
            numElements = 1; // Número de elementos en el layout
            D3D11_INPUT_ELEMENT_DESC* layoutArray = new D3D11_INPUT_ELEMENT_DESC[numElements];
            layoutArray[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
            return layoutArray; // Devuelve el número de elementos
        }
    };

    struct TextVertex : public IVertex {
        float position[3];
        float texCoord[2];
        float color[4];

        TextVertex() : position{}, texCoord{}, color{} {}
        TextVertex(DirectX::XMFLOAT3 p, DirectX::XMFLOAT2 t, DirectX::XMFLOAT4 c) : position(p.x, p.y, p.z), texCoord(t.x, t.y), color(c.x, c.y, c.z, c.w) {}
        TextVertex(float x, float y, float z, float u, float v, float r, float g, float b, float a)
            : position{ x,y,z }, texCoord{ u,v }, color{ r,g,b,a } {
        }

        std::string ToOBJLine() const override {
            return std::format("v {} {} {}\nvt {} {}\nvc {} {} {} {}",
                position[0], position[1], position[2],
                texCoord[0], texCoord[1],
                color[0], color[1], color[2], color[3]);
        }

        UINT Size() override {
            return sizeof(position) + sizeof(texCoord) + sizeof(color); // Retorna el tamaño en bytes del vértice
        }

        UINT GetByteWidth(size_t numVertex) override {
            return sizeof(position) * static_cast<UINT>(numVertex);
        };
        const void* GetRawData() const noexcept override {
            return static_cast<const void*>(position); // Devuelve un puntero a sí mismo
        }
        const DirectX::XMFLOAT3 GetPosition() const override {
            return DirectX::XMFLOAT3{ position };
        }
        const DirectX::XMFLOAT3 GetNormal() const override {
            return DirectX::XMFLOAT3();
        }
        void SetNormal(const DirectX::XMFLOAT3& normal) override {}

        void SetData(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT2 texCoord, DirectX::XMFLOAT3 normals, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 tangent, DirectX::XMFLOAT4 debugColor) override {
            position[0] = pos.x;
            position[1] = pos.y;
            position[2] = pos.z;
            this->texCoord[0] = texCoord.x;
            this->texCoord[1] = texCoord.y;
            this->color[0] = color.x;
            this->color[1] = color.y;
            this->color[2] = color.z;
            this->color[3] = color.w;
        }

        D3D11_INPUT_ELEMENT_DESC* GetInputLayout(unsigned int& numElements) override {
            numElements = 3; // Número de elementos en el layout
            D3D11_INPUT_ELEMENT_DESC* layoutArray = new D3D11_INPUT_ELEMENT_DESC[numElements];
            layoutArray[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
            layoutArray[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 };
            layoutArray[2] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 };
            return layoutArray; // Devuelve el número de elementos
        }
    };

    struct TextureBasicVertex : public IVertex {
        float position[3];
        float texCoord[2];

        TextureBasicVertex() : position{}, texCoord{} {}
        TextureBasicVertex(DirectX::XMFLOAT3 p, DirectX::XMFLOAT2 t, DirectX::XMFLOAT3 n, DirectX::XMFLOAT4 c) : position(p.x, p.y, p.z), texCoord(t.x, t.y) {}
        TextureBasicVertex(float x, float y, float z, float u, float v)
            : position{ x,y,z }, texCoord{ u,v } {
        }

        UINT Size() override {
            return sizeof(position) + sizeof(texCoord); // Retorna el tamaño en bytes del vértice
        }

        UINT GetByteWidth(size_t numVertex) override {
            return (sizeof(position) + sizeof(texCoord)) * static_cast<UINT>(numVertex);
        };
        const void* GetRawData() const noexcept override {
            return static_cast<const void*>(position); // Devuelve un puntero a sí mismo
        }
        const DirectX::XMFLOAT3 GetPosition() const override {
            return DirectX::XMFLOAT3{ position };
        }
        const DirectX::XMFLOAT3 GetNormal() const override {
            return DirectX::XMFLOAT3();
        }
        void SetNormal(const DirectX::XMFLOAT3& normal) override {}

        std::string ToOBJLine() const override {
            return std::format("v {} {} {}\nvt {} {}",
                position[0], position[1], position[2],
                texCoord[0], texCoord[1]);
        }

        void SetData(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT2 texCoord, DirectX::XMFLOAT3 normals, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 tangent, DirectX::XMFLOAT4 debugColor) override {
            position[0] = pos.x;
            position[1] = pos.y;
            position[2] = pos.z;
            this->texCoord[0] = texCoord.x;
            this->texCoord[1] = texCoord.y;
        }

        D3D11_INPUT_ELEMENT_DESC* GetInputLayout(unsigned int& numElements) override {
            numElements = 2; // Número de elementos en el layout
            D3D11_INPUT_ELEMENT_DESC* layoutArray = new D3D11_INPUT_ELEMENT_DESC[numElements];
            layoutArray[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
            layoutArray[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 };
            return layoutArray; // Devuelve el número de elementos
        }
    };

    struct TextureMapVertex : public IVertex {
        float position[3];
        float texCoord[2];
        float normal[3];
        float tangent[3];
        float debugColor[4];

        TextureMapVertex(const IVertex& iOther) {
            const TextureMapVertex* other = dynamic_cast<const TextureMapVertex*>(&iOther);
            if (!other) {
                throw std::runtime_error("IVertex no es un TextureMapVertex");
            }

            position[0] = other->position[0];
            position[1] = other->position[1];
            position[2] = other->position[2];
            texCoord[0] = other->texCoord[0];
            texCoord[1] = other->texCoord[1];
            normal[0] = other->normal[0];
            normal[1] = other->normal[1];
            normal[2] = other->normal[2];
            tangent[0] = other->tangent[0];
            tangent[1] = other->tangent[1];
            tangent[2] = other->tangent[2];
            debugColor[0] = other->debugColor[0];
            debugColor[1] = other->debugColor[1];
            debugColor[2] = other->debugColor[2];
            debugColor[3] = other->debugColor[3];
        }

        TextureMapVertex() : position{}, texCoord{}, normal{}, tangent{}, debugColor{} {}
        TextureMapVertex(DirectX::XMFLOAT3 p, DirectX::XMFLOAT2 t, DirectX::XMFLOAT3 n, DirectX::XMFLOAT3 ta, DirectX::XMFLOAT4 dbg) : position(p.x, p.y, p.z), texCoord(t.x, t.y), normal(n.x, n.y, n.z), tangent(ta.x, ta.y, ta.z), debugColor(dbg.x, dbg.y, dbg.z, dbg.w) {}
        TextureMapVertex(float x, float y, float z, float u, float v, float nx, float ny, float nz, float tax, float tay, float taz, float dbx, float dby, float dbz, float dbw)
            : position{ x,y,z }, texCoord{ u,v }, normal{ nx, ny, nz }, tangent{ tax, tay, taz }, debugColor{ dbx,dby,dbz,dbw } {
        }

        UINT Size() override {
            return sizeof(position) + sizeof(texCoord) + sizeof(normal) + sizeof(tangent) + sizeof(debugColor); // Retorna el tamaño en bytes del vértice
        }

        UINT GetByteWidth(size_t numVertex) override {
            return (Size()) * static_cast<UINT>(numVertex);
        };
        const void* GetRawData() const noexcept override {
            return static_cast<const void*>(position); // Devuelve un puntero a sí mismo
        }
        const DirectX::XMFLOAT3 GetPosition() const override {
            return DirectX::XMFLOAT3{ position };
        }
        const DirectX::XMFLOAT3 GetNormal() const override {
            return DirectX::XMFLOAT3{ normal };
        }
        const DirectX::XMFLOAT4 GetDebugColor() override {
            return DirectX::XMFLOAT4{ debugColor };
        }
        const void SetDebugColor(const DirectX::XMFLOAT4& color) override {
            this->debugColor[0] = color.x;
            this->debugColor[1] = color.y;
            this->debugColor[2] = color.z;
            this->debugColor[3] = color.w;
        }
        void SetNormal(const DirectX::XMFLOAT3& normal) override {
            this->normal[0] = normal.x;
            this->normal[1] = normal.y;
            this->normal[2] = normal.z;
        }

        std::string ToOBJLine() const override {
            return std::format("v {} {} {}\nvt {} {}\nvt {} {} {}\nvt {} {} {}\nvt {} {} {} {}",
                position[0], position[1], position[2],
                texCoord[0], texCoord[1],
                normal[0], normal[1], normal[2],
                tangent[0], tangent[1], tangent[2],
                debugColor[0], debugColor[1], debugColor[2], debugColor[3]
            );
        }

        void SetData(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT2 texCoord, DirectX::XMFLOAT3 normals, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 tangent, DirectX::XMFLOAT4 debugColor) override {
            position[0] = pos.x;
            position[1] = pos.y;
            position[2] = pos.z;
            this->texCoord[0] = texCoord.x;
            this->texCoord[1] = texCoord.y;
            normal[0] = normals.x;
            normal[1] = normals.y;
            normal[2] = normals.z;
            this->tangent[0] = tangent.x;
            this->tangent[1] = tangent.y;
            this->tangent[2] = tangent.z;
            this->debugColor[0] = debugColor.x;
            this->debugColor[1] = debugColor.y;
            this->debugColor[2] = debugColor.z;
            this->debugColor[3] = debugColor.w;
        }

        D3D11_INPUT_ELEMENT_DESC* GetInputLayout(unsigned int& numElements) override {
            numElements = 5;
            D3D11_INPUT_ELEMENT_DESC* layoutArray = new D3D11_INPUT_ELEMENT_DESC[numElements];

            layoutArray[0] = { "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0,  D3D11_INPUT_PER_VERTEX_DATA, 0 };  // 12 bytes
            layoutArray[1] = { "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,       0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 };  // 8 bytes
            layoutArray[2] = { "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 20,  D3D11_INPUT_PER_VERTEX_DATA, 0 };  // 12 bytes
            layoutArray[3] = { "TANGENT",    0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 32,  D3D11_INPUT_PER_VERTEX_DATA, 0 };  // 12 bytes
            layoutArray[4] = { "COLOR",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 44,  D3D11_INPUT_PER_VERTEX_DATA, 0 };  // 16 bytes

            return layoutArray;
        }
    };

    using VertexVariant = std::variant<SimpleVertex, SimpleNormalVertex, SkyboxVertex, TextVertex, TextureBasicVertex, TextureMapVertex>;

    template<typename T, typename... Types>
    inline constexpr bool is_any_of_v = (std::is_same_v<T, Types> || ...);

    template <typename T>
    inline constexpr bool IsIVertext() {
        return is_any_of_v<T, IVertex>;
    }

    class Factory {
    public:

        template <typename T>
        static std::shared_ptr<T> CreateVertex(std::string_view type) {
            if (type == VERTEX_TEXT) { // Ursa tus constantes de cadena reales
                return std::make_shared<TextVertex>(TextVertex{}); // Construye el variant con un TextVertex
            }
            else if (type == VERTEX_SIMPLE) {
                return std::make_shared<SimpleVertex>(SimpleVertex{});
            }
            else if (type == VERTEX_SKYBOX) {
                return std::make_shared<SkyboxVertex>(SkyboxVertex{});
            }
            else if (type == VERTEX_TEXTURE_BASIC) {
                return std::make_shared<TextureBasicVertex>(TextureBasicVertex{});
            }
            else if (type == VERTEX_TEXTURE_MAP) {
                return std::make_shared<TextureMapVertex>(TextureMapVertex{});
            }
            throw std::invalid_argument("Tipo de vértice desconocido: " + std::string(type));
        }
        template <typename T>
        static std::vector<std::shared_ptr<T>> CreateVertexVector(std::string_view type) {
            if (type == VERTEX_TEXT) { // Usa tus constantes de cadena reales
                return std::vector<std::shared_ptr<T>>{ std::make_shared<TextVertex>(TextVertex{}) };
            }
            else if (type == VERTEX_SIMPLE) {
                return std::vector<std::shared_ptr<T>>{ std::make_shared<SimpleVertex>(SimpleVertex{}) };
            }
            else if (type == VERTEX_SKYBOX) {
                return std::vector<std::shared_ptr<T>>{ std::make_shared<SkyboxVertex>(SkyboxVertex{}) };
            }
            else if (type == VERTEX_TEXTURE_BASIC) {
                return std::vector<std::shared_ptr<T>>{ std::make_shared<TextureBasicVertex>(TextureBasicVertex{}) };
            }
            else if (type == VERTEX_TEXTURE_MAP) {
                return std::vector<std::shared_ptr<T>>{ std::make_shared<TextureMapVertex>(TextureMapVertex{}) };
            }
            throw std::invalid_argument("Tipo de vértice desconocido: " + std::string(type));
        }
    };
};
