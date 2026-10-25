# DragonRage360 0.37-D3 — catálogo nativo e preparação parcial do combate

24/10/2026. O primeiro pedido foi tratado: o catálogo selecionado do executável nativo agora é gerado com `complete=true`. **O segundo pedido ainda não está completo:** há instalação de colisão no estado nativo e preparação de esqueletos/animações dos dois modelos, mas não existem lutadores completos inseridos numa luta. O XEX continua sendo um diagnóstico. Não foi executado no Xbox nem no Xenia.

## As quatro faixas, sem tipos inventados

| Faixa anterior | Decisão e prova |
|---|---|
| `D_002FE8D0`, 8 bytes | Primeiros 4 bytes: inteiro de 32 bits. No programa USA, `0x108B54` e `0x108B78` formam o endereço, `0x108B80` faz `LWC1 f30,0(v0)` e `0x108B84` executa `CVT.S.W`. É o inteiro 224 convertido para float pelo efeito de água. Os 4 bytes seguintes são preenchimento zero conferido. `test_water_pool.py` verifica os campos dessas instruções, a revisão SHA-256 e os dados. |
| `D_002EC2C0 + 16`, 16 bytes | Não pertence ao vetor de 16 bytes declarado pelos leitores nativos. A auditoria dos objetos encontrou 30 relocações para o início do vetor, todas com addend zero e em consumidores revisados; suas fontes são fixadas por SHA-256. A cauda não é carregada. |
| `D_002FE958`, 22 bytes | Pool original sem referência nas relocações alocadas das unidades nativas. Não recebe um tipo artificial e não é carregado. |
| `D_002FEB88`, 8 bytes | Pool original sem referência nas relocações alocadas das unidades nativas. Não recebe um tipo artificial e não é carregado. |

Portanto, uma faixa recebeu tipo e preenchimento comprovados; **46 bytes das outras três foram excluídos como pools sem consumidor neste build nativo**. Isso não afirma que todo leitor possível do PS2 foi provado. As exclusões estão em `native_unused_ranges` e sua evidência em `unused-native-proof.json`. Nenhum desses bytes é copiado ou zerado pelo carregador. Uma nova referência nativa, uma alteração nos leitores do vetor ou um ponteiro do catálogo para esses pools bloqueia a regeneração; os testes injetaram uma referência real e confirmaram a recusa.

O catálogo tem 33 regiões, 2.738 operações, 35.318 bytes cobertos e 46 bytes explicitamente excluídos. São dados selecionados para o executável nativo, **não todos os dados da ISO**. A invariância de preenchimento zero continua distinta de tipagem por estrutura. Os 23 trechos de alinhamento revisados na D2 continuam excluídos pelo motivo separado já documentado.

`complete=true` não removeu a proteção da luta. O carregador foi testado com catálogo completo, sem `ALLOW_PARTIAL`, e o marcador de prontidão permaneceu 2. A primeira chamada de `Battle_Main` continua sendo a verificação de prontidão antes de `Battle_GetWork`. A inicialização completa dos recursos e da plataforma continua obrigatória.

## Colisão instalada nos leitores originais

Novo `xb_battle_stage.c/.h`:

- prepara uma arena própria com cabeçalhos, árvores, polígonos e vértices em representação nativa, mantendo os offsets em palavras que `ColMesh_*` espera;
- constrói zonas, vizinhos, registros de colisão, limites e o estado inicial inteiro dos objetos destrutíveis;
- instala os ponteiros reais `gBtlStage` e `gColMeshBase`, protegendo contra substituição de cenário já instalado;
- mantém o cenário com a flag **NOT_READY**: ainda não possui o restante do estado necessário para todos os subsistemas;
- chama os **originais** `StgCol_QueryZone`, `ColMesh_WalkBox`, `ColMesh_GetPoly` e `ColMesh_GetPolyVerts` sobre essas estruturas, com o filtro original de objetos inteiros/quebrados;
- impede reentrada e liberação durante a consulta, restaura os globais de percurso e permite desinstalar e liberar a arena.

Nos testes, o recurso original foi liberado antes das consultas, comprovando que os ponteiros vivos pertencem à arena. Comparação com percurso independente em Python:

| Estado dos objetos | Ocorrências de triângulo | FNV64 dos valores e da ordem |
|---|---:|---|
| Inteiros | 12.251 | `ddc3c6e2c300bf12` |
| Quebrados | 9.789 | `a6ddb9dbe6da84b2` |

Os números diferem dos 13.583 da D2 porque a D2 percorria todas as malhas, incluindo versões inteiras e quebradas; agora o leitor original aplica o estado de cada objeto. Os testes passaram com ASan/UBSan em host. Os layouts e as máscaras de 64 bits também foram conferidos estaticamente no build PowerPC; o adaptador usa os tipos portáveis do upstream, evitando o `long` de 32 bits do PPC.

Isso instala a **parte de colisão**, não o cenário completo. A flag NOT_READY não foi removida, e a simulação de física do cenário não foi iniciada.

## Dois modelos com estado nativo de animação

Novo `xb_battle_fighter.c/.h`, para os modelos 0 e 1 do perfil fixo:

