#pragma once

#include <memory>
#include <vector>
#include <chrono>
#include <d3d11.h>

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
