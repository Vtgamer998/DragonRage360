# 0.22 — correções nos consumidores de targets

A lista solicitada NÃO foi concluída. O XEX contém o backend experimental e
as correções abaixo, mas continua sem a simulação original de combate.
Nenhuma luta ou execução de XEX no Xbox/emulador foi realizada.

## Mudanças implementadas

- O shader de leitura de targets usa o MRT auxiliar para recuperar o byte
  de alfa GS. A imagem de cor limita seu alfa a 1; utilizá-la perdia os
  valores 129..255 e o bit imposto por FBA nas passagens seguintes.
- O auxiliar da fonte usa o mesmo UV e sampler da imagem de cor. A cópia
  DATE continua independente no binding 1; o alfa da fonte usa binding 2.
- `targetCopy` agora escreve o alfa bruto da fonte no MRT do destino. Antes
  preservava o auxiliar antigo do destino, apesar de copiar a imagem de cor.
- A captura Xbox aceita leituras CT32 do próprio target, usando o resolve
  já existente na camada nativa. Primitivas que fazem essa leitura não são
  fundidas: cada uma recebe o estado resolvido antes de seu próprio desenho.
- Descritores que declaram textura de target, mas entregam um handle de
  textura comum, são rejeitados antes de desenhar.
- Os fontes duplicados e o patch da captura foram atualizados; os shaders
  foram regenerados e o XEX recompilado com GCC PowerPC/xsdk.

Esse resolve define uma imagem anterior por primitiva. Não foi comparado
com o cache/temporização de texturas do GS original. A correção não conclui
reinterpretações CT16/CT24, channel shuffle, leitura de canais como índices,
blending exato, máscaras por bit ou os demais registradores GS.

## Verificações realizadas

- 1.296 casos do microcódigo do shader de targets: alfa bruto 0..255,
  TFX/TCC, alfa de vértice e ATST, incluindo valores acima de 128. A imagem
  de cor fornece deliberadamente alfa incorreto para verificar a origem.
- 216 casos DATE/FBA, 2.304 seletores de stencil e 27 casos de limite UV
  passaram na simulação numérica de CPU do microcódigo Xenos.
- Captura completa: nove ciclos com ASan/UBSan. Verificados dois sprites
  consecutivos de leitura do próprio alvo, seus descritores separados,
  resolves, binding de alfa bruto e os consumidores/VU existentes.
- Testes de backend, replay e targets passaram com ASan/UBSan e APIs GPU
  simuladas. A detecção de leaks foi desativada. Os testes verificam chamadas,
  estados e propriedade de recursos; não rasterizam pixels de GPU.
- ELF/XEX: formato, símbolos, ABI PowerPC, cabeçalhos e cadeia de hashes
  passaram na verificação estática. `VALIDACAO.json` identifica o binário.

## O que continua impedindo a solicitação completa

`Battle_Main` continua fora do executável. A comparação do objeto original
`battle.c` com o título atual ainda encontra 87 dependências diretas ausentes.
Isso não é a contagem transitiva do motor inteiro. Defini-las artificialmente
ou apenas ligar o objeto não implementaria a luta.

O caminho original `Gfx_EndFrame -> Dma_Flush -> VIF/GIF` ainda não é usado
pelo título. Os produtores originais misturam escritas u32/u64, cópias de
recursos little-endian e endereços PS2; trocar a ordem de bytes de toda a
lista não resolve essa mistura. Faltam adaptar produtores, endereçamento,
heap, inicialização e recursos, além das dependências de simulação.

Continuam pendentes os demais consumidores de buffers, a sequência de
sombras com dados reais, iluminação e VU no combate, demais estados GS,
auras e explosões originais. O limite agregado de 1.024 vértices do caminho
nativo também continua inadequado para um frame completo de luta.

DATE, FBA e stencil NÃO têm pixels validados em hardware. Não há medições
de CPU/GPU/FPS. A ausência de hardware não é o único bloqueio: a integração
e os componentes do motor acima também não estão implementados.

Os diagnósticos existentes continuam sintéticos. O visualizador separado
carrega recursos reais da ISO em pose de referência; não executa combate.
Fontes, shaders, patch e evidências estão incluídos; a ISO não acompanha.