| Modelo | Registros de esqueleto | Animações comprimidas decodificadas | Slots vazios | Animações do modelo presentes/vazias |
|---|---:|---:|---:|---:|
| Esquerda | 69 | 361 | 53 | 1 / 7 |
| Direita | 69 | 357 | 57 | 1 / 7 |

O construtor lê a hierarquia real, cria os registros nativos `BobjBone` e os buffers decodificados com conversão por campo. Os IDs do modelo vão até 111; não foram confundidos com o limite de 71 canais de animação. Os oito ponteiros `modelAnims` seguem `BtlObj_BindTables`: entradas originais `0x1D..0x24`, indexadas a partir de 1. Nos dois modelos, somente o slot 7 contém uma animação.

Um `BobjObj` próprio recebe o esqueleto, os ponteiros das animações do modelo e o estado inicial do player. As funções **originais** `BtlObjAnim_PlayModel` e `BtlObjAnim_Step` passaram por 200 avanços e voltas de loop: duração 67 no modelo esquerdo e 72 no direito. O arquivo-fonte já havia sido liberado. Os sete slots ausentes são reportados como vazios, sem sucesso fictício.

**Esse objeto ainda não é um lutador de combate.** Os 414 buffers decodificados ficam guardados separadamente; não foram apresentados como se fossem dados comprimidos em `mdl.anims`. Faltam os binders de reprodução/retarget desse caminho, as tabelas e parâmetros restantes do modelo, volumes de corpo/ataque, poses e matrizes animadas, referências do renderer, texturas/facial, `BobjWork`, estado do personagem e seu registro nos gerenciadores de IA/combate. Nenhum construtor retorna prontidão para spawn completo.

## Novo XEX e testes

`startup_test.txt` continua `typed-data`. No novo diagnóstico:

1. carrega o catálogo pela ISO montada;
2. prepara e instala a arena de colisão com NOT_READY;
3. executa uma consulta com os leitores originais;
4. prepara os dois modelos/animações, inicia o slot 7 e avança um frame;
5. registra resultados no `default.log`, mantendo o combate bloqueado.

Os estados possuem memória própria e são liberados no encerramento ou antes de repetir o diagnóstico. Essas etapas estão compiladas e vinculadas, mas **a sequência ainda não foi executada no Xbox**. A interface não desenha uma luta ou os personagens animados por esse novo caminho.

- Base PowerPC: 398/398 unidades; vínculo com catálogo: 399/399; zero indefinidos e zero conflitos de ABI.
- Catálogo final: 2.738 operações e 1.779 slots de ponteiro conferidos após o vínculo; 1.473 ligações mudaram e foram realocadas simbolicamente.
- Verificações do carregador real, catálogos incompletos/completos, falhas atômicas, hash, ponteiro, limites, reentrada e sobreposição: PASS em host com ASan/UBSan.
- Provas das exclusões, referência nativa injetada e leitor alterado: PASS.
- Colisão original e player original dos dois modelos: PASS em host com ASan/UBSan.
- Verificação estática do XEX2 e guarda PowerPC de `Battle_Main`: PASS.
- Isolamento das 103 referências de bibliotecas legadas mantido; a sondagem de `Game_Main` completo continua com 69 dependências. Não foram criados stubs.

SHA-256 do novo `default.xex`:
`564c16bb2bcbaa5b5f3e21c8d3372491e0fb431c85bfc913f713cde81c676e91`

Fontes públicos mantidos nos pins da D2: upstream `3ca419c7966b11438e2dc3c9a7e1914f102ed545`, xsdk `57b02f1e60667e37d22c3aa613d23d2486514ff8`. Toolchain GCC 15.3.0 / binutils 2.47 / newlib 4.6.0.20260123 com os patches do SDK. Os adaptadores novos foram compilados para PPC com os tipos corrigidos; a arena foi também conferida com o verificador de alinhamento de FPR.

Os testes usaram os mesmos recursos USA locais da D2, cobertos pelos intervalos e fingerprints já verificados. Não há ISO, programas MIPS nem arquivos de modelos/animação extraídos no pacote.

## O que falta do segundo pedido

Ainda é preciso terminar os binders/tabelas acima, preparar os parâmetros dos dois personagens e o cenário completo (MAPD, objetos/partes, animações, luzes, efeitos e corpos físicos), inicializar os gerenciadores, integrar entrada e apresentação e validar uma partida fixa. A preparação de colisão não equivale à inicialização de todos os consumidores de `gBtlStage`; a preparação do player não equivale a criar os lutadores. Não execute `Battle_Main` contornando a guarda.

Fontes e XEX atuais na raiz; D2 anterior em `diagnostico-anterior-D2/`. Evidências em `checkpoint-dados-D3/`. O patch incremental parte exatamente da 0.36-D2 e já está aplicado aos fontes da raiz. Para reproduzir o XEX, use o alvo `make typed-data-diagnostic` com a ISO do próprio usuário e os caminhos do SDK/toolchain/upstream indicados na D2.

Os testes adicionais `test_native_stage.py`, `test_native_fighter.py`, `test_reviewed_unused.py` e `test_water_pool.py` aceitam caminhos externos para os dados de teste. Consulte `--help`. Eles não inicializam uma partida nem validam hardware.
