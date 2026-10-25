# 0.33 — decodificação PSS nativa; relatório de integração

O novo componente lê e decodifica os vídeos reais da ISO. Foram decodificados
7.010 quadros em host, usando o leitor ISO do projeto. A luta continua sem
executável: Game_Main e Battle_Main não estão no default.xex.

## Resultado desta continuação

xb_mpeg2.c adapta o decodificador inteiro de port/src/plat_mpeg2.c do upstream
fixado. xb_movie_source.c liga a entrada ao XbDisc/ISO montado. Não há fopen
nem caminhos do PC no componente de console. Não foram copiadas as funções
vazias do adaptador sceMpeg do PC.

O perfil aceito é o dos vídeos USA: MPEG-2 PS/PES, quadros I progressivos,
512 × 448, cor 4:2:0, saída RGBA linear com alpha 255. Quadros P/B, outras
dimensões e campos/field DCT são recusados. As tabelas VLC, IDCT inteiro,
quantização e conversão BT.601 vêm do decoder do upstream; os deslocamentos
de valores negativos na IDCT foram trocados por multiplicações para evitar
comportamento indefinido em C.

O leitor usa cache de 8 KiB, pacote PES limitado a 65.535 bytes e lookahead
ES limitado a 2 MiB. Não carrega o vídeo completo na memória do Xbox.
Cada decoder mantém também cerca de 336 KiB para os planos; a tabela VLC
compartilhada ocupa 384 KiB e o buffer RGBA do chamador usa 896 KiB.
Falhas de alocação, IO, cabeçalhos e capacidade da saída são verificadas.
A saída fica intacta em caso de erro; erros de decodificação são persistentes.
A interface é de uma única thread, sem captura para rollback.

XbMpeg_OpenReader recebe um descritor tipado com tamanho e callback de leitura
exata; copia o descritor e assume seu fechamento apenas quando a abertura
funciona. XbMpeg_Open resolve o arquivo na ISO e verifica a geração da montagem
antes de cada quadro e cada leitura de cache. Unmount/remount invalidam o
player mesmo se ainda houver bytes em cache. XbMpeg_Close libera seus buffers.

## Validação com a ISO real

| Vídeo | Tamanho PSS | Quadros decodificados até EOF | Comparação RGB com FFmpeg |
| --- | ---: | ---: | --- |
| Abertura, ZS3USOP.PSS | 120.569.860 bytes | 2.810 | Primeiros 32: erro médio 1,084; máximo 5 |
| Encerramento, ZS3USED.PSS | 180.191.236 bytes | 4.200 | Primeiros 32: erro médio 1,091; máximo 5 |

O erro é medido em níveis RGB de 0 a 255, com a mesma conversão de faixa e
amostragem de chroma por vizinho no FFmpeg. Não se trata de igualdade pixel a
pixel nem de comparação dos 7.010 quadros: a referência de imagem cobre os
primeiros 32 de cada vídeo. Todos os quadros dos dois vídeos foram decodificados
sem erro e o fim dos dois streams foi conferido.

Os testes executam xb_disc.c, dr_disc.c, o adaptador de fonte e o decoder reais.
Somente o transporte de arquivos NT do console é substituído por stdio.
ASan/UBSan passaram com halt_on_error; leak detection desabilitado no ambiente.
Também passaram: cabeçalho PES inválido, falha de IO, saída curta, dimensões
incorretas, quadro P, varredura das truncagens até o primeiro cabeçalho de
quadro, falhas de alocação do decoder/ES/planos, limite de 2 MiB, fechamento,
saída intacta e invalidação após remontar a ISO.

## Compilação e vínculo

389/389 unidades PowerPC compilaram, sem conflito de ABI. A inspeção dos 294
objetos com aritmética PS2 rebaixada não encontrou instruções float nativas.
As duas unidades adicionais são o decoder e sua fonte ISO.

O vínculo estrito com Game_Main e Battle_Main ainda falha em **207 nomes**:
104 têm referências em código e 103 somente em dados na auditoria dos objetos.
Essa contagem não mudou em relação à 0.32. Não foram apagadas tabelas,
geradas funções vazias ou desabilitadas verificações do linker.

As APIs nativas XbMpeg_* foram retidas no XEX de diagnóstico e validadas
estaticamente, sem símbolos fortes indefinidos. O novo código não é invocado
pela interface atual do launcher. O XEX não apresenta os filmes, não executa
Game_Main/Battle_Main e não contém o catálogo completo dos menus. O catálogo
de overlay do vínculo do motor foi novamente auditado: 72 unidades, 119 regiões,
83.288 bytes e 46 relocações de ponteiros; o vínculo correspondente continua
falhando, portanto esse mapa não é um executável utilizável.

## O que ainda falta

As nove funções sceMpeg continuam pendentes: o componente entregue não substitui
a API Sony do motor. Falta adaptar o fluxo de src/sys/movie.c, callbacks,
ring/demux, ordem de macroblocos RGBA com alpha GS 0x80, upload ao GS nativo,
clock de reprodução e sincronização com ADXT. O player original usa MMIO/IPU
PS2; encaminhá-lo ao novo decoder sem adaptar essas etapas não funcionaria.

Também permanecem RPC/IOP/SPU2, resets do GS, interfaces, rede e os demais
símbolos do relatório. O manifesto tipado completo dos dados do programa
principal, inicialização nativa, combate/IA/colisões/animações e renderização
integrada seguem pendentes. Saves, modos e online de combate não estão prontos.
Não houve execução em Xbox/emulador, medição de CPU/GPU do console ou validação
de pixels/áudio em hardware. O port ainda não é jogável.

## Reprodução

Upstream 3ca419c7966b11438e2dc3c9a7e1914f102ed545;
xsdk 57b02f1e60667e37d22c3aa613d23d2486514ff8; GCC PowerPC 15.3.0.
O teste host requer GCC, FFmpeg e numpy. Execute da raiz do projeto:

```sh
python3 engine/tools/test_mpeg2.py XSDK ISO OUT --full --sanitize
python3 engine/tools/link_battle.py UPSTREAM CC XSDK OUT --float-mode native-ps2 --platform --root Game_Main
python3 engine/tools/audit_overlay_catalog.py OUT
python3 engine/tools/audit_engine_refs.py OUT AUDIT_OUT
```

A segunda chamada termina com falha porque o vínculo ainda está incompleto.
Evidências: engine/reports/0.33. Patch textual desde 0.32:
extras/port-0.33.patch. Não acompanham ISO, PSS, ADX, AFS ou executáveis PS2.
