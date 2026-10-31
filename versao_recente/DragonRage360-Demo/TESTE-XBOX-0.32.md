# Diagnóstico 0.32: limites de teste

Este default.xex continua sendo o launcher de diagnóstico. Ele não chama
Game_Main/Battle_Main e não contém os 72 objetos de menus ou seu catálogo.
As APIs ADXT e do carregador genérico foram retidas para verificação estática;
não há botão novo de áudio e o launcher não começa a tocar música automaticamente.

As ações gráficas e de ISO da 0.31 continuam disponíveis. Registre default.log
e a imagem se testar em Xbox real. Nenhuma versão desta entrega foi executada
em console ou emulador.

Para integrar um futuro título do motor, regenere o catálogo com link_battle.py,
mantenha a ISO USA montada durante o jogo e preserve os erros de preparação.
O catálogo é específico dos objetos/layout dessa compilação. Não use endereços
do mapa de um vínculo incompleto como lista externa de destinos.

O boot modificado chama XbOverlay_PrepareBuild e só restaura os menus quando
há catálogo nativo válido e identidade de disco correta. Um erro é bloqueio
de boot, não deve ser transformado em sucesso. Ainda faltam 207 referências e
o restante da inicialização nativa, incluindo dados principais e integrações
de combate. Não force a chamada ao jogo neste launcher.

Os testes de áudio usam o transporte xsdk simulado. Ouvir música/vozes,
medir underruns/latência e jogar partidas completas ainda são testes pendentes.
