#include "Biome.h"
#include <Base/Enum.h>
#include <Config/BiomeBaseConfig.h>
#include <Config/Game/System/BiomeConfig.h>
#include <ConfigLocator/ConfigLocator.h>
#include <debugapi.h>
#include <Defines/Enums/Biome.h>
#include <Defines/Structs/BiomeItem.h>
#include <Game/Systems/Terrain/Procedural/Engines/ProceduralEngineBase.h>
#include <Game/Systems/Terrain/ProceduralService.h>
#include <memory>
#include <REGISTER_SERVICE_MACRO.h>
#include <ServiceLocator/ServiceLocator.h>
#include <string>
#include <vector>
#include <Windows.h>

REGISTER_SERVICE_TYPE(Biome, "Biome")

Biome::Biome()
{
    m_type = BiomeType::None;
}

Biome::~Biome()
{
}
HRESULT Biome::InitConfig() {
    m_type = to_enum<BiomeType>(m_config.biome_type);
    return S_OK;
}
HRESULT Biome::InitManagers() {

    return S_OK;
}
HRESULT Biome::InitServices() {
    m_proceduralService = ServiceLocator::GetService<ProceduralService>();
    if (!m_proceduralService) {
        OutputDebugStringA("Biome: Register procedural service fail.\n");
        return E_FAIL;
    }
    return S_OK;
}

HRESULT Biome::InitBiomes()
{
    std::vector<std::string> biomeNames = m_config.biomes;

    if (biomeNames.empty()) {
        OutputDebugStringA("Biome: No biomes defined in configuration.\n");
        return E_FAIL;
    }

    for (const auto& name : biomeNames) {
        if (name.empty()) {
            OutputDebugStringA("Biome: Encountered empty biome name in configuration.\n");
            continue;
        }
        std::shared_ptr<BiomeBaseConfig> config = ConfigLocator::GetConfig<BiomeBaseConfig>(name + "Config");
        if (!config) {
            OutputDebugStringA(("Biome: Failed to load config for biome: " + name + "\n").c_str());
            continue;
        }

        int id = config->id;

        BiomeItem item = BiomeItem(config->name, id, config);

        m_biomes[id] = item;
    }

    return S_OK;
}

HRESULT Biome::Init()
{
    if (this->IsInitialized()) {
        return S_OK;
    }

    HRESULT hr = InitConfig();
    if (FAILED(hr)) {
        OutputDebugStringA("Biome: Init config fail.\n");
        return hr;
    }

    hr = InitManagers();
    if (FAILED(hr)) {
        OutputDebugStringA("Biome: Init managers fail.\n");
        return hr;
    }

    hr = InitServices();
    if (FAILED(hr)) {
        OutputDebugStringA("Biome: Init services fail.\n");
        return hr;
    }

    hr = InitBiomes();
    if (FAILED(hr)) {
        OutputDebugStringA("Biome: Init biomes fail.\n");
        return hr;
    }

    this->SetInitialized(true);

    return S_OK;
}

[[nodiscard]] int Biome::CalculateProceduralBiomeId(float x, float z) const {

    // Comprobamos el servicio y obtenemos el motor.
    if (m_proceduralService)
    {
        ProceduralEngineBase* activeEngine = m_proceduralService->GetEngine().get();

        if (activeEngine)
        {
            float biomeIdFloat = activeEngine->GetHeight(x, z);
            return static_cast<int>(biomeIdFloat);
        }
    }

    // Fallback si el servicio no existe o el motor no está inicializado.
    return 0; // Usar el ID 0 como fallback (asumimos que es el bioma por defecto).
}

// Implementación de la función auxiliar para determinar el dominio
BiomeDomain Biome::GetDomainAtY(float y) const {
    if (y > m_config.ocean_level) {
        return BiomeDomain::Land;
    }
    else {
        return BiomeDomain::Water;
    }
}


[[nodiscard]] const BiomeItem& Biome::GetBiomeAt(float x, float y, float z) const {

    int biomeIdToUse = 0;

    if (m_type == BiomeType::Procedural) {
        // Llama al motor procedural usando la notación PascalCase.
        biomeIdToUse = CalculateProceduralBiomeId(x, z);
    }
    else if (m_type == BiomeType::Fixed) {
        // Lógica BiomeType::Fixed: Usa el primer bioma cargado como fijo.
        if (!m_biomes.empty()) {
            biomeIdToUse = m_biomes.begin()->first;
        }
    }

    // Obtener el BiomeItem del mapa de biomas disponibles.
    if (m_biomes.count(biomeIdToUse)) {
        return m_biomes.at(biomeIdToUse);
    }

    // Fallback: Devolver el bioma por defecto.
    return BiomeItem("Default", 0, nullptr);
}

void Biome::Update(float deltaTime)
{

}

void Biome::Render()
{
}
