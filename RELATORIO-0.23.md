# 0.23 — DMA/VIF como pré-requisito; combate continua pendente

**Os dois objetivos solicitados não foram concluídos.** Esta entrega adapta
um consumidor de comandos gráficos e o liga ao XEX, à captura sintética e
ao visualizador real. Não contém a simulação original de combate e não deve
ser apresentada como jogo executável ou GsBackend completo.

## O que foi implementado

- `dr_gs_stream_chain`: consumidor canônico little-endian para REFE, CNT,
  NEXT, REF, REFS, CALL, RET e END. Os endereços são resolvidos em até 64
  regiões explícitas, com limites reais de recurso e rejeição de sobreposição,
  referências inválidas, alinhamento incorreto, overflow, ciclo e pilha.
- Transferência TTE das duas palavras VIF da tag; estado de payload preservado
  entre trechos. DIRECT/DIRECTHL entregam GIF ao consumidor configurado.
- STCYCL, BASE, OFFSET, ITOP, STMOD, STMASK, STROW, STCOL, MPG, UNPACK,
  MSCAL/MSCALF e MSCNT alimentam a VU de referência. UNPACK suporta formatos,
  máscara e ciclos que esse interpretador já implementava. Comandos não
  suportados são erros explícitos. Flushes são síncronos; não há emulação
  de interrupções ou arbitragem temporal das vias PS2.
- Nova API de controle VIF individual: BASE e ITOP não reiniciam indevidamente
  a dupla bufferização configurada por OFFSET. Os dados VU permanecem LE.
- O diagnóstico GS gera uma cadeia CNT/TTE/FLUSHE/DIRECT/GIF/END e passa pelo
  novo consumidor antes do `gs_core.c`, `gs_draw.c` e backend Xenos existentes.
  Seus registradores e geometrias continuam sintéticos.
- O pipeline real de corpo/facial agora envia MPG, constantes, BASE/OFFSET,
  MSCAL e comandos dos recursos pelo consumidor VIF antes da VU de referência.
  Os GIF resultantes ainda são convertidos pelo visualizador separado em
  triângulos da pose fixa. Isso não é a captura de uma luta.
- Fontes duplicados, API de geração do interpretador, testes, Makefile e
  verificação estática do XEX foram atualizados. Não foram adicionadas funções
  vazias para simular as dependências de `Battle_Main`.

## Evidências obtidas

- Testes ASan/UBSan do consumidor: oito IDs DMA, TTE ligado/desligado,
  referências externas, CALL/RET, DIRECT/MPG fragmentados, UNPACK com máscara,
  ciclos de preenchimento, NUM=0 com 256 vetores, controle VU e MSCAL.
- Casos de erro: intervalos sobrepostos e fora de limites, borda de 4 GiB,
  endereço desalinhado, laço DMA, excesso/ausência de pilha, limite de palavras
  e de instruções VU, payload truncado, comando desconhecido e erro de callback.
- Captura GS completa passou em nove ciclos ASan/UBSan com a nova entrada
  sintética DMA/VIF e os consumidores/shaders VU/DATE/FBA/stencil existentes.
  As chamadas XSDK são simuladas, sem rasterização de GPU.
- Corpus real: 698 corpos, 43.121 nós e 32.454 cadeias VIF continuam válidos.
  Um corpo real executou o programa 0 da ISO: 53 cadeias, 564 MSCNT, 564 GIF,
  4.492 triângulos e 13.476 vértices na pose de referência.
- Comparação independente com os fontes da 0.22: todos os 377.328 bytes de
  vértices desse corpo foram idênticos. O hash está em
  `engine/reports/0.23/body/stream-geometry-comparison.json`.
- Os 13 materiais e as cinco substituições faciais passaram novamente com
  recursos reais; slots ausentes e falta de capacidade foram rejeitados.
  O visualizador com recursos reais também passou com GPU simulada.
- A geração do interpretador foi reproduzida a partir do upstream fixado,
  patch endian e API atual; o corpo C gerado correspondeu ao usado no build.
- Todos os objetos do título foram recompilados para PowerPC, seguidos de
  build final após ajustes do consumidor. ELF, símbolos, ABI, PE e hashes XEX
  passaram na verificação estática. Leia `VALIDACAO.json` e `BUILD-RESULT.txt`.

## Combate — não concluído

`Battle_Main` continua fora do ELF e do XEX. A auditoria do objeto original
`battle.c` contra o título final continua encontrando 87 dependências diretas
não ligadas; não é uma contagem transitiva do motor inteiro.

Ainda faltam carregamento/inicialização e integração dos módulos de
personagens, IA, colisões, estado de partida, câmera, cenário, animações e
serviços. A memória/endereço PS2, heap e dados nativos do motor também não
foram adaptados como conjunto. Portanto não houve uma luta executada.

A nova API NÃO é o `Dma_Flush` original. O caminho original
`Gfx_EndFrame -> Dma_Flush -> VIF/GIF` ainda não entra em execução no título.
Seus produtores misturam escritas numéricas u32/u64, cópias de arquivos LE e
endereços PS2. Eles precisam ser adaptados por tipo de pacote, além de
registrar regiões/endereço válidas; não basta trocar bytes da lista inteira.

## Renderização — não concluída

Continuam pendentes os demais consumidores de buffers e reinterpretações,
a sequência de sombras com dados reais, iluminação e VU dentro do combate,
os demais estados do GS, auras e explosões originais. O novo consumidor usa
a VU de referência na CPU; não seleciona automaticamente os shaders nativos
0/4/6 existentes a partir das chamadas MSCAL de uma luta.

O limite de 1.024 vértices agregados no replay GS nativo continua sendo um
bloqueio para frames completos. O visualizador separado usa seus próprios
lotes; carregar o modelo nele não elimina esse limite do caminho de captura.

DATE, FBA e stencil continuam SEM PIXELS VALIDADOS EM HARDWARE. Nenhum XEX
foi executado no Xbox/emulador. Não houve partida, medição de FPS ou CPU/GPU.
A ausência de hardware não é o único bloqueio: os componentes e a integração
acima permanecem ausentes.

## Limites do consumidor

Um único contexto VU global por vez, sem concorrência. Os buffers precisam
permanecer válidos e imutáveis durante a chamada. Cada cadeia tem limites
padrão de 8.192 tags e 262.144 palavras VIF, e cada chamada VU tem orçamento
de 10.000 instruções. Uma falha pode deixar GS/VRAM/VU parcialmente alterados;
descarte os desenhos capturados e reinicialize antes de reutilizar. Isso não
é captura/reconstrução de estado para rollback nem simulação determinística.

Fontes e evidências acompanham o pacote; ISO, microcódigo comercial e dumps
de geometria usados na comparação não são distribuídos.
