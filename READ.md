Vamos a ver si entiendo lo que debería pasar tal y como lo hemos planteado (SVO+LOD) y con la creación de un suelo virtual, donde si Y < 0 => LLENO. Y si Y <= 0 => VACIO.

Hemos creado un Octree por cada bloque de 1024x1024x1024 unidades.

La cámara la situamos en (2.0f, 2.0f, 2.0f) y mirando a SetRotation(XMConvertToRadians(45.0f), 0.0f, 0.0f); hacia abajo con un ángulo de 45º.

Aún no hemos aplicado Frustrum Culling + otras técnicas.

Todo esto se hace en el Update de World, engachado al principal, que es de MainGame y este a su vez en MainWindow en el bucle principal:
MSG msg = { 0 };
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            LARGE_INTEGER currentTime;
            QueryPerformanceCounter(&currentTime);
            float deltaTime = (float)(currentTime.QuadPart - previousTime.QuadPart) / frequency.QuadPart;
            previousTime = currentTime;

            g_controller->Update(deltaTime); // Llama a Update
            g_controller->Render();
            mouse.Render();   // Actualizar y usar la entrada del rat�n
            keyboard.Render(); // Actualizar y usar la entrada del teclado
            //InvalidateRect(g_hWnd, NULL, TRUE); // Fuerza a repintar toda la ventana
            //UpdateWindow(g_hWnd); // Sincroniza el repintado
        }
    }

Una vez hecho el Update (cosa que ya hablaremos, pero este Update es como el Render realmente. Igual sería mejor hacerlo con Hilos) se hace el Render.

Llega al Render de World que, para los nodos visibles, llama a GenerateMesh de MarchingCubes y este crea una geometría por cada nodo (array de vértices + array de índices). También genera geometría de transición entre nodos para resolver problemas de cambio de nivel de detalle (LOD) a través de GenerateTransitionMesh.
Y yo de manera rápida (que esto puede ser una de las cáusas, aunque no creo) he reaprovechado la VoxelMesh antigua para, de manera temporal, en el Render de la clase World, pasarle los vértices e índices, para que esta pinte la geometría (esta clase parece hacer su cometido ya que pinta la geometria, aunque ahora, tal y como está el código, sale fatal)

En este escenario se debería renderizar por cada Nodo (entiéndase por Nodo, cada Octree de cada bloque de 1024x1024x1024 unidades, o subdivisiones de un Octree hasta sus hojas, que en este caso hay tantos nodos por área como hojas visibles tenga ese nodo subdivididas tal y como diga el nivel de detalle por distancia) com digo, se debería renderizar por cada Nodo una sola cara horizontal al mundo, creando un vóxel grande cuando la distancia de la cámara al boundingBox del nodo (DistanceToAABB(cameraPosition, nodeMin, nodeMax);) es mayor a m_lodSettings.nearDistance = 128.0f;  y deja de hacerlos visibles a distancias mayores a  m_lodSettings.midDistance = 256.0f;

En distancias menores a m_lodSettings.nearDistance debería iterar por los nodos de cada área que sean hojas y visibles, pintando un VoxelMesh por cada hoja visible, tras ser procesado por MarchingCubes. 

El conjunto de los triángulos generados para cada VoxelMesh es un conjunto de vértices resultado de 2 cosas:
1) La geometría generada por MarchingCubes original
2) La geometria de transición

Visto todo esto, ten cuento lo que veo
1) lo primero que quiero es que me digas (y, por favor, accede al código para verifiicarlo) si en algo me estoy equivocando de la exposición.
2) Intentar averiguar 