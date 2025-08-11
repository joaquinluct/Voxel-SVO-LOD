
# Construcción de un Skybox DDS con Cubemap
# Construcción de un Skybox DDS con Cubemap

Este documento explica cómo construir un archivo `.dds` para un skybox usando cubemaps, tanto a partir de una sola imagen como de una imagen en formato 3x2. También se detalla cómo extraer las 6 caras necesarias y el uso de la herramienta `texconv` para la conversión y manipulación de imágenes.

## 1. ¿Qué es un Cubemap Skybox?

Un cubemap es una textura compuesta por 6 imágenes cuadradas que representan las caras de un cubo: **+X, -X, +Y, -Y, +Z, -Z**. Estas imágenes se utilizan para simular entornos 3D envolventes, como cielos (skyboxes) o reflejos.

## 2. Construcción a partir de una sola imagen

Si tienes una imagen panorámica o equirectangular, puedes convertirla en un cubemap usando herramientas como [ImageMagick](https://imagemagick.org/) o utilidades específicas de gráficos 3D. El proceso general es:

1. **Convertir la imagen panorámica a 6 caras** usando una herramienta de proyección (por ejemplo, [CubeMapGen](https://github.com/ptitSeb/CubeMapGen) o scripts de Python).
2. **Asegurarse de que cada cara sea cuadrada** y tenga la misma resolución.
3. **Empaquetar las 6 caras en un archivo DDS** usando una herramienta como [texconv](https://github.com/microsoft/DirectXTex/wiki/Texconv).

## 3. Construcción a partir de una imagen 3x2

Una imagen 3x2 contiene las 6 caras del cubemap dispuestas en una cuadrícula de 3 columnas y 2 filas. El orden habitual es:

### Extracción de las 6 caras con ImageMagick

Para extraer las 6 imágenes cuadradas de una imagen 3x2 llamada `skybox_day_night.png`, usa el siguiente comando en la terminal de Windows (cmd):- `-crop 3x2@` divide la imagen en 6 partes iguales (3 columnas x 2 filas).
- `+repage` elimina la información de página virtual.
- `skybox_ace_%%d.png` guarda cada cara como un archivo numerado del 0 al 5.

**Nota:** El orden de las caras extraídas puede variar según la herramienta o el estándar que sigas. Asegúrate de renombrar o reorganizar los archivos según el orden requerido por tu motor gráfico.

### Ejemplo de correspondencia de caras

| Índice | Cara  |
|--------|-------|
| 0      | +X    |
| 1      | -X    |
| 2      | +Y    |
| 3      | -Y    |
| 4      | +Z    |
| 5      | -Z    |

## 4. Creación del DDS con texconv

Una vez tengas las 6 imágenes cuadradas, usa el siguiente comando en la terminal de Windows para crear el DDS cubemap:- `-cube` indica que se va a crear un cubemap.
- El orden de los archivos debe coincidir con el estándar de tu motor.

## 5. Resumen del proceso

1. **Extraer las 6 caras** de la imagen 3x2 (o convertir desde panorámica).
2. **Asegurarse de que todas las imágenes sean cuadradas y del mismo tamaño.**
3. **Usar texconv** para empaquetar las imágenes en un DDS cubemap.

## 6. Recursos útiles

- [ImageMagick](https://imagemagick.org/)
- [texconv (DirectXTex)](https://github.com/microsoft/DirectXTex/wiki/Texconv)
- [CubeMapGen](https://github.com/ptitSeb/CubeMapGen)
Este documento explica cómo construir un archivo `.dds` para un skybox usando cubemaps, tanto a partir de una sola imagen como de una imagen en formato 3x2. También se detalla cómo extraer las 6 caras necesarias y el uso de la herramienta `textconv` para la conversión y manipulación de imágenes.

## 1. ¿Qué es un Cubemap Skybox?

Un cubemap es una textura compuesta por 6 imágenes cuadradas que representan las caras de un cubo: **+X, -X, +Y, -Y, +Z, -Z**. Estas imágenes se utilizan para simular entornos 3D envolventes, como cielos (skyboxes) o reflejos.

## 2. Construcción a partir de una sola imagen

Si tienes una imagen panorámica o equirectangular, puedes convertirla en un cubemap usando herramientas como [ImageMagick](https://imagemagick.org/) o utilidades específicas de gráficos 3D. El proceso general es:

1. **Convertir la imagen panorámica a 6 caras** usando una herramienta de proyección (por ejemplo, [CubeMapGen](https://github.com/ptitSeb/CubeMapGen) o scripts de Python).
2. **Asegurarse de que cada cara sea cuadrada** y tenga la misma resolución.
3. **Empaquetar las 6 caras en un archivo DDS** usando una herramienta como [texconv](https://github.com/microsoft/DirectXTex/wiki/Texconv).

## 3. Construcción a partir de una imagen 3x2

Una imagen 3x2 contiene las 6 caras del cubemap dispuestas en una cuadrícula de 3 columnas y 2 filas. El orden habitual es:
### Extracción de las 6 caras con ImageMagick

Para extraer las 6 imágenes cuadradas de una imagen 3x2 llamada `skybox_day_night.png`, usa el siguiente comando:
- `-crop 3x2@` divide la imagen en 6 partes iguales (3 columnas x 2 filas).
- `+repage` elimina la información de página virtual.
- `skybox_ace_%d.png` guarda cada cara como un archivo numerado del 0 al 5.

**Nota:** El orden de las caras extraídas puede variar según la herramienta o el estándar que sigas. Asegúrate de renombrar o reorganizar los archivos según el orden requerido por tu motor gráfico.

### Ejemplo de correspondencia de caras

| Índice | Cara  |
|--------|-------|
| 0      | +X    |
| 1      | -X    |
| 2      | +Y    |
| 3      | -Y    |
| 4      | +Z    |
| 5      | -Z    |

## 4. Creación del DDS con texconv

Una vez tengas las 6 imágenes cuadradas, usa `texconv` para crear el DDS:
- `-cube` indica que se va a crear un cubemap.
- El orden de los archivos debe coincidir con el estándar de tu motor.

## 5. Resumen del proceso

1. **Extraer las 6 caras** de la imagen 3x2 (o convertir desde panorámica).
2. **Asegurarse de que todas las imágenes sean cuadradas y del mismo tamaño.**
3. **Usar texconv** para empaquetar las imágenes en un DDS cubemap.

## 6. Recursos útiles

- [ImageMagick](https://imagemagick.org/)
- [texconv (DirectXTex)](https://github.com/microsoft/DirectXTex/wiki/Texconv)
- [CubeMapGen](https://github.com/ptitSeb/CubeMapGen)
