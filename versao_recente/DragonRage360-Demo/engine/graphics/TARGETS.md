# Atualização 0.18 — targets GS separados dos targets antigos

`dr_targets_ensure_gs` cria color/aux/D24S8 com passes em faixas, snapshots
congelados e reconstrução de stencil. `dr_targets_ensure` mantém o caminho
antigo color/depth. O alias D24S8/RGBA8 e os pixels continuam sem validação
no Xbox. Leia RELATORIO-0.18.md; as notas abaixo descrevem o histórico.

# Targets nativos — fontes 0.11

`dr_targets.c` implementa superfícies de cor e profundidade preservadas entre
passes. O núcleo `dr_xenos.c` usa o módulo diretamente, e a ponte `dr_gs_replay.c`
aceita listas `GsDraw` com vários IDs quando `DrGsReplay.saved_targets=1`.
Isso não registra um `GsBackend` nem inicia o jogo.

## Ciclo implementado

1. Criar os targets com `dr_xenos_target_create(id, width, height)`, fora de batch.
2. Iniciar com `dr_xenos_begin_target(id)`, ou usar `dr_xenos_replay_targets`.
3. Desenhar com o núcleo e trocar com `dr_xenos_select_target(id)`.
4. `dr_xenos_end()` salva o último target antes da fence.
5. Fora do batch, remover com `dr_xenos_target_drop` ou fechar o núcleo.

Ao sair de um target, o módulo resolve D24S8 sem limpar e depois resolve RGBA8
limpando eDRAM. Reconfigura o target entre os resolves, conforme o contrato do
SDK. Ao voltar, um shader copia a cor sem reescalar alpha e escreve a profundidade
amostrada em `gl_FragDepth`. Os shaders GLSL foram compilados em containers
nativos pelo glslang 15.1 e pelo compilador aberto xost/xsc do SDK.

Targets novos começam em cor zero e profundidade zero. A primeira cópia de
eDRAM indefinida serve apenas para acionar o clear: ela nunca é amostrada antes
de ser sobrescrita pelo primeiro store. O base level das texturas é colocado
no canto para permitir resolves também em texturas pequenas.

Cada troca altera shaders, depth, texturas e outros estados. O núcleo nativo
reconfigura seu estado antes de cada draw. O quad de restauração é imutável,
em alocação própria, e os vértices da luta/diagnóstico usam offsets separados
na arena do núcleo. Uma chamada externa de present/resolve não deve acontecer
durante um batch de targets.

## Memória e falhas

- 24 IDs de target, cada um com RGBA8 e D24S8, sem MSAA.
- Até 1280×1024, largura múltipla de 32 e validação do limite real de eDRAM pelo SDK.
- Orçamento de 128 MiB para as alocações dos targets, incluindo arredondamento
  de páginas de 64 KiB. A memória dos outros componentes não está nesse orçamento.
- O tamanho vem de `xsdk_texture_init` antes de alocar. O orçamento inclui o
  pico com o target antigo durante resize. Uma substituição pode falhar por
  falta de espaço mesmo se o tamanho final coubesse.
- Falha na segunda textura de um resize libera a primeira nova e preserva o
  target antigo. Timeout de fence preserva toda a memória em uso pela GPU.
- A lista completa é validada antes do primeiro bind no replay de targets:
  IDs, handles, intervalos de vértices, capacidade e scissor de cada superfície.

## Verificação e limites

```sh
ASAN_OPTIONS=detect_leaks=0 python3 engine/tools/test_targets.py /caminho/xsdk /tmp/targets-test --sanitize
ASAN_OPTIONS=detect_leaks=0 python3 engine/tools/test_gs_replay.py /caminho/upstream /caminho/xsdk /tmp/replay-test --sanitize
```

Os testes passaram com `-Wall -Wextra -Werror`, ASan e UBSan. O modelo CPU
verifica chamadas, ordem de resolve/restore, A→B→A, falhas, limites e posse de
memória. Ele representa cor/profundidade por valores de teste: **não rasteriza
shaders nem verifica pixels Xenos**. Leak scan foi desligado pela restrição de
acesso a `/proc` do ambiente. Não houve compilação PowerPC dos fontes novos,
novo XEX, recursos reais da ISO, emulação Xbox ou execução no console.

Ainda faltam stencil/aux e DATE/FBA, alpha-test, mipmaps, buffers amostrados
pelos efeitos GS, cópia/readback de targets para o contrato de `GsBackend`,
shaders VU e o registro do backend. A precisão da profundidade e a orientação
da restauração exigem teste de pixels no hardware. Nada aqui comprova sombras,
auras, explosões ou uma luta jogável.
