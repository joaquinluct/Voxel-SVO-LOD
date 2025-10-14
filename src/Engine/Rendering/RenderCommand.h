#pragma once

#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <d3d11.h>
#include <algorithm>

// ===============================================================================
// COMMAND PATTERN PARA RENDER PIPELINE AAA
// ===============================================================================

//--------------------------------------------------------------------------------------
// Interfaz base para comandos de renderizado
//--------------------------------------------------------------------------------------
class IRenderCommand {
public:
    virtual ~IRenderCommand() = default;
    virtual void Execute(ID3D11DeviceContext* context) = 0;
    virtual uint32_t GetSortKey() const = 0;  // Para batching/sorting
    virtual size_t GetMemoryFootprint() const = 0;
};

//--------------------------------------------------------------------------------------
// DrawTextCommand: placeholder for UI text drawing
//--------------------------------------------------------------------------------------
class DrawTextCommand : public IRenderCommand {
private:
    std::string m_text;
    float m_x, m_y;
    float m_fontSize;
    float m_color[4];

public:
    DrawTextCommand(const std::string& text, float x, float y, float fontSize, float r, float g, float b, float a)
        : m_text(text), m_x(x), m_y(y), m_fontSize(fontSize) {
        m_color[0] = r; m_color[1] = g; m_color[2] = b; m_color[3] = a;
    }

    void Execute(ID3D11DeviceContext* /*context*/) override;

    // Accessors for renderer
    const std::string& GetText() const { return m_text; }
    float GetX() const { return m_x; }
    float GetY() const { return m_y; }
    float GetFontSize() const { return m_fontSize; }
    void GetColor(float out[4]) const { out[0]=m_color[0]; out[1]=m_color[1]; out[2]=m_color[2]; out[3]=m_color[3]; }

    uint32_t GetSortKey() const override {
        // Text should be rendered after most geometry
        return 0xFFFF0001;
    }

    size_t GetMemoryFootprint() const override { return sizeof(DrawTextCommand) + m_text.size(); }
};

//--------------------------------------------------------------------------------------
// DrawQuadCommand: simple UI quad draw command (placeholder)
//--------------------------------------------------------------------------------------
class DrawQuadCommand : public IRenderCommand {
private:
    float m_x, m_y, m_w, m_h;
public:
    DrawQuadCommand(float x, float y, float w, float h)
        : m_x(x), m_y(y), m_w(w), m_h(h) {}

    void Execute(ID3D11DeviceContext* /*context*/) override;

    // Accessors for renderer
    float GetX() const { return m_x; }
    float GetY() const { return m_y; }
    float GetW() const { return m_w; }
    float GetH() const { return m_h; }

    uint32_t GetSortKey() const override {
        // UI quads should render late in the frame; use moderate sort key.
        return 0xFFFF0000;
    }

    size_t GetMemoryFootprint() const override { return sizeof(DrawQuadCommand); }
};

//--------------------------------------------------------------------------------------
// Tipos de comandos específicos
//--------------------------------------------------------------------------------------
enum class RenderCommandType {
    SetShader,
    SetVertexBuffer,
    SetIndexBuffer,
    SetConstantBuffer,
    DrawIndexed,
    Draw,
    SetTexture,
    SetSampler,
    Present
};

//--------------------------------------------------------------------------------------
// Paquete de comandos de renderizado para un frame
//--------------------------------------------------------------------------------------
struct RenderCommandPacket {
    int frameId;
    std::vector<std::unique_ptr<IRenderCommand>> commands;
    std::chrono::high_resolution_clock::time_point submitTime;
    
    RenderCommandPacket() : frameId(0) {}
    
    // Move constructor y assignment
    RenderCommandPacket(RenderCommandPacket&& other) noexcept
        : frameId(other.frameId)
        , commands(std::move(other.commands))
        , submitTime(other.submitTime) {}
    
    RenderCommandPacket& operator=(RenderCommandPacket&& other) noexcept {
        if (this != &other) {
            frameId = other.frameId;
            commands = std::move(other.commands);
            submitTime = other.submitTime;
        }
        return *this;
    }
    
    // Disable copy
    RenderCommandPacket(const RenderCommandPacket&) = delete;
    RenderCommandPacket& operator=(const RenderCommandPacket&) = delete;
};

//--------------------------------------------------------------------------------------
// Command Buffer con auto-sorting para optimización
//--------------------------------------------------------------------------------------
class CommandBuffer {
private:
    std::vector<std::unique_ptr<IRenderCommand>> m_commands;
    bool m_needsSorting = false;
    
public:
    template<typename TCommand, typename... Args>
    void AddCommand(Args&&... args) {
        m_commands.emplace_back(std::make_unique<TCommand>(std::forward<Args>(args)...));
        m_needsSorting = true;
    }
    
    void ExecuteAll(ID3D11DeviceContext* context);
    void Clear();
    size_t GetCommandCount() const { return m_commands.size(); }
    
    // Mover comandos a un packet
    RenderCommandPacket CreatePacket(int frameId);
    
private:
    void SortForOptimalExecution();
};

//--------------------------------------------------------------------------------------
// Comandos específicos de implementación
//--------------------------------------------------------------------------------------
class SetShaderCommand : public IRenderCommand {
private:
    ID3D11VertexShader* m_vertexShader;
    ID3D11PixelShader* m_pixelShader;
    
public:
    SetShaderCommand(ID3D11VertexShader* vs, ID3D11PixelShader* ps)
        : m_vertexShader(vs), m_pixelShader(ps) {}
    
    void Execute(ID3D11DeviceContext* context) override;
    uint32_t GetSortKey() const override;
    size_t GetMemoryFootprint() const override { return sizeof(SetShaderCommand); }
};

class DrawIndexedCommand : public IRenderCommand {
private:
    uint32_t m_indexCount;
    uint32_t m_startIndexLocation;
    int32_t m_baseVertexLocation;
    
public:
    DrawIndexedCommand(uint32_t indexCount, uint32_t startIndex = 0, int32_t baseVertex = 0)
        : m_indexCount(indexCount), m_startIndexLocation(startIndex), m_baseVertexLocation(baseVertex) {}
    
    void Execute(ID3D11DeviceContext* context) override;
    uint32_t GetSortKey() const override;
    size_t GetMemoryFootprint() const override { return sizeof(DrawIndexedCommand); }
};

// Implementations are provided in RenderCommand.cpp to avoid multiple
// definition issues when this header is included in multiple translation units.
