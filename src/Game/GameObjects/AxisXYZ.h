#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <DeviceManager.h>
#include <CameraManager.h>
#include <WorldMatrixManager.h>
#include <IService.h>
#include "../../../RenderObjects/Line/Line.h"
#include "../../../RenderObjects/UIBox/UIBox.h"

using namespace DirectX;

class AxisXYZ : public IService {
public:
	AxisXYZ();
    ~AxisXYZ() override;

	HRESULT Init() override;
	HRESULT Init(std::shared_ptr<ID3D11Device> device) { return S_OK; };
	void Render() override;
	void Render(ID3D11DeviceContext* context) {};
	void Update(float deltaTime) override {};
	void Release() { Shutdown(); };
	void Shutdown() override;
	const std::string& GetServiceName() const override {
		static const std::string name = "AxisXYZ"; // Esta cadena se crea una sola vez y vive durante toda la ejecución del programa.
		return name;
	}
	static const std::string& GetStaticManagerName()
	{
		static const std::string name = "AxisXYZ"; // Esta cadena se crea una sola vez y vive durante toda la ejecución del programa.
		return name;
	}
    
    ID3D11Buffer* GetVertexBuffer() { return nullptr; }
		//void SetKeyboardManager(KeyboardManager* keyboardManager) override {}

private:
    Material* m_material;
    std::vector<Line*> m_lines; // Usar un vector para almacenar las líneas
    std::vector<UIBox*> boxes;
	std::shared_ptr<DeviceManager> m_deviceManager;
	std::shared_ptr<WorldMatrixManager> m_worldMatrixManager;
	std::shared_ptr<CameraManager> m_cameraManager;
};