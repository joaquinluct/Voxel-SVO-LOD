// DefineLocator.cpp
#include "DefineLocator.h"
#include <iostream> // Para debugging si quieres usar std::cout en lugar de OutputDebugStringA
#include <windows.h> // Para OutputDebugStringA
#include <string> // Para std::to_string

std::map<std::string, DefineLocator::DefineEntry>& DefineLocator::GetDefineEntries() {
    static std::map<std::string, DefineEntry> s_defineEntries; 
    return s_defineEntries;
}

DefineLocator& DefineLocator::GetInstance() {
    static DefineLocator instance; // Se crea una vez, la primera vez que se llama
    return instance;
}

void DefineLocator::RegisterDefineCreator(
    const std::string& name,
    CreateDefineLambda createFn) {
    auto& entries = DefineLocator::GetDefineEntries();
    if (entries.count(name)) {
        OutputDebugStringA(("WARNING: Define creator for '" + name + "' already registered. Overwriting.\n").c_str());
    }
    //s_defineCreators[name] = { createFn, initFn };
    entries[name] = {nullptr, createFn};
}

HRESULT DefineLocator::InitializeDefines() {
    auto& entries = DefineLocator::GetDefineEntries();
    for (auto& entry: entries) {
        entry.second.instance = entry.second.creator();
        if (!entry.second.instance) {
            OutputDebugStringA(("ERROR: Failed to create instance for define '" + entry.first + "'\n").c_str());
            return E_FAIL;
        }
    }
    return S_OK;
}

std::shared_ptr<IVertex> DefineLocator::GetDefine(const std::string& name) {
    auto& entries = DefineLocator::GetDefineEntries();
    auto it = entries.find(name);
    if (it != entries.end()) {
        return entries[name].instance;
    }
    return nullptr;
}

std::shared_ptr<IVertex> DefineLocator::GetVertexDefine(const std::string& name) {
    auto& entries = DefineLocator::GetDefineEntries();
    auto it = entries.find(name);
    if (it != entries.end()) {
        return entries[name].instance;
    }
    return nullptr;
}

std::vector<std::shared_ptr<VertexDefinition::VertexVariant>> DefineLocator::GetVertexDefineAsVector(const std::string& name) {
    auto& entries = DefineLocator::GetDefineEntries();
    auto it = entries.find(name);
    if (it != entries.end()) {
        std::shared_ptr<IVertex> base_instance = entries[name].instance;

        if (!base_instance) {
            return {}; // No hay instancia
        }

        // Aquí es donde necesitas averiguar el tipo concreto y construir el variant
        // Intentar dynamic_pointer_cast a cada tipo posible que pueda contener VertexVariant
        if (auto simple_vtx = std::dynamic_pointer_cast<VertexDefinition::SimpleVertex>(base_instance)) {
            // Si es un SimpleVertex, lo metemos en el variant
            std::shared_ptr<VertexDefinition::VertexVariant> v =
                std::make_shared<VertexDefinition::VertexVariant>(*simple_vtx); // Desreferenciar el shared_ptr
            return { v };
        }
        else if (auto texture_vtx = std::dynamic_pointer_cast<VertexDefinition::TextureBasicVertex>(base_instance)) {
            std::shared_ptr<VertexDefinition::VertexVariant> v =
                std::make_shared<VertexDefinition::VertexVariant>(*texture_vtx); // Desreferenciar el shared_ptr
            return { v };
        }        
        else if (auto texture_map_vtx = std::dynamic_pointer_cast<VertexDefinition::TextureMapVertex>(base_instance)) {
            std::shared_ptr<VertexDefinition::VertexVariant> v =
                std::make_shared<VertexDefinition::VertexVariant>(*texture_map_vtx); // Desreferenciar el shared_ptr
            return { v };
        }
        else if (auto simpleNormal_vtx = std::dynamic_pointer_cast<VertexDefinition::SimpleNormalVertex>(base_instance)) {
            std::shared_ptr<VertexDefinition::VertexVariant> v =
                std::make_shared<VertexDefinition::VertexVariant>(*simpleNormal_vtx); // Desreferenciar el shared_ptr
            return { v };
        }
        else if (auto skybox_vtx = std::dynamic_pointer_cast<VertexDefinition::SkyboxVertex>(base_instance)) {
            std::shared_ptr<VertexDefinition::VertexVariant> v =
                std::make_shared<VertexDefinition::VertexVariant>(*skybox_vtx); // Desreferenciar el shared_ptr
            return { v };
        }
        // Agrega más 'else if' para cada tipo concreto que VertexVariant pueda contener

        // Si el tipo no coincide con ningún tipo conocido en VertexVariant
        std::cerr << "Error: Tipo de vértice obtenido del locator no es compatible con VertexVariant.\n";
        return {};
    }
    return {}; // Si no se encontró la entrada
}



void DefineLocator::Shutdown() {
    auto& entries = DefineLocator::GetDefineEntries();
	entries.clear(); // Limpia las entradas de servicios
    
    OutputDebugStringA("DefineLocator: All defines and creators shut down.\n");
}