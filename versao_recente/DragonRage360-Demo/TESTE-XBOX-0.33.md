# Diagnóstico 0.33 no Xbox 360

Não houve teste desta versão no Xbox ou emulador. O default.xex é somente o
launcher de diagnóstico. Game_Main/Battle_Main não estão ligados.

As APIs do decoder PSS estão retidas no binário, mas ainda não há ação de
menu para chamá-las. Portanto, iniciar este XEX não testa a reprodução de
vídeo nem o desempenho do decoder. Os resultados de 7.010 quadros são de host.

Os testes existentes de ISO/modelo/captura continuam disponíveis conforme os
guias anteriores. Use a ISO USA do próprio usuário na pasta isos/, guarde o
default.log após executar e registre o modelo do console. Nenhum resultado
visual ou de áudio em hardware foi presumido neste pacote.

O trabalho seguinte de vídeo é adaptar o player, clock/ADXT e upload das
imagens ao backend nativo antes de oferecer uma prova visual no launcher.
Consulte RELATORIO-0.33.md para os bloqueios do vínculo e da luta.
