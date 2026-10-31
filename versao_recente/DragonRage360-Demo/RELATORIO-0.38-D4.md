# DragonRage360 0.38-D4 — player, poses e volumes nativos

**A lista solicitada ainda não está toda concluída.** Esta revisão implementa reprodução/retarget, `BobjWork`, poses, matrizes e volumes de dois modelos; prepara texturas e referências de geometria. Não cria os personagens completos nos gerenciadores nem desenha uma luta. O novo XEX continua sendo diagnóstico e não foi executado no Xbox nem no Xenia.

## Resultado por item solicitado

| Item | Resultado desta revisão |
|---|---|
| Reprodução | `BtlObjAnim_Play`/`PlayModel` e `BtlObjAnim_Load` originais ligados aos dados próprios. O carregador de motions reconhece o proprietário e copia a animação nativa para a camada real de `BobjWork`. A tabela de presença aponta para cópias reais dos recursos comprimidos, nunca para buffers decodificados disfarçados de BPE. |
| Retarget | `BtlObjAnim_PlayFrom` e `BtlObjAnim_Retarget` originais executados, usando os esqueletos e alturas dos modelos. O teste libera o personagem de origem e continua amostrando a cópia no destino. A variante baseada na tabela comum ainda não foi inicializada. |
| Tabelas/parâmetros | Preservado o prefixo de quatro bytes do recurso de parâmetros e convertidos altura/raio usados pelo retarget/corpo. O cabeçalho e os tamanhos do modelo foram preparados. **Não é conversão de toda a entrada de 256 bytes, das tabelas de ações ou das tabelas comuns.** |
| Corpo | 22 partes por modelo, com offsets e cantos locais validados e ligados aos nós. `BtlObjBody_Init`/`Update` originais calculam os volumes. |
| Ataque | Eventos e shapes alimentam `BtlObjAnim_UpdateEvents` e `BtlObjHit_BuildVolumes` originais. O clip 55 gerou volumes ativos em 108 dos 200 passos de teste, em cada modelo. Não há aplicação de dano entre personagens nesta revisão. |
| Poses/matrizes | `BtlObjAnim_SamplePose`, `BtlObjXf_Update` e `BtlObjPose_CalcMatrices` originais executados. Há matrizes de nós que mudam com os frames; não é somente pose de referência. O cálculo é pelo caminho CPU/VU0 portátil, não skinning GPU validado. |
| Referências do renderer | A API fornece VIF canônico limitado, pivôs e matriz mundial atual de cada parte. Recusa matrizes ainda não calculadas ou invalidadas por troca/avanço de animação. Não submete comandos ao GS/GPU. |
| Texturas | 24 imagens por modelo decodificadas para RGBA, com paletas/swizzle tratados pelo leitor existente. Cache próprio com limite de memória; falha não publica cache parcial. Não há upload/materiais/mipmaps completos do caminho original. |
| Facial | 5 geometrias presentes no modelo esquerdo e 6 no direito, validadas e disponíveis por referência própria. **Blink, seleção facial, jaw/lip-sync e sua animação/renderização ainda não foram integrados.** |
| BobjWork | Bloco real de `0x1A0C0`, com duas camadas `0xC000`, flash, contatos e espaço dos volumes. Tamanhos e offsets relevantes verificados em PPC32. |
| Estado/gerenciadores | Há objetos próprios, nós, player e corpo. O registro criado serve à busca de proprietários/nós e ao carregamento de motions; **não é o registro em `gBtlObjTbl`, nas listas originais nem nos gerenciadores de personagens/IA**. Isso continua pendente. |

## Implementação e correções

`xb_battle_fighter.c/.h` agora mantém os recursos de modelo/motions, buffers decodificados, nós, parâmetros selecionados, corpo, work e cache de imagens em memória própria. Os testes liberam os arquivos de origem antes de usar esses dados. A liberação remove o proprietário da busca de nós e libera todos os buffers.

A preparação tem limites para tamanhos de arquivo, contagens e orçamento de texturas. A hierarquia, os IDs de nós, os campos usados dos volumes e seus cantos são validados. Os nós usam o stride nativo de `0xE0`, incluindo espaço da posição anterior; não um array apertado de views de `0xD0`. A distinção é necessária para leitores que usam a view completa do nó.

Duas adaptações explícitas no fonte preparado de `bobj_a.c` consultam o proprietário antes de:

- buscar um nó pelo offset PS2 de 32 bits;
- descomprimir uma motion cuja versão nativa já foi preparada.

Objetos que não pertencem a esse caminho mantêm os acessos originais. Os anchors do patch precisam coincidir com a revisão fixada do upstream. O relatório de compilação identifica essa adaptação.

Os testes de volumes também expuseram problemas de representação do port que foram corrigidos na preparação do fonte:

- `BtlObjBody_UpdateParts` usava um temporário local sem definir o componente homogêneo antes de tratá-lo como posição; agora W é explicitamente 1;
- os argumentos locais passados aos helpers de colisão que exigem vetor alinhado a 16 bytes agora satisfazem esse alinhamento;
- cantos de caixas com três floats não são passados como se fossem vetores alinhados de quatro floats: são copiados para um temporário adequado, sem mudar os valores XYZ.

Isso corrige indefinições e incompatibilidades de tipos/alinhamento no port; não demonstra equivalência de pixels ou física com o PS2 em hardware. O prefixo dos parâmetros também não foi trocado como um float: o leitor de presets usa bytes nesse campo.

## Evidência com recursos reais

