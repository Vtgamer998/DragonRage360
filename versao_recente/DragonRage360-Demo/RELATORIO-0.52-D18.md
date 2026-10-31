# DragonRage360 0.52-D18 — vídeo visível e saída PCM no Xenia

## Evidência nova

O pixel lido **dentro** do XEX D18 continuou `FF000000`, mas uma captura externa da janela do Xenia mostrou a interface desenhada. O verificador `engine/tools/verify_xenia_capture.py` encontrou o bloco laranja `RGB(255,138,31)` no ponto (60,77) da captura. O deslocamento inclui a barra da janela; a captura exibe apenas a interface de diagnóstico. A leitura interna continua com FAIL e precisa de investigação de coerência/layout do framebuffer. Nenhum pixel ou quadro da luta foi observado.

Repeti o XEX com `--apu=sdl`. No ambiente padrão, `SDL_OpenAudioDevice()` falhou por falta de dispositivo host. Ao definir `SDL_AUDIODRIVER=disk`, o SDL abriu um dispositivo de gravação e gerou 22.663.168 bytes de PCM float, dos quais 1.287.418 amostras são não nulas, pico 0,226946. `engine/tools/verify_xenia_audio.py` checou alinhamento, finitude, faixa e o log. Isso comprova saída PCM do emulador para um arquivo, **não** som audível em alto-falante nem no Xbox real. O áudio bruto de conteúdo do jogo não está distribuído.

Uma auditoria sem execução (`engine/tools/audit_original_battle_load.py`) identificou os treze arquivos solicitados no início do carregamento original para o cenário 0 e os lutadores 0/1. Todos foram encontrados na ISO de teste, e os tamanhos dos arquivos de cenário e membros cabem nos buffers declarados do carregador. O carregador ainda precisa de conversão dos recursos no caminho `File_Request`, da fila de jobs e de um caminho de apresentação para a tela de carregamento. A auditoria não chama `Battle_Load`.

O pacote de cenário 0 tem 25 entradas; as não vazias são 0,1,2,3,4,5,6,12,15,18,19,20,21,22,23. A preparação atual trata a geometria e texturas de 0, MAPD de 1, navegação de 15, MEF de 18 e caminhos de 23; 2–6, 12 e 19–22 ainda pedem formatos/consumidores internos. O cenário 0 não é representativo dos recursos opcionais de todos os cenários.

## Estado da lista

| Requisito | Estado após D18 |
| --- | --- |
| Recursos internos do cenário | Incompletos; entradas restantes enumeradas acima |
| `Battle_Load`/`Battle_Init` | Não executados; a sessão separada do diagnóstico persiste |
| `Battle_Update`/`Battle_Main` | Não executados; `NOT_READY=2` |
| GPU da interface | Captura externa confirmou imagem; leitura interna de pixel segue FAIL |
| Quadro da luta, skinning, HUD, lábios, auras e efeitos | Não demonstrados |
| Áudio | PCM float capturado via dispositivo SDL de arquivo; áudio audível não comprovado |
| Menus completos, saves e online | Não implementados como jogo completo; a interface e o arquivo de opções são demonstrativos |
| Xbox 360 real | Sem novo teste; último comunicado continua 0.27 |

## Bloqueio do boot original

`Battle_Load` chama `BtlObj_Init(1)`, inicializa script, enfileira `BtlLoad_StepInitial`, roda `Load_RunBlocking` e cria os personagens. O job carrega quatro bancos de som, pede os três arquivos por personagem, o cenário e dados de membros, depois depende de `File_UpdateRequests`, `BtlRes` e conversões. `Battle_Init` assume que esse estado já foi produzido, inicializa efeitos/cenário/gerentes/câmera e chama `Battle_Restart`. O diagnóstico atual cria objetos e gerentes antes disso; chamá-lo por cima duplicaria o registro. Uma implementação correta exige um boot **isolado e fresco** e a adaptação verificada de cada recurso e da apresentação da tela de carregamento. Nenhuma função original foi chamada apenas para alterar o marcador.

## Reprodução

Para auditar recursos, execute `python3 engine/tools/audit_original_battle_load.py /caminho/para/sua.iso`. Para conferir uma captura externa do Xenia, execute `python3 engine/tools/verify_xenia_capture.py screen.png default.log`. Para capturar PCM sem placa de som host, execute Xenia com `--apu=sdl`, `SDL_AUDIODRIVER=disk` e `SDL_DISKAUDIOFILE=/caminho/audio.raw`, e depois `python3 engine/tools/verify_xenia_audio.py audio.raw xenia.log`. O verificador de captura de vídeo requer Pillow.

No Xbox 360 RGH, coloque `default.xex`, sua ISO USA e `startup_test.txt` com `typed-data` na pasta do diagnóstico e execute pela Aurora. Envie o log e a observação da tela/som. Não há ISO ou arquivos de jogo nos pacotes.
