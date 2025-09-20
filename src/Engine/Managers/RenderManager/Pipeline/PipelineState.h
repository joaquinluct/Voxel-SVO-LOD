#pragma once

#include <string>
#include <map>
#include <unordered_map>
#include <Helpers/PipelineHelper.h>
#include <Defines/EngineDefinition.h>
#include <Config/PassConfigBase.h>

class PipelineSlot
{
public:
    PipelineSlot() : id(-1), name(""), used(false) {}
    PipelineSlot(int id, const std::string& name, bool used) : id(id), name(name), used(used) {}
    ~PipelineSlot() = default;
    int GetId() const { return id; }
    std::string GetName() const { return name; }
    bool IsUsed() const { return used; }
    void SetId(int id) { this->id = id; }
    void SetName(const std::string& name) { this->name = name; }
    void SetUsed(bool used) { this->used = used; }
private:
    int id;
    std::string name;
    bool used;
};

struct PipelinePassStateData
{
    int id;
    bool enabled;
    std::string viewport;
    std::string shader;
    std::string material;
    std::string rasterizedState;
    std::string viewPortState;
    std::string stencilState;
    std::string stencilDef;
    std::string shaderViewState;
    bool shadowTextureView;
    std::string blendState;
    int primitiveTopology;
    int resolution;
};

// CONTROL DE ESTADOS DEL PIPELINE
// A nivel de:
//     1) Inicialización
//     2) Pases de render
//     3) Objetos a renderizar
//     4) Finalización

using namespace PipelineHelper;

class PipelineState
{
public:
    PipelineState() {
        Init();
    };

    PipelineState(const PipelineState&) {};

    virtual ~PipelineState() = default;

    // PASS GETTERS
    int GetId() const { return passData.id; }
    bool IsEnabled() const { return passData.enabled; }
    std::string GetViewport() const { return passData.viewport; }
    std::string GetShader() const { return passData.shader; }
    std::string GetMaterial() const { return passData.material; }
    std::string GetRasterizedState() const { return passData.rasterizedState; }
    std::string GetViewPortState() const { return passData.viewPortState; }
    std::string GetStencilState() const { return passData.stencilState; }
    std::string GetStencilDef() const { return passData.stencilDef; }
    std::string GetShaderViewState() const { return passData.shaderViewState; }
    bool GetShadowTextureView() const { return passData.shadowTextureView; }
    std::string GetBlendState() const { return passData.blendState; }
    int GetPrimitiveTopology() const { return passData.primitiveTopology; }
    int GetResolution() const { return passData.resolution; }
    std::string GetCurrentSamplerSlot() const { return currentSamplerSlot; }
    std::unordered_map<unsigned int, PipelineSlot> GetAllConstantBufferSlots() const { return contantsBuffersSlots; }

    // PASS SETTERS
    void SetId(int id) { prevPassData.id = passData.id; passData.id = id; }
    void SetMainViewport(const std::string& viewport) { mainViewport = GetConfigStringValue(viewport); }
    void SetMainStencilView(const std::string& stencilView) { mainStencilView = GetConfigStringValue(stencilView); }
    void SetMainStencilState(const std::string& stencilState) { mainStencilState = GetConfigStringValue(stencilState); }
    void SetViewport(const std::string& viewport) { prevPassData.viewport = passData.viewport; passData.viewport = GetConfigStringValue(viewport); }
    void SetShader(const std::string& shader) {
        prevPassData.shader = passData.shader; passData.shader = GetConfigStringValue(shader);
    }
    void SetMaterial(const std::string& material) { prevPassData.material = passData.material; passData.material = GetConfigStringValue(material); }
    void SetRasterizedState(const std::string& rasterizedState) {
        prevPassData.rasterizedState = passData.rasterizedState;
        passData.rasterizedState = GetConfigStringValue(rasterizedState);
    }
    void SetViewPortState(const std::string& viewPortState) { prevPassData.viewPortState = passData.viewPortState; passData.viewPortState = GetConfigStringValue(viewPortState); }
    void SetStencilState(const std::string& stencilState) { prevPassData.stencilState = passData.stencilState; passData.stencilState = GetConfigStringValue(stencilState); }
    void SetStencilDef(const std::string& stencilDef) { prevPassData.stencilDef = passData.stencilDef; passData.stencilDef = GetConfigStringValue(stencilDef); }
    void SetShaderViewState(const std::string& shaderViewState) { prevPassData.shaderViewState = passData.shaderViewState; passData.shaderViewState = GetConfigStringValue(shaderViewState); }
    void SetShadowTextureView(const bool& shadowTextureView) { prevPassData.shadowTextureView = passData.shadowTextureView; passData.shadowTextureView = shadowTextureView; }
    void SetBlendState(const std::string& blendState) { prevPassData.blendState = passData.blendState; passData.blendState = GetConfigStringValue(blendState); }
    void SetPrimitiveTopology(int primitiveTopology) { prevPassData.primitiveTopology = passData.primitiveTopology; passData.primitiveTopology = primitiveTopology; }
    void SetResolution(int resolution) { prevPassData.resolution = passData.resolution; passData.resolution = resolution; }
    void SetEnabled(const bool& enabled) { prevPassData.enabled = passData.enabled; passData.enabled = enabled; }
    void SetConstantBufferSlot(unsigned int slot, const PipelineSlot& pipelineSlot) {
        contantsBuffersSlots[slot] = pipelineSlot;
    }
    void SetResourceSlot(unsigned int slot, const PipelineSlot& pipelineSlot) {
        resourcesSlots[slot] = pipelineSlot;
    }
    void SetSamplerSlot(unsigned int slot, const PipelineSlot& pipelineSlot) {
        samplersSlots[slot] = pipelineSlot;
    }
    void SetCurrentSamplerSlot(const std::string& samplerSlot) {
        currentSamplerSlot = samplerSlot;
    }

