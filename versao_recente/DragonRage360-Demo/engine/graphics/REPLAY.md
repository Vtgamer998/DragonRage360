# Atualização 0.21

A ponte traduz native=1 (contorno) e native=3 (CLUT por ID/profundidade).
Os marcadores native=4/5 mantêm as cópias de efeitos e DATE separadas.
Snapshots ausentes/de outro target e formatos inválidos rejeitam a lista
antes de emitir primitivas. Ainda não há simulação de combate ligada ou
validação de pixels em Xbox. Leia ../../RELATORIO-0.21.md.

--- Histórico anterior ---

# Atualização 0.20

A ponte aceita as três famílias VU da captura original, testes/escrita Z,
quantização no fragmento, clamp de target e máscara de stencil com reset no
início do grupo. Não conclui todos os estados/efeitos nem liga o combate.
Leia ../../RELATORIO-0.20.md e ../reports/0.20/. Sem pixels de hardware.

--- Histórico anterior ---

# Atualização 0.19 — captura original e backend no XEX

A captura e o backend experimental agora estão ligados ao título e têm um
caminho GIF sintético acessível. A ponte traduz samplers 0..15. VU/GPU,
quantização Z, sombras completas e consumidores nativos seguem pendentes.
Leia ../../RELATORIO-0.19.md e ../reports/0.19/. Não há validação de GPU.

--- Histórico anterior ---

# Atualização 0.18

Há agora um objeto GsBackend experimental e patch de registro no gs_draw.c.
A ponte aceita marcadores DATE/aux (native 5/4), DATE/FBA e cor de target em
surfaces GS. O backend e a ponte ainda ficam fora do link do launcher.
Leia RELATORIO-0.18.md para testes, limites e pendências, inclusive VU e efeitos.
O núcleo de 1.024 vértices agora tem 256 slots de textura. As notas abaixo
são históricas; os estados atuais estão no relatório 0.18.

# Atualização 0.17 — ponte ainda fora do motor e do XEX

TFX 0..3, TCC, ATST desligado/0..7, AREF inteiro, blend keys e máscaras de
canais agora são traduzidos para o núcleo nativo. O shader implementa o
descarte de alfa e a escala da fonte pelo alfa usada no bit 9 do blend key.
A lista inteira continua validada antes de desenhar. Leia RELATORIO-0.17.md
na raiz para testes e limites atuais.

Continuam sem implementação: registro de GsBackend, VU/GPU, stencil,
DATE/FBA, targets como textura, efeitos nativos, estado de samplers/mips do
GS e profundidade quantizada. Os mips explícitos são uma API do núcleo;
a ponte não carrega mipmaps originais. O limite continua 1024 vértices por
lista e 16 texturas. A ponte foi compilada separadamente para PowerPC.

As notas abaixo são históricas e seus limites pertencem às versões indicadas.

# Atualização 0.11

A ponte também aceita `saved_targets=1`: cada `GsDraw.target` seleciona um
target criado no núcleo nativo. O replay valida toda a lista antes do primeiro
bind e preserva cor/profundidade entre passes. Veja TARGETS.md para o ciclo,
os testes e os limites. Não há registro de GsBackend ou luta executável.

--- Histórico 0.10 (o limite a uma superfície abaixo se refere à 0.10) ---

# Ponte de comandos GS — fontes 0.10

`dr_gs_replay.c` consome a estrutura **original** `GsDraw` de `gs_draw.h`,
revisão upstream `3ca419c7966b11438e2dc3c9a7e1914f102ed545`.
Não registra um `GsBackend` e não altera `GsGpu_Init`. É um passo de integração
testável, ainda sem consumidor no loop do jogo ou no XEX distribuído.

Recebe uma lista, vértices CPU e as chaves correspondentes aos índices de
pipeline. O chamador fornece a correspondência de handles de textura, a
textura branca e as dimensões da única superfície diagnóstica. O renderizador
nativo deve estar aberto e sem batch ativo. A função cuida de begin/end;
o chamador continua responsável pela abertura do dispositivo e pelo present.

O subconjunto aceito é: vértices CPU, uma superfície escolhida pelo chamador,
triângulos/linhas/pontos, MODULATE com TCC ou cor sem textura, sampler point/repeat,
teste alpha desligado/ALWAYS, teste Z ALWAYS sem escrita, RGBA completo e
blend opaco ou source-alpha convencional. Este último mantém as limitações
de precisão/clamping do núcleo 0.9; não demonstra equivalência GS em pixels.

VU, outros targets, framebuffer como textura, efeitos nativos, alpha-test com
descarte, DATE/FBA, filtros, mipmaps, máscaras, profundidade quantizada e demais
blends retornam erro. O blend aditivo original usa o bit 9 da chave para
escalar a fonte no shader: é rejeitado, não confundido com alpha aditivo simples.
Nenhum draw é emitido quando qualquer comando é incompatível ou inválido.
O núcleo valida a lista inteira antes de emitir, inclusive os últimos handles
e vértices; falhas posteriores do dispositivo continuam fora dessa garantia.

São no máximo 1024 vértices copiados por replay, com memória temporária limitada.
Não é capacidade suficiente nem otimização demonstrada para uma luta.
As cópias são CPU-nativas com asserts de campos, sem casts entre estruturas
incompatíveis. Nenhum recurso da ISO é interpretado nessa ponte.

## Executar os testes

```sh
python3 engine/tools/test_gs_replay.py /caminho/Tenkaichi3DecompAndroid /caminho/xsdk /tmp/replay-test
ASAN_OPTIONS=detect_leaks=0 python3 engine/tools/test_gs_replay.py /caminho/Tenkaichi3DecompAndroid /caminho/xsdk /tmp/replay-asan --sanitize
```

O teste usa o cabeçalho real do upstream e mocks de chamadas xsdk. Exercita
replay válido, offsets/intervalos, limites, handles inválidos no fim da lista,
estados não implementados e batch já ativo. O ASan/UBSan passou; a detecção
de vazamentos ficou desligada porque o ambiente não permite inspecionar
`/proc/.../task`. Não houve execução de shader/GPU ou emulação do Xbox.

## Próxima integração necessária

Implementar a posse e ciclo de vida dos attachments color/aux/depth de cada
target, resolve/cópias e restauração dos estados após resolve; depois ampliar
shaders e VU. Só então registrar um `GsBackend` com callbacks reais. Fazer esse
registro agora exigiria callbacks sem implementação e produziria falhas
ocultas em passes essenciais do jogo.

Os fontes novos estão somente em `engine/graphics`. `src/dr_xenos.c` continua
sendo a versão do diagnóstico 0.9, correspondente ao `default.xex` preservado.
O compilador xsdk não estava disponível nesta execução; os fontes 0.10 não
foram recompilados para PowerPC e não são anunciados como novo XEX.
