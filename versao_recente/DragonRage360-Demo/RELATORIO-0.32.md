# 0.32 — overlay residente nativo e ADXT

O pedido completo ainda não está atendido. As 13 entradas ADXT e a consulta
de leitura usada pelo player de vídeo receberam implementação nativa. O
vínculo passa de 221 para 207 nomes pendentes. O caminho que copiava o overlay
MIPS sobre uma função PowerPC foi substituído por restauração de dados nativos.
Battle_Main e Game_Main continuam fora do default.xex: não há luta executável.

## Vínculo e contagem

O vínculo estrito força Battle_Main e Game_Main. Compilaram 387/387 unidades
PowerPC, sem conflito de ABI. Os 294 objetos submetidos à passagem de aritmética
não apresentaram instruções float nativas na inspeção. As novas unidades são
os dois componentes de plataforma e o catálogo nativo gerado.

Saíram da lista: ADXT_Init, ADXT_Create, ADXT_SetReloadSct, ADXT_SetSvrFreq,
ADXT_StartAfs, ADXT_StartFname, ADXT_Stop, ADXT_Pause, ADXT_SetOutVol,
ADXT_GetOutVol, ADXT_SetOutPan, ADXT_SetOutputMono, ADXT_GetStat e
func_00277A70. Essa última consulta o estado real de prefetch síncrono do player.

Restam 207 nomes: 104 com referência em código e 103 somente em dados na
auditoria dos objetos, incluindo seções depois descartadas por GC. Não foram
removidas tabelas nem gerados retornos vazios para encerrar o vínculo. Seguem
pendentes as nove sceMpeg*, RPC/IOP/SPU2, resets GS, interfaces, rede e outros
rótulos. A lista exata está em engine/reports/0.32/battle-integration.json.

## O novo Overlay_Load

O perfil PORT_BIGENDIAN usa engine/combat/main.c. Overlay_Load chama
XbOverlay_Load; não usa sceRead com MainMenu_StartBallLoops como destino.
Game_Main exige XbOverlay_PrepareBuild antes do boot e verifica o erro de cada
carregamento antes de chamar Progress_Main. Catálogo ausente, ISO incorreta
ou remonte interrompem esse caminho. Não há laço infinito tentando sobrescrever
o código nativo.

Os menus já são funções C compiladas para PowerPC. Seus inicializadores e
ponteiros são produzidos pelo compilador e pelo linker nativos; eles não são
instruções nem ponteiros extraídos do DBZP.BIN. O novo carregador captura esse
estado inicial e restaura somente os dados que pertencem ao overlay original.
Código e constantes imutáveis ficam residentes no executável.

O catálogo é gerado automaticamente dos 72 objetos de menus compilados:

| Catálogo desta compilação | Cobertura |
| --- | ---: |
| Regiões graváveis | 119 |
| Regiões restauradas no reload | 75 |
| Regiões persistentes das extensões port_* | 44 |
| Bytes graváveis catalogados | 83.288 |
| Relocações de ponteiros nos dados graváveis | 46 |
| Relocações alocadas inventariadas, incluindo código/constantes | 34.986 |

build_overlay_catalog.py gera um fragmento de layout a partir do xsdk.ld,
os limites simbólicos das regiões e o catálogo C. Dados COMMON são associados
ao símbolo nativo final, pois o linker pode coalescer definições de objetos
diferentes. As extensões port_language, port_screen e port_tbo são inventariadas
mas preservadas, evitando resetar estado que não existia no overlay PS2.

As relocações nativas continuam sob responsabilidade do linker. O carregador
confere os 46 valores de ponteiro antes de capturá-los; não tenta relocar um
endereço MIPS com uma soma arbitrária. Os destinos locais necessários receberam aliases
globais apenas para essas verificações: os bytes de todas as seções alocadas
e os atributos de ABI são comparados antes/depois do objcopy.

audit_overlay_catalog.py verifica cobertura de todas as seções graváveis,
limites/tamanhos no mapa e hashes dos objetos. Também produz um inventário de
seções e relocações de código e constantes que o linker trata normalmente.
Esse mapa pertence a um vínculo que ainda falhou: ele não é um executável nem
prova de funcionamento do menu.

XbOverlay_Prepare valida limites de DATA, sobreposições, tamanhos, políticas
e ponteiros. Faz cópias privadas dos descritores e do estado inicial antes de
chamar a leitura externa, impedindo que alterações dos descritores durante a
leitura redirecionem uma restauração. Reentrada é recusada. O custo do snapshot
é limitado a 4 MiB; o desta compilação usa cerca de 83 KiB.

DBZP.BIN é lido somente para identificar a versão USA: 563.996 bytes e
FNV-1a 0x91a321108fc31289. Esse checksum não é autenticação criptográfica.
Nenhum byte dele é copiado para código ou dados do programa nativo. Um remonte
invalida o carregamento; o novo fluxo não executa código MIPS.

