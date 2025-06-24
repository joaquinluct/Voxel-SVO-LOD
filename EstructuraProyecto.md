/YourProjectRoot/
├── bin/                       # Directorio de salida para ejecutables y assets finales del juego
│   ├── Debug/
│   └── Release/
│       └── game.exe
│       └── engine_config.yaml
│       └── assets_manifest.yaml
│       └── Assets/             # <--- ¡Aquí van los assets de DATOS desplegados!
│           ├── Textures/       # .dds, .png, .jpg
│           ├── Meshes/         # .obj, .fbx, .stl (o formatos binarios de motor)
│           ├── Audio/          # .wav, .mp3
│           ├── Shaders/        # Archivos de shader compilados (.cso) o HLSL/GLSL
│           ├── Levels/         # Archivos de configuración de nivel
│           └── Configs/        # engine_config.yaml, assets_manifest.yaml
│           └── Fonts/          # Archivos de fuente
│           └── ...
│
├── lib/                       # Librerías de terceros precompiladas (.lib, .dll)
│   ├── Debug/
│   └── Release/
│       └── DirectXTK.lib
│       └── yaml-cpp.lib
│       └── ...
│
├── include/                   # Cabeceras de librerías de terceros
│   ├── DirectX/
│   ├── yaml-cpp/
│   └── ...
│
├── src/                       # Código fuente de tu motor y juego
│   ├── Engine/                 # Componentes del núcleo del motor
│   │   ├── Application/        # Bucle principal, gestión de ventana
│   │   ├── Managers/           # Clases Manager (DeviceManager, ShaderManager, ServiceLocator)
│   │   │   ├── AssetManager/   # <--- AssetManager.h/.cpp, AssetLoadContext.h
│   │   │   ├── DeviceManager/
│   │   │   └── ...
│   │   ├── Rendering/          # Pipeline de renderizado, envoltorios de D3D11
│   │   │   ├── Shaders/        # Clases para gestionar shaders (e.g., BasicShader.h/.cpp)
│   │   │   ├── Buffers/
│   │   │   └── ...
│   │   ├── Core/               # Utilidades, matemáticas, tipos comunes
│   │   ├── DataStructures/
│   │   ├── Components/         # Clases de componentes (Transform, Camera, Renderer)
│   │   ├── Assets/             # <--- ¡Aquí van las clases C++ de tus Assets!
│   │   │   ├── IAsset.h        # Interfaz base
│   │   │   ├── Texture.h
│   │   │   ├── Texture.cpp
│   │   │   ├── Mesh.h
│   │   │   ├── Mesh.cpp
│   │   │   ├── Skybox.h        # Clase Skybox que usaremos
│   │   │   ├── Skybox.cpp
│   │   │   └── ...
│   │   │
│   ├── Game/                   # Código específico del juego (si lo separas del motor)
│   │   ├── Entities/
│   │   ├── Scenes/
│   │   └── UI/                 # Clases de la interfaz de usuario
│   │       ├── UIElement.h
│   │       ├── UIElement.cpp
│   │       ├── UIRectangle.h
│   │       ├── UIRectangle.cpp
│   │       ├── UIButton.h
│   │       ├── UIButton.cpp
│   │       ├── UIText.h
│   │       ├── UIText.cpp
│   │       └── ...
│   │
│   └── Main/                   # Punto de entrada principal (Main.cpp, MainController.h/cpp)
│       └── MainController.h
│       └── MainController.cpp
│       └── main.cpp
│
├── Assets/                    # <--- ¡Aquí van tus assets de DATOS de DESARROLLO (la fuente)!
│   ├── Textures/
│   │   └── grass_diffuse.png
│   │   └── sky_day_cubemap.dds
│   ├── Meshes/
│   │   └── simple_cube.obj
│   ├── Audio/
│   ├── Shaders/               # Archivos de código fuente de shaders (.hlsl, .glsl)
│   ├── Configs/               # engine_config.yaml, assets_manifest.yaml
│   │   └── engine_config.yaml
│   │   └── assets_manifest.yaml
│   └── Fonts/
│   └── ...
│
├── solutions/                 # Archivos de solución de Visual Studio (.sln)
│   └── MyEngine.sln
│
└── docs/                      # Documentación del proyecto