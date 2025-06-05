// Nodo base para el SVO (Sparse Voxel Octree)
#pragma once
#include <array>
#include <memory>

class SVO_Node {
public:
    enum LODTransitionFace {
        FACE_X_NEG = 0,
        FACE_X_POS = 1,
        FACE_Y_NEG = 2,
        FACE_Y_POS = 3,
        FACE_Z_NEG = 4,
        FACE_Z_POS = 5
    };

    SVO_Node(bool isLeaf = false);
    ~SVO_Node();

    void Subdivide();
    bool HasChild(int index) const;
    SVO_Node* GetChild(int index) const;    
    void SetOccupied(bool occupied);
    bool IsOccupied() const;
    bool IsLeaf() const;
    void SetIsLeaf(bool isLeaf) { m_isLeaf = isLeaf; }
    void SetLODTransition(int face, bool value);
    bool GetLODTransition(int face) const;
    void ClearLODTransitions();

    void SetDensity(float density);
    inline float GetDensity() const { return m_density; };


    void SetIsChecked(bool isChecked) { m_isChecked = isChecked; };
    bool GetIsChecked() const { return m_isChecked; };

private:
    float m_density;
    bool m_isChecked;
    bool m_isLeaf;
    bool m_isOccupied;
    std::array<std::unique_ptr<SVO_Node>, 8> m_children;
    std::array<bool, 6> m_lodTransitions; // Marca si cada cara requiere transición LOD
};
