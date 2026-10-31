# D47 — adaptação de paletas, somente fontes

Esta revisão atualiza os fontes. Não contém um XEX novo; os executáveis D46 e suas evidências continuam sendo históricos. Não há medição nova de FPS nem aprovação visual do efeito relatado no console.

Foi consultado o ramo `main` de Tenkaichi3Decomp em `363e92c20af366db9a9050abe395b3416e2195fc`. A base de captura da D46 é `3ca419c7966b11438e2dc3c9a7e1914f102ed545`. A comparação do GitHub indica ramos divergentes, com 484 commits exclusivos do ramo consultado e 56 exclusivos da base; não é seguro substituir tudo como uma atualização linear. Os arquivos originais em vendor continuam intactos.

## Mudança aplicada

Em `engine/capture/port/src/gs/gs_draw.c`, o cache CLUT agora confirma as 256 cores depois do hash e inclui os bits altos, que contêm alpha, no hash. A mudança vem de `clut_texture` e `sClutPx` do upstream consultado. Paletas diferentes com o mesmo hash não podem mais reutilizar a mesma textura por engano. O armazenamento adicional é de 64 KiB para os 64 slots existentes; o limite de texturas não aumenta.

A conversão RGBA para a ordem de bytes do Xbox foi conservada, assim como a proteção das texturas usadas no quadro atual. Também se invalida o identificador destruído antes de uma alocação que possa falhar, evitando devolver uma textura já liberada em uma consulta posterior.

O teste compila e executa a função real extraída do arquivo, com ASAN/UBSAN. Inclui colisões construídas para o hash antigo e o novo, reutilização correta, permutação de índices, bytes RGBA de upload, proteção durante o quadro, falta de slots e recuperação de falhas de alocação/criação. O teste negativo com a função D46 reproduz a reutilização incorreta. Isso demonstra a correção do cache; não identifica esse defeito como a causa confirmada do efeito fotografado.

## O que a comparação permite aproveitar depois

O upstream registra menus originais, saves, áudio e online experimental no PC. A documentação atual exige o mesmo release entre participantes online. Esses serviços ainda precisam ser ligados às APIs do Xbox; copiá-los não habilita automaticamente esses recursos no XEX.

O renderizador novo também tem decodificação de texturas por trabalhadores pthread/SDL e tratamentos de glow em resolução menor. Esses caminhos têm dependências e estados que o backend Xenos não oferece com o mesmo contrato; não foram ativados sem testes. O cache de pipelines por chave já existia na nossa captura D46, portanto não foi apresentado como uma otimização nova.

## Reproduzir

Execute `python3 tools/test_d47.py` na raiz. São cinco grupos: identidade de paletas, pressão de texturas, alvos/snapshots/estados gráficos, interface e integridade/sintaxe dos 566 arquivos originais.

A revisão é pequena e revisável em `engine/patches/d47-clut-identity.patch`. Proveniência em `adaptation/upstream-review-d47.json`; resultados em `validation-d47/final`. A compilação e os testes nativos D46 documentados no README-D46 não foram repetidos nesta revisão. NOT_READY continua 2. Não há promessa de 30 FPS, jogo completo, menus completos ou online funcional no Xbox.
