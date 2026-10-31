# 0.29 — ISO completa, recursos reais e heap do motor

A ISO do endereço fornecido foi baixada e os arquivos necessários foram
extraídos. **Os três AFS estão disponíveis; não são mais um bloqueio de entrada.**
O jogo e a luta ainda não foram executados: Battle_Main permanece fora do XEX.
Esta entrega atualiza os fontes e inclui o diagnóstico 0.27 inalterado.

## Entrada real disponível e validada

ISO: DBZ3_SLUS_21678.ISO, 3.020.816.384 bytes.
Origem: https://archive.org/download/dbz-3-slus-21678/DBZ3_SLUS_21678.ISO
O tamanho e SHA-1 conferiram com os metadados publicados do mesmo item.
SHA-1: a1c4d21c154da8b1d4b15055907edb0abb055e63
SHA-256: 93f1f911e9a2bbdf92e5794fc3075ddf3f87db3df4b1cb8cfbb0448721c91172

A extração usa o próprio dr_disc.c do port. Foram lidos SYSTEM.CNF,
SLUS_216.78, BIN/DBZP.BIN e os seguintes arquivos:

| Arquivo | Bytes | Entradas AFS |
| --- | ---: | ---: |
| DATA/PZS3US0.AFS | 14.336 | 1 |
| DATA/PZS3US1.AFS | 1.462.016.000 | 3.399 |
| DATA/PZS3US2.AFS | 1.220.337.664 | 65.201 |

Todas as entradas AFS foram verificadas quanto aos limites do arquivo.
engine/tools/extract_iso.py permite repetir a extração com a ISO local;
--inspect-only verifica os diretórios/tabelas sem copiar os arquivos grandes.
Os arquivos do jogo são entradas externas, não são incluídos no pacote de fontes.
Os relatórios incluem tamanhos e hashes, não os recursos binários extraídos.

## Correções na memória que a luta usa

Heap_Create calculava o tamanho como um endereço final PS2 menos o endereço
de D_3BE71C. Um símbolo nativo realocado não pode ser usado nessa subtração.
No perfil PORT_BIGENDIAN, o orçamento agora é a diferença NUMÉRICA original
do jogo USA, alinhada em 4 bytes; não é um ponteiro PS2 convertido em tamanho.
A obtenção do bloco continua usando Port_Malloc da plataforma.

XbMem_InitHeap inicializa separadamente a arena relocável e verifica se seu
intervalo cabe em endereços de 32 bits. Ela não mapeia MMIO nem declara a
plataforma inteira pronta. Uma nova chamada não limpa nem retrocede uma arena
em uso. O estado inicial dos testes de memória também marca as janelas fixas
como indisponíveis até a inicialização, evitando acessá-las antes de possuí-las.

A criação do heap trata falha de alocação antes de escrever no cabeçalho.
Uma repetição de Heap_Init preserva as alocações vivas, em vez de consumir
outro bloco da arena. XbHeap_Ready/LastError expõem o resultado ao futuro boot.
O boot ainda não chama essa sequência dentro do XEX.

Heap_Alloc recusa alinhamento zero, índice de heap inválido e soma de tamanhos
que estouraria o limite. Heap_Free passa a aceitar NULL e recusa ponteiros
inválidos ou já liberados sem acessar o bloco nulo. O split, a alocação pela
frente/cauda e a coalescência continuam sendo o algoritmo original do jogo.
Isso não implementa validação geral de uma arena arbitrariamente corrompida.

A liberação de buffers grandes de cenário também esquece os registros de
recursos do intervalo antes de liberar a memória, como já acontecia nos blocos
do heap. Port_Malloc verifica o cursor antes de subtrair a capacidade restante.

O perfil nativo agora fornece Port_StageBufSize com o tamanho original do disco
(0x6CB800) e Port_WideFactor=1.0, coerente com a composição 4:3 que o renderizador
já selecionava. São valores de configuração desse perfil; não implementam
modos widescreen ou cenários adicionados de fora do disco. Nenhuma resposta
vazia de sucesso foi acrescentada para substituir um serviço ausente.

