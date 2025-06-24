#pragma once
#include <string>
#include <format>

namespace VertexDefinition {

	struct SimpleVertex {
		float position[3];
		std::string ToOBJLine(const VertexDefinition::SimpleVertex& v) {
			return std::format("v {} {} {}", v.position[0], v.position[1], v.position[2]);
		}
	};

	struct SimpleNormalVertex {
		float position[3];
		float normal[3];
		std::string ToOBJLine(const VertexDefinition::SimpleNormalVertex& v) {
			return std::format("v {} {} {}\nvn {} {} {}",
				v.position[0], v.position[1], v.position[2],
				v.normal[0], v.normal[1], v.normal[2]);
		}
	};

	struct SimpleTextureVertex {
		float position[3];
		float normal[3];
		float texCoord[2];
		std::string ToOBJLine(const VertexDefinition::SimpleTextureVertex& v) {
			return std::format("v {} {} {}\nvn {} {} {}\nvt {} {}",
				v.position[0], v.position[1], v.position[2],
				v.normal[0], v.normal[1], v.normal[2],
				v.texCoord[0], v.texCoord[1]);
		}
	};
}