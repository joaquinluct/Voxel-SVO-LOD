# Resumen de los Mapas de Texturas PBR para el Suelo
Los mapas que tienes son ideales para crear un material de suelo rocoso y arenoso con PBR:

- **coast_sand_rocks_02_diff_4k.jpg**  
    *Albedo/Diffuse*: Proporciona el color base del suelo.

- **coast_sand_rocks_02_nor_dx_4k.jpg**  
    *Normal Map para DirectX*: Añade detalles de rocas y arena sin necesidad de geometría adicional. Úsalo si trabajas en DirectX.

- **coast_sand_rocks_02_rough_4k.jpg**  
    *Roughness Map*: Controla la rugosidad de la superficie, afectando el reflejo de la luz.

- **coast_sand_rocks_02_ao_4k.jpg**  
    *Ambient Occlusion Map*: Simula sombras en las grietas y zonas de contacto del suelo.

- **coast_sand_rocks_02_disp_4k.png**  
    *Displacement/Height Map*: Permite desplazamiento geométrico real si usas teselación para mayor detalle. Es opcional y avanzado.

- **coast_sand_rocks_02_arm_4k.jpg**  
    *ARM Map (AO, Roughness, Metallic)*: Combina AO y Roughness en un solo archivo, optimizando el uso de texturas y muestreos. El canal Metallic permanece bajo para materiales no metálicos como la roca. Recomendado si la memoria de textura es limitada.