## Testes e limites

| Verificação | Resultado |
| --- | --- |
| Motor/plataforma PowerPC | 383/383 unidades compiladas |
| ABI float | Nenhum conflito |
| Vínculo Battle_Main | 139 → 137 nomes pendentes; ainda falha |
| Heap real adaptado + arena nativa | 3.016 verificações, incluindo 3.000 operações aleatórias |
| Alinhamento, dados e coalescência | Passaram com ASan/UBSan em host |
| Falha de inicialização, repetição, lotação e liberações inválidas | Passaram |
| Registro de recursos após liberação normal/grande | Invalidado nos testes |
| Animações reais | 66.654 descomprimidas; 57.259 não vazias validadas e equivalentes após conversão BE |
| Modelos reais | 698 modelos, 43.121 nós e 32.454 cadeias VIF de corpo validados |
| VU com um modelo real | 13.476 vértices / 4.492 triângulos em pose de referência |
| Carregador de tabelas com executáveis reais | Três tabelas escalares copiadas para buffers de teste BE |
| ADXF/BPE/conversores | Regressões em host passaram |
| XEX | Validação estática passou; diagnóstico 0.27 inalterado |
| Luta, PowerPC em execução, GPU Xbox e desempenho | Não testados |

O teste do heap compila o próprio engine/combat/heap.c, o módulo de memória,
a classe de perfis e o registro de conversores. No host, apenas os transportes
de alocação e log são substituídos: mmap obtém memória abaixo de 4 GiB sem
sobrepor mapeamentos existentes. Não há emulação de Xbox ou de MMIO. ASan foi
usado com detect_leaks=0; isso não mede vazamentos.

A validação das animações não toca a reprodução durante a luta. A dos skeletons
cobre o núcleo de 64 bytes de cada nó; não afirma converter todos os campos.
A geometria foi produzida pelo VU original no computador e não rasterizada em
Xenos. Esses testes não são partidas completas, iluminação original validada,
rollback ou benchmark de 60 FPS.

As três tabelas do teste real são gMathfSinCoef, gSndBankVolume e
gBattleTimeLimitTbl, com larguras explícitas verificadas nas declarações C.
O destino é um buffer de teste; isso não preencheu as tabelas globais do motor
nem gerou o manifesto completo necessário ao boot.

## O que ainda impede executar a luta

O vínculo tem 137 referências pendentes, incluindo áudio ADXT/IOP/RPC,
serviços Port_*, leitura de imagem GS e rótulos usados em tabelas de bibliotecas.
Port_StageBufSize e Port_WideFactor foram os dois nomes resolvidos nesta etapa.
Os objetos de dados continuam como skeletons sem todos os valores originais.

Faltam o carregamento tipado completo das tabelas e relocações da compilação,
a inicialização nativa do fluxo original e a integração dos produtores de
pacotes, memória de trabalho, animação, IA, colisões e apresentação do combate.
O renderizador ainda precisa de validação com a sequência real do combate e
pixels em Xbox, inclusive DATE/FBA/stencil. Áudio, saves, online e estabilidade
de partidas completas continuam sem validação.

A ISO completa permite agora continuar os testes reais. Ela não resolve as
funções e os adaptadores que faltam no código. Não foi criado um XEX com uma
luta falsa nem forçado um boot em tabelas incompletas.

## Reproduzir

- engine/tools/extract_iso.py ISO OUT
- engine/tools/test_heap_native.py UPSTREAM XSDK OUT --sanitize
- engine/tools/test_body_pipeline.py ISO OUT --sanitize
- engine/tools/test_real_iso.py ISO UPSTREAM OUT
- engine/tools/test_gamedata_real.py EXTRACTED_DISC_DIR OUT
- engine/tools/link_battle.py UPSTREAM CC XSDK OUT --float-mode native-ps2 --platform

O checkout exigido continua 3ca419c7966b11438e2dc3c9a7e1914f102ed545.
SDK/toolchain e ISO são externos. Evidências em engine/reports/0.29.
