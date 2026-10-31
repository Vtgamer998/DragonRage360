# Diagnóstico 0.30 — roteiro de validação

O default.xex deste projeto é um diagnóstico, não executa Battle_Main.
Não há confirmação de execução desta versão em Xbox ou emulador.

No console, abra o diagnóstico gráfico de captura original GS e guarde
`default.log` após sair. O log deve identificar a versão 0.30 e mostrar
`native Port_Gs ... DBuff clear probe rc=0`. Esse probe agora também espera
o fence da GPU e lê um pixel RGB do target solicitado. O marcador de efeito
atravessa o produtor de pacotes real; o desenho continua sintético.

Um rc diferente de zero indica erro; não é autorização para ignorar a falha.
A confirmação de rc=0 ainda não comprova pixels corretos. Registre a imagem
exibida, a revisão do console, o dashboard e o resultado de abrir, desenhar,
sair e repetir. DATE/FBA/stencil exigem comparação de imagens de referência.

Os testes de host em engine/reports/0.30 substituem as chamadas GPU por mocks.
Eles verificam contratos e memória, não execução de shaders nem uma luta.
