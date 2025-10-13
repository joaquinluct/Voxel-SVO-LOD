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


---

Reglas de uso:
- El asistente añadirá entradas nuevas automáticamente cuando realice commits de refactor importantes.
- Para commits manuales, añade una entrada con el mismo formato para mantener coherencia.

