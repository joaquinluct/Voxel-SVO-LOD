#pragma once
#include <d3d11.h>
#include <windows.h>
#include "Managers/RenderTargetManager.h"
#include "Camera/Camera.h"
#include "Managers/DeviceManager.h"
#include "Managers/ShaderManager.h"
#include "Managers/WorldMatrixManager.h"
#include "Material/Material.h"
#include "RenderObjects/Triangulo.h"
#include "RenderObjects/Axis/Axis.h"
#include "RenderObjects/Suelo/Suelo.h"
#include "RenderObjects/Axis/Axis.h"
#include "Devices/Mouse.h"
#include "Devices/Keyboard.h"
#include "UI/UI.h"
#include "UI/UIText.h"
#include "Util/Utils.h"
//#include "Voxel/Chunks/ChunkRenderer.h"
//#include "Voxel/Chunks/ChunkDebug.h"
#include "RenderObjects/UIBox/UIBox.h"
#include "SVO/World/World.h"
#include "SVO/Debug/OctreeDebug.h"
//#include "Voxel/VoxelMesh/VoxelDebug.h"

using namespace DirectX;

class MainGame{
public:
    MainGame();
	~MainGame();
    HRESULT Init(HWND hwnd);
    void Release();
    void Update(float deltaTime);
    void Render();
	Camera* GetCamera() { return g_camera; }    

private:
    HWND                    g_hwnd;
    DeviceManager*          g_device;
    RenderTargetManager*    g_renderTarget;
    Camera*                 g_camera;
	WorldMatrixManager*     g_worldMatrixManager;
    ShaderManager*          g_shaderManager;
    Material*               g_material;
    Triangulo*              g_triangulo;
    ID3D11Buffer*           g_matrixBuffer;
    Axis*                   g_axis;
	Suelo*                  g_suelo;
	UI*                     g_ui;
	//ChunkManager*           g_chunkManager;
	//ChunkRenderer*          g_chunkRenderer;
    World*                  g_world;
    /*VoxelDebug*             g_voxelDebug;
	ChunkDebug*             g_chunkDebug;*/
    OctreeDebug*            g_octreeDebug;

    HRESULT InitUI();
};