Este é o catálogo completo do estado **nativo compilado dos menus** desta
compilação. Não é um schema de todos os recursos da ISO nem o manifesto tipado
completo das tabelas do executável principal SLUS_216.78. Essa outra parte do
boot continua pendente. O catálogo deve ser regenerado a cada compilação;
nunca reutilize endereços de um mapa antigo como destinos.

## ADXT nativo

xb_adxt.c implementa os 13 pontos de entrada usados pelo jogo. Abre o cliente
de áudio real do xsdk e registra um serviço no vblank. Cria até 16 players,
lê arquivos AFS/soltos pela plataforma ISO, valida o cabeçalho e os limites,
decodifica ADPCM e mistura os players no frame do SDK: 256 amostras a 48 kHz,
float planar, canais frontais esquerdo/direito e os outros quatro zerados.

O perfil suporta ADX tipo 3/version 4, blocos de 18 bytes, mono/estéreo e
16/24/48 kHz. Valida total de frames, assinatura, criptografia e pontos de loop.
O histórico do preditor é salvo/restaurado no início do loop, inclusive quando
o fim não coincide com um bloco. Conversão para 48 kHz usa sample-hold;
não é um resampler de alta qualidade como o SDL.

Há volume em décimos de dB, pan por canal, mono, pausa e parada. O ganho é
calculado quando o volume muda, não por frame de áudio. Arquivos são
prefetched integralmente: limite de 32 MiB por arquivo e 64 MiB somados nos
players. ReloadSct determina o tamanho das leituras e SvrFreq limita a quota
de serviço da fila. Não há thread concorrendo com o leitor ISO.

GetStat usa duração em vblanks de 59,94 Hz, independente da disponibilidade
de slots na fila. O avanço do PCM é separado desse relógio. Isso não integra
os players à captura de estado do rollback: rollback do jogo inteiro continua
sem validação. Tampouco implementa os bancos VAG/efeitos SPU2 usados por RPC.

A fila do SDK pode manter cerca de 43 ms de áudio pronto. Pausa/parada afetam
os próximos frames; a cauda já enfileirada pode ser ouvida. Um frame de jogo
demorado pode provocar underrun, pois o serviço está no vblank da thread
principal. Não foram medidos latência, custo de CPU ou som em hardware.

## Validação

Os testes de overlay restauraram dados nativos mistos e ponteiros 100 vezes,
confirmaram código intacto e recusaram destinos em código, regiões sobrepostas,
relocações erradas, IO/checksum inválidos, reentrada e alteração dos descritores.
Uma fixture ELF PowerPC separada verificou, com o linker real, ponteiros para
funções, strings e COMMON. Ela não substitui nenhuma função no vínculo do jogo.

Outro teste usa o wrapper de console real, xb_disc.c/dr_disc.c e a ISO real,
com somente o transporte NT substituído por stdio. Conferiu a identidade do
DBZP, restauração de ponteiros e recusa de unmount/remount. O catálogo de dados
desse teste é uma fixture de host; o catálogo completo do motor foi auditado
separadamente e ainda não foi executado em PowerPC.

ADX reais testados: AFS2 índices 0 (24 kHz mono), 32763 (16 kHz mono) e
64974 (24 kHz estéreo). Centenas de frames, com amostras não nulas, conferiram
com as próprias rotinas decode/adx_coefs do upstream compiladas separadamente.
48 kHz, pan/mono/volume, pausa, loops de 64 e 63 amostras, duração por ticks,
fila cheia, limites de cache e falhas de dispositivo tiveram fixtures próprias.
O transporte xsdk de áudio e XbFile_* é simulado nesse teste; não houve saída
de som no console. Testes usaram ASan/UBSan com detect_leaks=0.

O default.xex 0.32 contém as implementações ADXT e do carregador genérico,
retidas explicitamente pelo Makefile para verificação estática. O launcher
não inicializa áudio automaticamente e não contém o catálogo completo dos
menus nem Game_Main/Battle_Main. Ele permanece um diagnóstico. Sua validação
estática passou; Xbox/emulador, pixels e luta não foram testados.

## Reprodução

Upstream 3ca419c7966b11438e2dc3c9a7e1914f102ed545; xsdk
57b02f1e60667e37d22c3aa613d23d2486514ff8; powerpc-xsdk-elf GCC 15.3.0.

```sh
python3 engine/tools/test_adxt.py UPSTREAM XSDK ISO OUT --sanitize
python3 engine/tools/test_overlay.py CC XSDK DBZP.BIN OUT --sanitize
python3 engine/tools/test_overlay_disc.py XSDK ISO OUT --sanitize
python3 engine/tools/link_battle.py UPSTREAM CC XSDK OUT --float-mode native-ps2 --platform --root Game_Main
python3 engine/tools/audit_overlay_catalog.py OUT
python3 engine/tools/audit_engine_refs.py OUT AUDIT_OUT
```

Evidências e catálogo simbólico desta compilação: engine/reports/0.32.
O patch textual desde 0.31 está em extras/port-0.32.patch. Nenhum arquivo ADX,
ISO, AFS ou executável PS2 é distribuído neste pacote.
