// WorldMatrixManager.h
#pragma once

#include "IManager.h"
#include "IInitializable.h" // Para Init(const YAML::Node&)
#include "IUpdatable.h"     // Si decide actualizar el buffer en cada Update global
#include "IRenderable.h"    // Para Render (la fase de renderizado)
#include "IShutdownable.h"  // Para Release
#include "Utils.h"
#include <Defines/MatrixDefinition.h>

#include <d3d11.h>
#include <DirectXMath.h>
#include <memory> // Para std::shared_ptr

// Forward declarations para evitar inclusiones completas si es posible
// Pero dado que los usarás en la firma de Render, la inclusión completa es necesaria.
// #include "DeviceManager.h"
// #include "CameraManager.h" // Asumiendo que Camera es parte de CameraManager
// #include "RenderTargetManager.h"

class DeviceManager; // Forward declaration
class CameraManager; // Forward declaration
class RenderTargetManager; // Forward declaration

class WorldMatrixManager : public IManager, public IInitializable, public IRenderable, public IUpdatable, public IShutdownable {
public:
    WorldMatrixManager();
     ~WorldMatrixManager() override;

    const std::string& GetManagerName() const override { 
        static const std::string name = "WorldMatrixManager";
        return name;
    }
    static const std::string& GetStaticManagerName()
    {
        static const std::string name = "WorldMatrixManager";
        return name;
    }

    // Init con configuración YAML
    HRESULT Init();
    void Render() override; // La firma simplificada para IRenderable
    void Update(float deltaTime) override {};
    // Init con dependencias (la firma que tenías con DeviceManager)
    // Este método debería ser llamado internamente por Init(configNode)
    // O bien, Init(configNode) se encarga de obtener las dependencias.
    HRESULT InitInternal(std::shared_ptr<DeviceManager> device);

    // Método para actualizar las matrices, llamado por MainGame/GameManager antes de renderizar objetos
    // El GameManager o cada objeto pasará su propia worldMatrix si es diferente.
    void SetGlobalMatrices(const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection);

    // Getter para el buffer si algún shader lo necesita directamente (raro, normalmente lo vincula el manager)
    //ID3D11Buffer* GetMatrixBuffer() const { return g_matrixBuffer; }

    // Liberación de recursos
    void Shutdown() override; // Implementa IShutdownable

private:
    ID3D11Buffer* g_matrixBuffer;
    MatrixDefinition::AnyMatrixBuffer g_matrix; // Guarda las matrices actuales

    // Punteros a managers que se obtendrán del ServiceLocator en Init
    std::shared_ptr<DeviceManager> m_device;
    std::shared_ptr<CameraManager> m_cameraManager;
    std::shared_ptr<RenderTargetManager> m_renderTargetManager;

};