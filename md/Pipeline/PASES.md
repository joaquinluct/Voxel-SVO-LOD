Qué operaciones hacen que RenderDocs detecte un nuevo pase?


El software de captura y análisis de gráficos como RenderDoc detecta los pases de renderizado basándose en los cambios de estado del pipeline que la aplicación le envía a la GPU. Un "pase" para RenderDoc es, en esencia, una serie de llamadas de dibujo que comparten un mismo destino de renderizado y una configuración de estado similar.

RenderDoc registra un nuevo pase cuando detecta una o más de las siguientes operaciones:

Cambios de Render Target (OMSetRenderTargets): Esta es la operación más importante. Cuando cambias el render target o el depth stencil view al que la GPU está escribiendo, RenderDoc interpreta que estás comenzando un nuevo pase de renderizado. Por ejemplo, al cambiar de una vista de shadow map a la vista de la escena principal.

Cambios de Viewport: Si cambias el área de la pantalla (RSSetViewports) donde se está renderizando, RenderDoc puede marcarlo como el inicio de un nuevo pase, ya que el destino de renderizado ha cambiado, incluso si la vista de renderizado subyacente es la misma.

Limpiar los Buffers (Clear calls): Las operaciones de limpieza como ClearRenderTargetView o ClearDepthStencilView a menudo preceden a un pase de dibujo, y RenderDoc las usa como un marcador para identificar el comienzo de un nuevo conjunto de comandos.

Lo que RenderDoc No usa para detectar pases
Cambios de shaders: Cambiar de shader (VSSetShader, PSSetShader, etc.) no necesariamente crea un nuevo pase en RenderDoc. El cambio de shader se considera un simple cambio de estado dentro del pase actual.

Cambios de recursos de entrada: Cambiar los buffers de vértices, las texturas (PSSetShaderResources) o los samplers (PSSetSamplers) no inician un nuevo pase. Estos son recursos que el shader utiliza, pero no cambian el destino de renderizado.

En tu arquitectura, un nuevo pase sería detectado por las operaciones SetRenderTargetOperation o SetViewportOperation, ya que cambian el estado fundamental del pipeline de la GPU para el siguiente conjunto de llamadas de dibujo.
