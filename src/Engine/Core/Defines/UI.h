#pragma once

#include <string>
#include <DirectXMath.h>

class Font
{
private:
	int m_size;
	XMFLOAT4 m_color;
public:	
	Font(int size, XMFLOAT4 color) : m_size(size), m_color(color) {}
	~Font() {};
	const void SetSize(int size) {
		m_size = size;
	}
	const int GetSize() const { return m_size; }
	const void SetColor(XMFLOAT4 color) { m_color = color; };
	const XMFLOAT4 GetColor() { return m_color; }
};
