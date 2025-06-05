#include "DirectXUtils.h"

namespace DirectXUtils
{
    // Definir una tolerancia pequeña para comparaciones de punto flotante
    const float EPSILON = 0.0001f;
    	
    XMFLOAT3 AddInt(const XMFLOAT3& v1, const int v2[3]) {
        // Cargar los XMFLOAT3 a XMVECTOR  
        XMVECTOR vecA = XMLoadFloat3(&v1);  
        XMFLOAT3 v2_as_float3 = { static_cast<float>(v2[0]), static_cast<float>(v2[1]), static_cast<float>(v2[2]) };
        XMVECTOR vecB = XMLoadFloat3(&v2_as_float3);
        // Realizar la suma  
        XMVECTOR resultVec = XMVectorAdd(vecA, vecB);  
        // Almacenar el resultado de vuelta en un XMFLOAT3  
        XMFLOAT3 result;  
        XMStoreFloat3(&result, resultVec);  
        return result;  
    }
          
    XMFLOAT3 Add(const XMFLOAT3& v1, const XMFLOAT3& v2)
    {
        // Cargar los XMFLOAT3 a XMVECTOR
        XMVECTOR vecA = XMLoadFloat3(&v1);
        XMVECTOR vecB = XMLoadFloat3(&v2);

        // Realizar la suma
        XMVECTOR resultVec = XMVectorAdd(vecA, vecB);

        // Almacenar el resultado de vuelta en un XMFLOAT3
        XMFLOAT3 result;
        XMStoreFloat3(&result, resultVec);
        return result;
	}

    bool AreEqual(const XMFLOAT3& v1, const XMFLOAT3& v2)
    {
        // 1. Cargar los XMFLOAT3 a XMVECTOR
        XMVECTOR xmVec1 = XMLoadFloat3(&v1);
        XMVECTOR xmVec2 = XMLoadFloat3(&v2);

        // 2. Crear un XMVECTOR de tolerancia (todas las componentes con EPSILON)
        XMVECTOR xmEpsilon = XMVectorReplicate(EPSILON);

        // 3. Realizar la comparación optimizada
        // XMVector3NearEqual compara si los vectores son "casi iguales" dentro de la tolerancia.
        // Devuelve un booleano (como un int) si son iguales.
        return XMVector3NearEqual(xmVec1, xmVec2, xmEpsilon);
    }

    XMFLOAT3 Subtract(const XMFLOAT3& a, const XMFLOAT3& b) {
        XMVECTOR vecA = XMLoadFloat3(&a);
        XMVECTOR vecB = XMLoadFloat3(&b);
        XMVECTOR resultVec = XMVectorSubtract(vecA, vecB);
        XMFLOAT3 result;
        XMStoreFloat3(&result, resultVec);
        return result;
    }

    XMFLOAT3 Multiply(const XMFLOAT3& a, float scalar) {
        XMVECTOR vecA = XMLoadFloat3(&a);
        XMVECTOR scalarVec = XMVectorReplicate(scalar); // Crear un XMVECTOR con el escalar en todas las componentes
        XMVECTOR resultVec = XMVectorMultiply(vecA, scalarVec);
        XMFLOAT3 result;
        XMStoreFloat3(&result, resultVec);
        return result;
    }

    XMFLOAT3 Divide(const XMFLOAT3& a, float scalar) {
        XMVECTOR vecA = XMLoadFloat3(&a);
        XMVECTOR scalarVec = XMVectorReplicate(scalar);
        XMVECTOR resultVec = XMVectorDivide(vecA, scalarVec);
        XMFLOAT3 result;
        XMStoreFloat3(&result, resultVec);
        return result;
    }

    XMFLOAT3 Normalize(const XMFLOAT3& v) {
        XMVECTOR vec = XMLoadFloat3(&v);
        XMVECTOR resultVec = XMVector3Normalize(vec);
        XMFLOAT3 result;
        XMStoreFloat3(&result, resultVec);
        return result;
    }

    float Dot(const XMFLOAT3& a, const XMFLOAT3& b) {
        XMVECTOR vecA = XMLoadFloat3(&a);
        XMVECTOR vecB = XMLoadFloat3(&b);
        XMVECTOR resultVec = XMVector3Dot(vecA, vecB);
        // El resultado del producto punto está en todas las componentes de resultVec
        // Puedes extraerlo con XMVectorGetX, Y, Z, o W. Para producto punto, da igual cuál.
        return XMVectorGetX(resultVec);
    }

    XMFLOAT3 Cross(const XMFLOAT3& a, const XMFLOAT3& b) {
        XMVECTOR vecA = XMLoadFloat3(&a);
        XMVECTOR vecB = XMLoadFloat3(&b);
        XMVECTOR resultVec = XMVector3Cross(vecA, vecB);
        XMFLOAT3 result;
        XMStoreFloat3(&result, resultVec);
        return result;
    }

    XMFLOAT3 Lerp(const XMFLOAT3& v1, const XMFLOAT3& v2, float t)
    {
        // Cargar los XMFLOAT3 a XMVECTOR
        XMVECTOR vecA = XMLoadFloat3(&v1);
        XMVECTOR vecB = XMLoadFloat3(&v2);
        // Realizar la interpolación lineal
        XMVECTOR resultVec = XMVectorLerp(vecA, vecB, t);
        // Almacenar el resultado de vuelta en un XMFLOAT3
        XMFLOAT3 result;
        XMStoreFloat3(&result, resultVec);
        return result;
	}

    XMFLOAT3 Transform(const XMFLOAT3& v, const XMMATRIX& m)
    {
        // Cargar el XMFLOAT3 a XMVECTOR
        XMVECTOR vec = XMLoadFloat3(&v);
        // Realizar la transformación
        XMVECTOR resultVec = XMVector3Transform(vec, m);
        // Almacenar el resultado de vuelta en un XMFLOAT3
        XMFLOAT3 result;
        XMStoreFloat3(&result, resultVec);
        return result;
	}
    

    // Función más específica para verificar si una normal apunta en una dirección cardinal
    bool IsNormalPointing(const XMFLOAT3& normal, const XMFLOAT3& direction, float dotThreshold) {
        // Asegúrate de que ambas normales estén normalizadas antes de llamar a esto
        XMVECTOR n = XMLoadFloat3(&normal);
        XMVECTOR dir = XMLoadFloat3(&direction); // Asegúrate de que 'direction' sea un vector unitario (ej. {0,1,0} para arriba)

        // El producto escalar de dos vectores unitarios es el coseno del ángulo entre ellos.
        // Si apuntan en la misma dirección, el coseno es 1.
        // Usamos un umbral para la tolerancia.
        return XMVectorGetX(XMVector3Dot(n, dir)) > dotThreshold;
    }
}