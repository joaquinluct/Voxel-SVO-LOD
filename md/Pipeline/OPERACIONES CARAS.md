A parte del draw, ¿qué otras operaciones eran muy caras?


Las operaciones de cambio de estado de la GPU son muy caras. El Draw es la operación de dibujo principal, pero a menudo los cuellos de botella no están en el Draw en sí, sino en las operaciones que lo preceden y que fuerzan a la GPU a reconfigurar su pipeline de renderizado.

Operaciones Cara
Estas son las operaciones que causan los mayores gastos generales en el pipeline de renderizado:

Cambiar de Shader: Cargar un nuevo shader (VSSetShader, PSSetShader, etc.) es una operación costosa. La GPU necesita compilar y validar el código del shader, lo que puede llevar tiempo.

Cambiar de Render Target: Alterar el destino de renderizado (OMSetRenderTargets) es una operación muy costosa. La GPU tiene que descargar el render target actual y preparar la memoria para la nueva vista. Esta es la operación principal que define un nuevo pase de renderizado.

Cambiar el Input Layout: Cambiar la forma en que los vértices se vinculan al pipeline (IASetInputLayout) es una operación costosa, ya que el driver de la GPU tiene que revalidar cómo se procesa la geometría.

Cambiar de State Objects: Cambiar el estado de la GPU, como los estados de rasterización (RSSetState), mezcla (OMSetBlendState), o profundidad/stencil (OMSetDepthStencilState), es menos costoso que los cambios de shader o render target, pero sigue contribuyendo significativamente a los gastos generales, especialmente si ocurren con frecuencia.

Actualizar Búferes (UpdateSubresource): La actualización de búferes de constantes, sobre todo si se hace a menudo, es una operación de sincronización que fuerza a la GPU a detenerse y esperar los nuevos datos de la CPU. Un uso ineficiente puede crear un cuello de botella entre la CPU y la GPU.

La clave para una renderización eficiente es minimizar el número de estos cambios de estado. Puedes agrupar las llamadas de dibujo que comparten el mismo shader, render target, y estados, reduciendo así la cantidad de veces que la GPU necesita reconfigurarse. Esta técnica se llama agrupación por estados o state batching.
