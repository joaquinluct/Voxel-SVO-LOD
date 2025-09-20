#include "VertexDefinition.h"
#include <REGISTER_DEFINE_MACRO.h>

using namespace VertexDefinition;

REGISTER_DEFINE_TYPE(VertexDefinition::SimpleVertex, SimpleVertex, "SimpleVertex")
REGISTER_DEFINE_TYPE(VertexDefinition::SimpleNormalVertex, SimpleNormalVertex, "SimpleNormalVertex")
REGISTER_DEFINE_TYPE(VertexDefinition::TextureBasicVertex, TextureBasicVertex, "TextureBasicVertex")
REGISTER_DEFINE_TYPE(VertexDefinition::TextureMapVertex, TextureMapVertex, "TextureMapVertex")
REGISTER_DEFINE_TYPE(VertexDefinition::TextVertex, TextVertex, "TextVertex")
REGISTER_DEFINE_TYPE(VertexDefinition::SkyboxVertex, SkyboxVertex, "SkyboxVertex")

namespace VertexDefinition {

    //std::shared_ptr<VertexBase> Factory::CreateVertex(std::string_view type) {
    //    if (type == VERTEX_TEXT) { // Usa tus constantes de cadena reales
    //        return std::make_shared<VertexBase>(TextVertex{}); // Construye el variant con un TextVertex
    //    }
    //    else if (type == VERTEX_SIMPLE) {
    //        return std::make_shared<VertexBase>(SimpleVertex{});
    //    }
    //    else if (type == VERTEX_SKYBOX) {
    //        return std::make_shared<VertexBase>(SkyboxVertex{});
    //    }
    //    else if (type == VERTEX_TEXTURE_BASIC) {
    //        return std::make_shared<VertexBase>(TextureBasicVertex{});
    //    }
    //    throw std::invalid_argument("Tipo de vértice desconocido: " + std::string(type));
    //}
}