    // MAPPERS
    void MapPassFromConfig(std::shared_ptr<PassConfigBase> config) {
        SetBlendState(config->blendState);
        SetPrimitiveTopology(config->primitiveTopology);
        SetRasterizedState(config->rasterizedState);
        SetResolution(config->resolution);
        SetShader(config->shader_name);
        SetShaderViewState(config->shaderViewState);
        SetStencilDef(config->stencilDef);
        SetStencilState(config->stencilState);
        SetViewPortState(config->viewPortState);
        SetId(config->id);
        SetShadowTextureView(config->shadowTextureView);
    }

    // PASS MAIN GETTERS
    std::string GetMainViewport() const { return mainViewport; }
    std::string GetMainStencilView() const { return mainStencilView; }
    std::string GetMainStencilState() const { return mainStencilState; }
    std::string GetCurrentConstantBuffer() const { return currentConstantBuffer; }
    void SetCurrentConstantBuffer(const std::string& constantBuffer) { currentConstantBuffer = constantBuffer; }

    // PASS CHANGED CHECKERS
    bool ChangedId() const { return passData.id != prevPassData.id; }
    bool ChangedViewport() const { return passData.viewport != prevPassData.viewport; }
    bool ChangedShader() const { return passData.shader != prevPassData.shader; }
    bool ChangedMaterial() const { return passData.material != prevPassData.material; }
    bool ChangedRasterizedState() const { return passData.rasterizedState != prevPassData.rasterizedState; }
    bool ChangedViewPortState() const { return passData.viewPortState != prevPassData.viewPortState; }
    bool ChangedStencilState() const { return passData.stencilState != prevPassData.stencilState; }
    bool ChangedStencilDef() const { return passData.stencilDef != prevPassData.stencilDef; }
    bool ChangedShaderViewState() const { return passData.shaderViewState != prevPassData.shaderViewState; }
    bool ChangedShadowTextureView() const { return passData.shadowTextureView != prevPassData.shadowTextureView; }
    bool ChangedBlendState() const { return passData.blendState != prevPassData.blendState; }
    bool ChangedPrimitiveTopology() const { return passData.primitiveTopology != prevPassData.primitiveTopology; }
    bool ChangedResolution() const { return passData.resolution != prevPassData.resolution; }

    // PASS SLOT USAGE CHECKERS
    bool IsConstantBufferSlotUsed(unsigned int slot) const {
        auto it = contantsBuffersSlots.find(slot);
        if (it != contantsBuffersSlots.end()) {
            return it->second.IsUsed();
        }
        return false;
    }
    bool IsResourceSlotUsed(unsigned int slot) const {
        auto it = resourcesSlots.find(slot);
        if (it != resourcesSlots.end()) {
            return it->second.IsUsed();
        }
        return false;
    }
    bool IsSamplerSlotUsed(unsigned int slot) const {
        auto it = samplersSlots.find(slot);
        if (it != samplersSlots.end()) {
            return it->second.IsUsed();
        }
        return false;
    }

    // MAIN PASS DATA STORAGE
    void SetPassData(const std::string& passName, PipelinePassStateData passData) {
        data[passName] = passData;
    }
    PipelinePassStateData GetPassData(const std::string& passName) {
        if (data.find(passName) != data.end()) {
            return data[passName];
        }
        return PipelinePassStateData();
    }

    // MAIN PASS STATE STORAGE
    void SetPassState(const std::string& passName) {
        if (data.find(passName) != data.end()) {
            passData = data[passName];
        }
    }

    // PASS INITIALIZATIONS
    void Init() {
        passData = {};
        prevPassData = {};
        passData.id = -1; prevPassData.id = -2;
        passData.enabled = true;
        passData.viewport = ""; prevPassData.viewport = "a";
        passData.shader = ""; prevPassData.shader = "a";
        passData.material = ""; prevPassData.material = "a";
        passData.rasterizedState = ""; prevPassData.rasterizedState = "a";
        passData.viewPortState = ""; prevPassData.viewPortState = "a";
        passData.stencilState = ""; prevPassData.stencilState = "a";
        passData.stencilDef = ""; prevPassData.stencilDef = "a";
        passData.shaderViewState = ""; prevPassData.shaderViewState = "a";
        passData.shadowTextureView = false; prevPassData.shadowTextureView = true;
        passData.blendState = ""; prevPassData.blendState = "a";
        passData.primitiveTopology = 3; prevPassData.primitiveTopology = -1;
        passData.resolution = 0; prevPassData.resolution = -1;
    }


private:
    // Control general
    std::string mainViewport;
    std::string mainStencilView;
    std::string mainStencilState;

    std::string currentConstantBuffer;
    std::string currentSamplerSlot;

    // Control de estados del pase actual y del pase anterior (para detectar cambios)
    PipelinePassStateData passData;
    PipelinePassStateData prevPassData;

    std::map<std::string, PipelinePassStateData> data;

    // Control de slots usados en el pase actual
    std::unordered_map<unsigned int, PipelineSlot> contantsBuffersSlots;
    std::unordered_map<unsigned int, PipelineSlot> resourcesSlots;
    std::unordered_map<unsigned int, PipelineSlot> samplersSlots;
};

using PipelineStateManager = PipelineState;
