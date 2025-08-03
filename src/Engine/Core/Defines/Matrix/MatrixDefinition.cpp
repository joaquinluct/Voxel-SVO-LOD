// MatrixDefinition.cpp
#include "MatrixDefinition.h"
#include <iostream>
#include <variant>

namespace MatrixDefinition {

 //   // 5. Implementación de los métodos virtuales fuera de las clases
 //   void MatrixBufferType::SetMatrixBufferType(AnyMatrixBuffer& matrix) {
 //       if (auto* m = std::get_if<MatrixBufferType>(&matrix)) {
 //           worldMatrix = m->worldMatrix;
 //           viewMatrix = m->viewMatrix;
 //           projectionMatrix = m->projectionMatrix;
 //       }
 //       else {
 //           // Opcional: std::cerr << "Error: Intentando asignar un tipo de MatrixBufferType incorrecto." << std::endl;
 //       }
 //   }

 //   void MatrixBufferType::SetMatrixBuffer(DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix) {
 //       this->worldMatrix = worldMatrix;
 //       this->viewMatrix = viewMatrix;
	//	this->projectionMatrix = projectionMatrix;
	//}

 //   void MatrixBufferTypeSkyBox::SetMatrixBufferType(AnyMatrixBuffer& matrix) {
 //       if (auto* m = std::get_if<MatrixBufferTypeSkyBox>(&matrix)) {
 //           viewMatrix = m->viewMatrix;
 //           projectionMatrix = m->projectionMatrix;
 //       }
 //       else {
 //           // Opcional: std::cerr << "Error: Intentando asignar un tipo de MatrixBufferTypeSkyBox incorrecto." << std::endl;
 //       }
 //   }

 //   void MatrixBufferTypeSkyBox::SetMatrixBuffer(DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix) {
 //       this->viewMatrix = viewMatrix;
 //       this->projectionMatrix = projectionMatrix;
 //   }

 //   void MatrixBufferTypeOrthographic::SetMatrixBufferType(AnyMatrixBuffer& matrix) {
 //       if (auto* m = std::get_if<MatrixBufferTypeOrthographic>(&matrix)) {
 //           projectionMatrix = m->projectionMatrix;
 //       }
 //       else {
 //           // Opcional: std::cerr << "Error: Intentando asignar un tipo de MatrixBufferTypeOrthographic incorrecto." << std::endl;
 //       }
 //   }

 //   void MatrixBufferTypeOrthographic::SetMatrixBuffer(DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix) {
 //       this->projectionMatrix = projectionMatrix;
	//}

 //   void MatrixBase::SetConstantsBuffers(ID3D11Buffer* m_matrixBuffer, const MatrixDefinition::AnyMatrixBuffer& matrixVariant, ID3D11DeviceContext* context, int slot) {
 //       D3D11_MAPPED_SUBRESOURCE mapped = {};
 //       context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

 //       // Usa std::visit para copiar el tipo de matriz correcto
 //       std::visit([&](const auto& currentMatrixStruct) {
 //           // currentMatrixStruct será una referencia a MatrixBufferType, MatrixBufferTypeSkyBox, etc.
 //           // Esto copia el tamaño correcto de la estructura activa.
 //           memcpy(mapped.pData, &currentMatrixStruct, sizeof(currentMatrixStruct));
 //           }, matrixVariant);

 //       context->Unmap(m_matrixBuffer, 0);
 //       context->VSSetConstantBuffers(slot, 1, &m_matrixBuffer);
 //   }
} // namespace MatrixDefinition