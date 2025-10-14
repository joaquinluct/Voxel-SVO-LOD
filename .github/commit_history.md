# Commit history — DirectX-Voxelado

Este fichero registra los commits importantes realizados por el asistente para facilitar rollback y documentación.

Formato: una entrada por commit con:
- hash
- mensaje/branch
- fecha (si disponible)
- breve descripción

Listado (cronológico, más reciente al final):

1. commit 8efdc70679e3aa163a803568bfd5ebe166b4cad1
   - mensaje: compilación correcta (estado inicial previo a refactor)
   - fecha: 2025-10-13
   - descripción: Estado previo a la migración a modelo AAA; build limpia.

2. commit 2c0596316cc00b0746be3f5c0eb85937b6fbc53f
   - mensaje: [wip] TERRAIN 4
   - fecha: 2025-10-13
   - descripción: Trabajo en la subsistema de terreno; refactorings parciales en Terrain engines y chunks.

3. assistant change (no commit hash)
   - mensaje: update instructions & commit mechanics
   - fecha: 2025-10-13
   - descripción: Se añadió la mecánica obligatoria de commits pequeños y compilaciones limpias al principio de `.github/copilot-instructions.md` y se añadió la sección "Por dónde vamos" indicando el siguiente paso (implementación de `RenderLoop`).

4. commit 43b8562
   - mensaje: refactor/threading-step-3: execute render packets in render thread
   - fecha: 2025-10-13
   - descripción: Implementación de `Engine::RenderLoop`, `SubmitRenderCommands` y ejecución de `RenderCommandPacket` en el render thread. Compilación verificada localmente (build limpia). Push a rama `terrain`.

5. commit f8f275d
   - mensaje: refactor/threading-step-4: migrate UpdateManager and SceneManager to main thread
   - fecha: 2025-10-13
   - descripción: Migración de `UpdateManager` y `SceneManager` para ejecutar su lógica en el hilo principal. `RunLoop()` se mantiene como no-op para compatibilidad. Compilación verificada y push a rama `terrain`.

6. commit 17dfbd6
   - mensaje: chore: add post-commit hook to auto-update commit_history
   - fecha: 2025-10-13
   - descripción: Añadido `.githooks/post-commit` que automatiza la adición de entradas en `.github/commit_history.md` tras cada commit. Instrucciones: copiar a `.git/hooks/post-commit` y marcar ejecutable localmente.

7. commit ef8b34a
   - mensaje: refactor/threading-step-5: introduce UpdateSystem and switch flag in Engine::Init
   - fecha: 2025-10-13
   - descripción: Introducido `IUpdateSystem` y `UpdateSystem` (delegando en `UpdateManager` durante la migración). Engine ahora usa `UpdateSystem` en `UpdateGameLogic`. Compilación verificada y commit creado.

8. commit 317d264
   - mensaje: refactor/ui-step-1: start UI subsystem and pause terrain integration
   - fecha: 2025-10-13
   - descripción: Se inició la base del subsistema UI (API en `UIManager` para generar comandos, integración con SceneManager). La integración del terreno queda en stand-by hasta validar el render pipeline con la UI.

9. commit 4fe939d
   - mensaje: refactor/ui-step-2: add DrawTextCommand and panel-label wiring
   - fecha: 2025-10-13
   - descripción: Añadido `DrawTextCommand` placeholder y wiring en `UIManager` para crear panels y asociar labels; `FillCommandBuffer` ahora emite comandos de texto/quad para el pipeline UI.

10. commit 6e91c54
   - mensaje: refactor/ui-step-3: implement UIRenderer VB/draw plumbing and create hello-world panel
   - fecha: 2025-10-14
   - descripción: Añadido `UIRenderer` (creación de input layout, VB dinámico y binding de shaders) y registro de un panel "Hola Mundo" en `SceneManager::PostInit`. Compilación verificada.

11. commit 053be89
   - mensaje: feat(ui): adapt UIText UVs to UI atlas cell size (256x256 atlas POC)
   - fecha: 2025-10-14
   - descripción: UIText UVs adaptadas para mapear correctamente a un atlas de 256x256 con celdas 16x16; mejora del mapping de caracteres.

12. commit 1aa9b4b
   - mensaje: chore(ui): prefer external atlas if present (log fallback)
   - fecha: 2025-10-14
   - descripción: UIRenderer detecta la presencia de `Assets/Textures/UI/atlas_ui.png` y registra en log la recomendación de cargarlo desde AssetManager; se mantiene fallback seguro.

13. commit c736975
   - mensaje: refactor/assets: implement AssetManager::LoadTexture to return SRV from TextureAsset via AssetLocator
   - fecha: 2025-10-14
   - descripción: `AssetManager::LoadTexture` devuelve `ID3D11ShaderResourceView*` obtenido desde `TextureAsset` (vía `AssetLocator`), asegurando Init() del asset.

14. commit 5fa853b
   - mensaje: feat(ui): register UIAtlas in YAML and wire AssetManager loading; prefer UIAtlas SRV in UIRenderer
   - fecha: 2025-10-14
   - descripción: Registrado `UIAtlas` en `Resources/Config/Assets` y `UIRenderer` intenta cargar `UIAtlas` vía `AssetManager::LoadTexture("UIAtlas")` para reemplazar el atlas POC.

15. commit fd03cd0
   - mensaje: chore(assets): register UIAtlas in assets index and texture maps
   - fecha: 2025-10-14
   - descripción: Añadida la entrada `UIAtlas` al `MainIndex` de assets y el `UIAtlasMap` con la ruta `Assets/Textures/UI/atlas_ui.png`.

16. commit 62e6406
   - mensaje: feat(ui): support external atlas metadata for UIText (atlas_ui.meta)
   - fecha: 2025-10-14
   - descripción: UIText ahora lee `Assets/Textures/UI/atlas_ui.meta` (textureSize, cellSize) para configurar columnas/filas del atlas y ajustar UVs.

17. commit 1ee43ab
   - mensaje: feat(ui): per-character rendering groundwork - atlas metrics parsing and glyph metrics support
   - fecha: 2025-10-14
   - descripción: UIRenderer parsea `atlas_ui.json` (heurísticamente) y almacena métricas de glyphs; estructura `GlyphMetric` añadida y preparada para render per-character.

18. commit 8d2b12f
   - mensaje: refactor/ui-step-6b: implement per-character VB upload and draw using UI atlas metrics
   - fecha: 2025-10-14
   - descripción: Implementado llenado dinámico de VB por carácter en `UIRenderer` (Map/Write/Unmap) y draw de quads usando métricas del atlas.

19. commit e873ac5
   - mensaje: fix(ui): transform text pixel positions to centered ortho coords for correct placement
   - fecha: 2025-10-14
   - descripción: Ajuste de coordenadas de texto (pixels top-left -> coordenadas centradas) para que los quads de texto se dibujen correctamente con la matriz ortográfica.


---

Reglas de uso:
- El asistente añadirá entradas nuevas automáticamente cuando realice commits de refactor importantes.
- Para commits manuales, añade una entrada con el mismo formato para mantener coherencia.

