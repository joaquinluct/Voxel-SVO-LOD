# Samplers en Gráficos 3D

## Reutilización de Samplers

**Pregunta: ¿Puedo tener una lista de samplers y usarlos en diferentes texturas?**

¡Sí, y esto es la práctica recomendada! Los motores gráficos suelen definir un conjunto pequeño y limitado de "estados de muestreo" (sampler states) predefinidos que cubren la mayoría de los casos de uso. Estos samplers se crean una sola vez al inicio de la aplicación o del motor y se reutilizan.

## Ejemplos de Samplers Comunes

### Sampler Lineal con Wrap
- **Uso**: Para la mayoría de las texturas de color (`_MainTex`, `_Albedo`)
- **Características**: Filtrado suave y repetición de la textura cuando las UVs se salen del rango [0,1]
- **Aplicaciones**: Texturas de terreno repetitivas, patrones, etc.

### Sampler Puntual con Clamp
- **Uso**: Para texturas que representan datos discretos o que no deben repetirse
- **Ejemplos**: Mapas de sombras (`_ShadowMap`), texturas de interfaz de usuario (`_UIAtlas`)
- **Aplicaciones**: Texturas que se generan proceduralmente y no tienen sentido fuera del rango

### Sampler Lineal con Clamp
- **Características**: Similar al anterior, pero con filtrado lineal
- **Aplicaciones**: Texturas que se estiran hasta los bordes del UV (como un terreno que no se repite)

### Sampler Anisotrópico
- **Uso**: Para texturas que se ven desde ángulos oblicuos
- **Características**: Filtrado de mayor calidad para reducir el aliasing
- **Aplicaciones**: Superficies de carreteras, terrenos vistos a distancia

### Samplers de Comparación (Comparison Samplers)
- **Uso**: Específicos para mapas de profundidad y sombras
- **Función**: Comparan el valor de profundidad muestreado con la profundidad actual del píxel
- **Implementación**: Usando `SamplerComparisonState` en HLSL y `D3D11_FILTER_COMPARISON_*` en `D3D11_SAMPLER_DESC`

## Buenas Prácticas

Cuando un material o un objeto necesita una textura, no se le asigna un sampler individual. En cambio, se le asigna una referencia a uno de estos samplers preexistentes, lo que mejora el rendimiento y reduce el uso de recursos.

