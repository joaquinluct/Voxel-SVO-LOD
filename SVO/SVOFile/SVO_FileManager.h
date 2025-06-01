#pragma once
#include "../SVOBase/SVO_Node.h"
#include <string>
#include <fstream>

class SVO_FileManager {
public:
    static void SaveSVO(const SVO_Node* root, const std::string& filename);
    static SVO_Node* LoadSVO(const std::string& filename);

private:
    static void SaveNode(const SVO_Node* node, std::ofstream& file);
    static SVO_Node* LoadNode(std::ifstream& file);
};
