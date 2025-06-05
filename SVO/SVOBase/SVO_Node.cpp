#include "SVO_Node.h"

SVO_Node::SVO_Node(bool isLeaf) : m_isLeaf(isLeaf), m_isOccupied(false), m_children{}, m_lodTransitions{}, m_isChecked(false), m_density(0.0f) {}
SVO_Node::~SVO_Node() {}

void SVO_Node::Subdivide() {
    if (!m_isLeaf) {
        for (int i = 0; i < 8; ++i) {
            m_children[i] = std::make_unique<SVO_Node>();
        }
    }
}

bool SVO_Node::HasChild(int index) const {
    return m_children[index] != nullptr;
}

SVO_Node* SVO_Node::GetChild(int index) const {
    return m_children[index].get();
}

void SVO_Node::SetOccupied(bool occupied) {
    m_isOccupied = occupied;
}

bool SVO_Node::IsOccupied() const {
    return m_isOccupied;
}

bool SVO_Node::IsLeaf() const {
    return m_isLeaf;
}

void SVO_Node::SetLODTransition(int face, bool value) {
    m_lodTransitions[face] = value;
}

bool SVO_Node::GetLODTransition(int face) const {
    return m_lodTransitions[face];
}

void SVO_Node::ClearLODTransitions() {
    m_lodTransitions.fill(false);
}

void SVO_Node::SetDensity(float density) {
    m_density = density;
}
    