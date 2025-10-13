#include "RenderCommand.h"
#include <algorithm>

//--------------------------------------------------------------------------------------
// CommandBuffer Implementation
//--------------------------------------------------------------------------------------

void CommandBuffer::ExecuteAll(ID3D11DeviceContext* context) {
    if (m_needsSorting) {
        SortForOptimalExecution();
        m_needsSorting = false;
    }
    
    for (auto& command : m_commands) {
        command->Execute(context);
    }
}

void CommandBuffer::Clear() {
    m_commands.clear();
    m_needsSorting = false;
}

RenderCommandPacket CommandBuffer::CreatePacket(int frameId) {
    RenderCommandPacket packet;
    packet.frameId = frameId;
    packet.submitTime = std::chrono::high_resolution_clock::now();
    packet.commands = std::move(m_commands);
    
    Clear();
    return packet;
}

void CommandBuffer::SortForOptimalExecution() {
    // Sort para minimizar state changes en GPU
    std::sort(m_commands.begin(), m_commands.end(),
              [](const auto& a, const auto& b) {
                  return a->GetSortKey() < b->GetSortKey();
              });
}

//--------------------------------------------------------------------------------------
// SetShaderCommand Implementation
//--------------------------------------------------------------------------------------

void SetShaderCommand::Execute(ID3D11DeviceContext* context) {
    if (m_vertexShader) {
        context->VSSetShader(m_vertexShader, nullptr, 0);
    }
    if (m_pixelShader) {
        context->PSSetShader(m_pixelShader, nullptr, 0);
    }
}

uint32_t SetShaderCommand::GetSortKey() const {
    // Shader changes son costosos, darle alta prioridad en sorting
    uint32_t vsKey = reinterpret_cast<uintptr_t>(m_vertexShader) >> 8;
    uint32_t psKey = reinterpret_cast<uintptr_t>(m_pixelShader) >> 16;
    return vsKey | psKey;
}

//--------------------------------------------------------------------------------------
// DrawIndexedCommand Implementation
//--------------------------------------------------------------------------------------

void DrawIndexedCommand::Execute(ID3D11DeviceContext* context) {
    context->DrawIndexed(m_indexCount, m_startIndexLocation, m_baseVertexLocation);
}

uint32_t DrawIndexedCommand::GetSortKey() const {
    // Draw commands van al final después de todos los state changes
    return 0xFFFFFF00 + m_indexCount;  // Higher number = later execution
}
