# Testar o diagnóstico 0.27 no Xbox 360 RGH

Este arquivo não inicia uma luta. O objetivo é verificar o primeiro caminho
gráfico nativo no console; ainda não há confirmação de execução em hardware.

1. Extraia a pasta DragonRage360-Demo em uma pasta separada no HDD/USB.
2. Abra seu default.xex pelo Aurora. Não é necessário colocar uma ISO para
   este diagnóstico sintético.
3. Use LB/RB até a aba Controle. Pressione X para abrir o diagnóstico gráfico.
4. Dentro do diagnóstico gráfico, pressione Y para abrir a captura original GS.
5. Observe a imagem ou o erro exibido. B volta. Copie o default.log da pasta do
   executável; se não estiver lá, procure na raiz do HDD/USB (o SDK tem fallback).

O log deve identificar a versão 0.27 e registrar os resultados das sequências
mapped DMA NEXT/return e native Port_Gs VIF/MPG/UNPACK/MSCAL/XGKICK, GIF and DBuff
clear probe. Um rc=0 comprova apenas que a rotina não detectou erro; não valida
os pixels. Envie o log e descreva o que apareceu na tela para continuar a
validação do console. O modelo da ISO é outro diagnóstico, em pose de referência.
