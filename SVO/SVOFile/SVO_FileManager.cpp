#include "SVO_FileManager.h"
#include <fstream>

void SVO_FileManager::SaveSVO(const SVO_Node* root, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) return;
    SaveNode(root, file);
    file.close();
}

SVO_Node* SVO_FileManager::LoadSVO(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) return nullptr;
    SVO_Node* root = LoadNode(file);
    file.close();
    return root;
}

void SVO_FileManager::SaveNode(const SVO_Node* node, std::ofstream& file) {
    if (!node) return;
    bool isOccupied = node->IsOccupied();
    file.write(reinterpret_cast<const char*>(&isOccupied), sizeof(bool));
    for (int i = 0; i < 8; ++i) {
        SaveNode(node->GetChild(i), file);
    }
}

SVO_Node* SVO_FileManager::LoadNode(std::ifstream& file) {  
    bool isOccupied;  
    file.read(reinterpret_cast<char*>(&isOccupied), sizeof(bool));  
    SVO_Node* node = new SVO_Node();  
    node->SetOccupied(isOccupied);  
    for (int i = 0; i < 8; ++i) {  
        if (file.peek() != EOF) {  
            node->Subdivide();  
            node->GetChild(i)->SetOccupied(LoadNode(file)->IsOccupied());  
        }  
    }  
    return node;  
}