| Medida | Modelo esquerdo | Modelo direito |
|---|---:|---:|
| Registros de esqueleto | 69 | 69 |
| Motions não vazias preparadas | 361 | 357 |
| Slots comprimidos vazios | 53 | 57 |
| Motions do próprio modelo presentes/vazias | 1 / 7 | 1 / 7 |
| Partes de corpo ligadas | 22 | 22 |
| Soma de diferenças da matriz do nó 3, frames 0 → 30, motion 0 | 0,137962 | 0,099297 |
| Passos com volume ativo, clip 55 em loop por 200 passos | 108 | 108 |
| Texturas RGBA no cache | 24 | 24 |
| Geometrias faciais presentes | 5 | 6 |

Os deltas de matriz são uma verificação de que o fluxo produz animação; não são uma medida de erro em relação ao console. O teste de ataque verifica geração de shapes, não acerto/dano, IA ou estabilidade de uma partida.

Foram feitos dois tipos de teste, com escopos diferentes:

- **Host x86-64, ASan + UBSan:** construção, vida útil, bounds, camadas, poses que usam a view C correspondente e cache de texturas/geometrias. As rotinas posteriores que assumem layouts originais de ponteiro de 32 bits não são validadas nessa execução.
- **Host i386, UBSan/bounds, via QEMU-i386:** views com ponteiros de 32 bits, retarget entre os dois proprietários, matrizes e volumes pelas rotinas originais. ASan não pôde ser usado nesse caminho porque seu shadow range conflitou com os mapeamentos do QEMU; não foi reportado como teste ASan aprovado.

QEMU-i386 executou um programa de teste Linux/i386. **Não executou o XEX, PowerPC, Xbox ou Xenia.** Os testes 32-bit usam o player e leitores originais preparados, os modelos reais e a biblioteca VU0 portátil. O fallback BPE legado do fixture aborta se for acionado: não simula decodificação bem-sucedida. Em produção, o fallback real continua existente para os objetos fora da preparação própria.

## Novo default.xex

O startup `typed-data` mantém as etapas da D3 e acrescenta, para cada modelo: reprodução de motion, amostragem, matrizes, volumes, cache de texturas e consulta de referências de malha/facial. Ao final, executa um warm-up de retarget entre os dois proprietários. Os resultados vão para `default.log`.

A interface continua no diagnóstico. Não há personagens animados desenhados por esse novo caminho, controles de uma luta nem criação dos gerenciadores. A colisão do cenário continua instalada com NOT_READY. `Battle_Main` continua bloqueado antes de acessar o estado.

- Base: 398/398 unidades PowerPC. Vínculo com catálogo: 399/399; zero símbolos indefinidos e zero conflitos de ABI.
- Catálogo nativo: `complete=true`, 33 regiões e 2.738 operações. As exclusões auditadas de pools originais da D3 continuam distintas de dados tipados.
- Todas as operações e 1.779 slots de ponteiro foram conferidos no ELF final; 1.473 ligações mudaram e foram realocadas simbolicamente.
- Verificação estática de XEX2, ABI e guarda de `Battle_Main`: PASS. Marcador de prontidão permanece 2.
- Testes do carregador, isolamento de bibliotecas e recusa de novas referências aos pools excluídos: PASS.
- Nenhuma execução desta versão no Xbox/Xenia ou validação de GPU, desempenho de partida, áudio de luta ou rollback.

SHA-256 do novo `default.xex`:
`51e0b9f9a14ab35ad2f5e4894ce56f654fd8bdc4b58ae0404b944a0cb0fdb801`

Upstream e SDK continuam fixados nos pins das revisões anteriores: `3ca419c7966b11438e2dc3c9a7e1914f102ed545` e `57b02f1e60667e37d22c3aa613d23d2486514ff8`. GCC 15.3.0 e ABI xsdk. O fonte do adaptador distribuído foi comparado ao hash do input da compilação final.

## Bloqueios restantes para concluir a lista

Faltam converter e ligar as tabelas completas de personagem/ações, as tabelas comuns e os outros consumidores dos recursos. As entradas de modelo 0x12/0x13 e outras contêm estruturas mistas; preparar altura/raio não autoriza tratar o restante como palavras de 32 bits. Os novos bridges não substituem todos os binders originais.

Ainda faltam a ligação das referências com o renderer e seus materiais/estados, o fluxo facial completo, o estado do personagem e seu registro nas listas/tabelas e gerenciadores originais. `BtlObj_Setup/CreateChara` também inicializa recursos, chains, face, alpha/material e outras referências ainda não preparadas aqui. Publicar o objeto nas listas agora permitiria que esses leitores consumissem tabelas incompletas. A guarda não foi retirada e não foram criados retornos de sucesso para esconder essa falta.

Depois disso ainda haverá o cenário completo, gerenciadores e boot da partida fixa. Esta revisão não fornece luta jogável.

## Arquivos e reprodução

Fontes atuais na raiz; evidências em `checkpoint-dados-D4/`; binário anterior em `diagnostico-anterior-D3/`. Patch incremental para uma cópia exata da 0.37-D3 em `extras/`, já aplicado aos fontes da raiz. ISO, programas MIPS e recursos extraídos não fazem parte do pacote.

O alvo `make typed-data-diagnostic` continua reproduzindo o XEX com os caminhos do SDK/toolchain/upstream e a ISO do próprio usuário. O teste ampliado `test_native_fighter.py` recebe modelo, motions e diretório de saída; veja `--help`. O modo `--i386-root` espera headers/libs multilib GCC 13 e glibc da instalação de teste, e `--qemu` aceita o caminho de QEMU-i386. `--other-model`/`--other-motions` ativa o teste de retarget entre proprietários. A configuração de teste usada foi Ubuntu 24.04, GCC 13 e QEMU 8.2.2.
