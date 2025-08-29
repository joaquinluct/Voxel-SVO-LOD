// VertexAsset.h
#pragma once
#include <Defines/VertexDefinition.h>
#include <Assets/Base/AssetBase.h>
#include <Assets/IAssetMesh.h>
#include <Assets/IAssetVertexConfig.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <stdint.h>
#include <VertexAssetConfigBase.h>

class VertexAsset : public AssetBase {
public:
    VertexAsset();
    ~VertexAsset() override;
    // IAsset overrides
    virtual std::shared_ptr<AssetBase> Clone() const override {
        // Crea una nueva instancia utilizando el constructor de copia
        // y la devuelve como un shared_ptr.
        return std::make_shared<VertexAsset>(*this);
    }
    virtual std::shared_ptr<MeshAsset> CloneAsMesh() const override {
        return nullptr;
    }
    virtual std::unique_ptr<AssetBase> CloneUnique() const override {
        // Crea una nueva instancia utilizando el constructor de copia
        // y la devuelve como un shared_ptr.
        return std::make_unique<VertexAsset>(*this);
    }
    void Load() override {};
    void Unload() override {};
    HRESULT Init() override;
    void Render() override;
    void Update(float deltaTime) override {};
    void Shutdown() override;
    const std::string& GetAssetName() override {
        static const std::string name = "VertexAsset";
        return name;
    }
    static const std::string& GetStaticAssetName()
    {
        static const std::string name = "VertexAsset";
        return name;
    }

    std::shared_ptr<VertexAsset> GetAsset(const std::string& name);

    // Getters para los buffers
	template <typename T>
    std::vector<T> GetVertex() {
        if (!m_config) {
            return {};
        }

        //auto vertexMesh = GetAsset(m_config->GetAssetName());
        
        //return vertexMesh->
        return {};
    }

    std::vector<uint16_t> GetIndex() {
        if (!m_config) {
            return {};
        }
        return m_config->GetIndex();
    }

    void SetConfig(std::shared_ptr<ConfigBase> config) {};

    void SetConfig(std::shared_ptr<VertexAssetConfigBase> config); 
    //{
    //    //m_config = std::static_pointer_cast<IAssetVertexConfig<VertexDefinition::BaseVertex>>(config);
    //}

private:
    std::shared_ptr<VertexAssetConfigBase> m_config;
    // Evitar copias
    /*VertexAsset(const VertexAsset&) = delete;
    VertexAsset& operator=(const VertexAsset&) = delete;*/
};