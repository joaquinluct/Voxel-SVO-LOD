#pragma once

namespace Mesh
{
	enum class Type
	{
		None = -1,
		Simple = 0,
		File_Obj = 1,
		Text = 2,
		Terrain = 3,
	};
	enum class RenderType
	{
		None,
		Opaque,
		AlphaTested
	};

	enum class DrawType
	{
		None = 0,
		DrawIndexed = 1,
		Draw = 2,
		DrawInstanced = 3,
	};
}
