#pragma once
#include <Windows.h>
#include <d3d11.h>
#include "DeviceManager.h"
#include <DirectXMath.h>
#include "../Interfaces/iDirectXManager.h"
#include "../Camera/Camera.h"
#include "../Managers/RenderTargetManager.h"
#include "../Util/Utils.h"


using namespace DirectX;

class WorldMatrixManager : public iDirectXManager
{
public:
	WorldMatrixManager();
	~WorldMatrixManager() override;

	HRESULT Init(DeviceManager* deviceManager);
	HRESULT Init() override;
	HRESULT Release() override;
	HRESULT Render(DeviceManager* g_device, RenderTargetManager* g_renderTarget, Camera* g_camera, const XMMATRIX& worldMatrix) const;
	HRESULT Render() override;
	
	ID3D11Buffer* GetMatrixBuffer() const { return g_matrixBuffer; } // Obtener el buffer de matrices
	//XMMATRIX GetProjectionMatrix() const { return projectionMatrix; } // Obtener la matriz de proyección
	HRESULT SetVertexShaderConstantBuffer(ID3D11DeviceContext* context, UINT slot, ID3D11Buffer* buffer);
	//void SetWorldMatrix(ID3D11DeviceContext* context, const DirectX::XMMATRIX& worldMatrix);

	ID3D11Buffer* g_matrixBuffer;
	MatrixBufferType g_matrix;


private:
};
