# D12 — geometria e ciclo original do cenário em CPU

O pedido completo continua pendente. Esta revisão executa o ciclo original do
cenário e prepara suas estruturas gráficas; não executa a partida completa,
Battle_Update/Battle_Main, HUD ou desenho GPU. Os 526 recursos restantes do
comum 3 continuam indisponíveis. Não representa uma luta jogável.

## Implementação

O arquivo real do cenário 0 passa a ter uma cópia própria com descritores
numéricos nativos e offsets validados: sete grupos, 34 materiais, 2.243 malhas,
1.959 partes, 172 nós, 30 células de visibilidade, onze objetos e 25 texturas.
São convertidas as tabelas e estruturas lidas pelo código original; os fluxos
DMA/VIF/GIF e pixels mantêm sua representação canônica. A validação dos fluxos
confere seus limites e o DMA RET externo, sem afirmar interpretação VU ou
desenho dessas malhas. Malhas de colisão nulas recebem uma malha vazia própria,
evita-se interpretar o cabeçalho do cenário como uma malha.

O diagnóstico inicializa File_Init original, três partições, buffers de trabalho
e seis players ADX. Depois executa StgModel_InitStage/BtlStage_Init originais:
relocação, associação de dados, física, reset dos objetos e ambiente sonoro.
A interface atende BtlStage_Update e o serviço de áudio; a finalização chama
BtlStage_Term e libera players, buffers e recursos na ordem inversa.

A prontidão **do cenário** passa a ser produzida pelo inicializador original;
não se limpa NOT_READY manualmente. O marcador **do combate** permanece em 2.
A prontidão do cenário não libera Battle_Main nem declara todos os recursos
internos ou a apresentação concluídos.

O conversor recusa cenários com o membro de objetos animados presente. O cenário
0 usado nos testes não contém esse membro nem temporizadores. Os nós e suas
chaves são preparados, mas isso não prova reprodução de todos os formatos de
animação dos demais cenários. A animação de textura não foi exercitada por esse
cenário. Ambiente gráfico, cena e efeitos continuam dependentes do restante do
boot e do produtor de pacotes.

## Correção importante dos testes anteriores

O carregador tipado escrevia valores em ordem PowerPC mesmo no teste i386. Agora
escreve os campos e ponteiros na ordem nativa da CPU, preservando a representação
PowerPC no Xbox. Os testes com consumidores originais foram repetidos com essa
correção; os resultados antigos do host não devem ser usados como evidência de
que todos os valores tipados eram interpretados corretamente. O carregador BT3X
legado não foi alterado.

Foram corrigidos também locais de consulta de chão e a origem da esfera na
física para cumprir o contrato de alinhamento de 16 bytes das rotinas de
colisão. As adaptações exigem correspondência exata com o upstream fixado.

## Execução e limites da evidência

Teste i386 sob QEMU com UBSan/bounds, programas e recursos originais, leitor de
ISO da plataforma e transporte de arquivo host. A ISO de teste é esparsa: os
setores necessários são reais; isso não testa todos os setores do disco. Os
arquivos reais não acompanham a entrega.

- Dois ciclos completos de BtlObj/BtlChar/BtlAi, cenário, navegação, áudio e
  encerramento, com heap do motor restaurado em cada ciclo.
- Relocação, inicialização, reset, atualizações e finalização originais do
  cenário. BtlStage_GetStartPlace original encontrou a zona 44.
- Busca original de uma rota adjacente: nós 0 → 1, três pontos.
- Corpo físico ativo criado, lançado, atualizado e liberado pelo código
  original: y de -100 a aproximadamente -90,489632 em doze atualizações.
  Esse teste não prova colisão de detritos contra triângulos do cenário.
- Culling original na CPU marcou 983 malhas visíveis para a câmera de teste.
  Não houve submissão GPU dessas malhas.
- Temporizador original: avanço e término exercitados com um exemplo sintético
  não vazio, identificado separadamente do cenário real, cujo total é zero.
- Sete corrupções de parâmetros/caminhos/navegação e cinco corrupções de
  geometria, ponteiros, malha, chaves e ciclo da árvore recusadas.
- Reinício do cenário/players mantendo o dispositivo de áudio aberto, seguido
  de encerramento completo. O descarte ADX agora libera também o slot do player.
- As vinte seções MAPD continuam comparadas ao binder original. O consumidor
  original das 111 texturas/154 imagens do comum 3 continua verificado.
- 192 blocos de áudio, 98.288 valores PCM não nulos, finitos e limitados; os
  demais canais ficam zerados. Inclui o stream real do ambiente do cenário.
  O dispositivo SDK é um coletor PCM simulado: não comprova saída audível.

As exceções anteriores para aritmética de bits da biblioteca newlib permanecem
limitadas ao código de terceiros. Não houve execução PowerPC, Xbox ou Xenia.
Resultados de compilação e integridade do novo XEX ficam na validação JSON e
em checkpoint-D12-final; não devem ser confundidos com execução no console.

## Pendências do pedido

| Item | Estado nesta revisão |
|---|---|
| Cenário gráfico completo | Estruturas do cenário 0 preparadas e código de CPU executado; animações de outros cenários, todos os recursos internos e desenho ainda pendentes. |
| Comum 3 | 111 recursos/154 imagens preparados; 526 recursos indisponíveis, sem avanço nesta revisão. |
| Battle_Load/Battle_Init | Ciclo completo pendente. Inicialização do cenário não equivale ao boot integral. |
| Battle_Update/Battle_Main | Não executados. Marcador 2 preservado. |
| HUD, cena, auras e efeitos | Integração completa pendente. |
| Skinning/pixels/mandíbula/lábios na GPU | Não comprovados. Os modelos anteriores não têm mandíbula animada. |
| Vozes e ambiente sonoro | Mixer/stream executados em CPU; saída física e sincronização facial pendentes. |

## Reprodução

Upstream 3ca419c7966b11438e2dc3c9a7e1914f102ed545; SDK
57b02f1e60667e37d22c3aa613d23d2486514ff8. Veja BUILD.md e TESTAR-PRIMEIRO.md.
O teste test_native_managers.py mantém a ordem de argumentos anterior e aceita
--iso /caminho/da/ISO-USA-local.iso. Por padrão procura stage-fixture.iso ao lado
da pasta de recursos. A imagem pode ser a ISO completa original. Ela deve
conter os diretórios ISO, tabelas AFS e recursos usados, incluindo o ambiente
AFS2/65116.bin. O teste não substitui dados ausentes por recursos fictícios.

## Binário e verificações finais

Novo default.xex D12: 13770752 bytes; SHA-256 e81885660384fe3788e37f8353223bb16eaa1fdaebba1583e67c666aee9bb139.
405/405 unidades PowerPC compiladas; zero símbolos pendentes e conflitos ABI.
XEX2/PE, integridade e 33 regiões/2.738 operações do catálogo final: PASS.
Isolamento das tabelas antigas: PASS (103 nomes originais não implementados
continuam recusados; o probe Game_Main mantém 69 nomes ausentes). O XEX D11
anterior foi preservado em diagnostico-anterior-D11/default.xex.
