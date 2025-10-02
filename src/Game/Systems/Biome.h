#pragma once
#include <Config/Game/System/BiomeConfig.h>
#include <Defines/Enums/Biome.h>
#include <Defines/Structs/BiomeItem.h>
#include <Game/Systems/Terrain/ProceduralService.h>
#include <IService.h>
#include <map>
#include <memory>
#include <string>
#include <windows.h>

class Biome : public IService
{
public:
    Biome();
    ~Biome() override;

    HRESULT Init() override;
    HRESULT InitConfig();
    HRESULT InitManagers();
    HRESULT InitServices();
    HRESULT InitBiomes();
    void Render() override;
    void Update(float deltaTime) override;
    void Shutdown() override {};
    const std::string& GetServiceName() const override {
        static const std::string name = "Biome";
        return name;
    }
    static const std::string& GetStaticServiceName()
    {
        static const std::string name = "Biome";
        return name;
    }
    // Nueva función auxiliar para determinar el dominio
    [[nodiscard]] BiomeDomain GetDomainAtY(float y) const;
    [[nodiscard]] int CalculateProceduralBiomeId(float x, float z) const;
    [[nodiscard]] const BiomeItem& GetBiomeAt(float x, float y, float z) const;

private:
    std::shared_ptr<ProceduralService> m_proceduralService;

    BiomeConfig m_config = {};
    BiomeType m_type = BiomeType::None;
    std::map<int, BiomeItem> m_biomes;
};
