#include "WorldPerling.h"

WorldPerling::WorldPerling(): m_material(nullptr) {}
WorldPerling::~WorldPerling() {}
HRESULT WorldPerling::Init(Material* material) { return S_OK; }
void WorldPerling::Release() {}
void WorldPerling::Update(float deltaTime) {}
void WorldPerling::Render(ID3D11DeviceContext* context) {}