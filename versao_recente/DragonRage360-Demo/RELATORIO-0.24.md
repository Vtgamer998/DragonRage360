# 0.24 — produtores DMA adaptados; combate ainda ausente

**A solicitação de integrar o combate não foi concluída.** Esta entrega
adapta produtores concretos de `src/sys/dma.c`, incluindo seu transporte de
listas. `Battle_Main` continua fora do XEX. IA, colisões e animações de
combate continuam sem integração e não houve luta executada.

## Implementado

`src/dr_dma_producer.c` adapta o subconjunto abaixo da revisão upstream
3ca419c7966b11438e2dc3c9a7e1914f102ed545. O módulo usa stores LE explícitos,
sem casts de bytes de arquivo para palavras nativas e sem MMIO do PS2.

- Registradores: Dma_AddTexFlush, AddFrame, AddClamp, AddScissor,
  AddXyOffset, AddZbuf, AddTex0, AddTex1, AddFba, AddColClamp e AddAlpha.
- Retângulos: Dma_AddFillRect preserva os sprites de 32 pixels, os
  registradores e o formato GIF REGLIST do produtor original. Preserva
  floor(largura/32), inclusive larguras não múltiplas e zero faixas.
- Dma_Flush acrescenta o pacote GS FINISH e a tag END, chama o consumidor
  configurado e alterna os dois buffers somente quando há sucesso.
- Dma_ResetBuffers descarta os bytes enfileirados. Há dois buffers de 1 MiB;
  64 bytes em cada um ficam reservados para fechar a lista com segurança.
- Cada produtor valida seus argumentos e reserva todo o pacote antes de
  escrever. Falhas de capacidade, argumentos ou submissão são persistentes.
  ResetBuffers não limpa uma falha; fechar/abrir é uma decisão do chamador.
- APIs separadas recebem palavras u32 nativas, DMA já canônico ou pares
  registrador/valor. Não existe detecção automática de formato misturado.
- Os endereços privados de cadeia são 0x41000000/0x41100000 e só são usados
  no mapa explícito. Não são ponteiros nativos nem um mapa de memória PS2.

## Caminho que entra no XEX

O diagnóstico de captura agora configura Frame/Zbuf/Scissor/XYOffset pelos
produtores adaptados. Seus outros registradores usam o produtor AD explícito.
Dma_Flush fecha e entrega essas listas ao consumidor DMA/VIF e ao GS original;
a submissão dos desenhos e a apresentação continuam no loop do diagnóstico.

O ELF verifica a presença efetiva desses produtores e de Dma_Flush/ResetBuffers.
Os demais helpers têm fontes e testes, mas os que não são usados pelo título
podem ser eliminados por --gc-sections. A presença de dois símbolos antes
ausentes não significa que o Battle_Main esteja ligado ou que a plataforma
de combate tenha sido substituída como conjunto.

## Testes e evidências

- 1.329 comparações de bytes com funções do dma.c original, compiladas
  separadamente e renomeadas. O hash do arquivo original é verificado pelo
  teste. Foram comparados valores de 64 bits, máscaras, registradores,
  retângulos de vários tamanhos e uma sequência concatenada de produtores.
- Teste de submissão real no consumidor da 0.23: FINISH/END, alternância
  A/B dos buffers, falha de callback sem reutilização silenciosa, capacidade,
  argumentos inválidos e serialização tipada. Passou com ASan/UBSan.
- As declarações do header nativo foram compiladas junto de sys/dma.h
  original no PowerPC xsdk, com verificação dos tamanhos de tipos/ponteiros.
- A captura GS completa passou novamente em nove ciclos ASan/UBSan, agora
  passando pelos produtores e por Dma_Flush. Os caminhos VU/DATE/FBA/stencil,
  contorno/CLUT e feedback existentes continuam nos testes. As APIs GPU
  são simuladas; não há rasterização ou pixels de Xbox nesses testes.
- O XEX foi recompilado para PowerPC. A validação estática verifica os novos
  símbolos, ABI, formato PE/XEX e cadeia de hashes. VALIDACAO.json identifica
  o binário. Os logs e resultados estão em engine/reports/0.24/.

Os testes de corpo/materiais reais da 0.23 não foram repetidos: o pipeline do
visualizador não foi alterado nesta versão. Seus resultados históricos não
são testes de combate nem de hardware.

## O que falta para o pedido do combate

A auditoria do objeto original battle.c passou de 87 para 85 dependências
DIRETAS ausentes no título, por Dma_Flush e Dma_ResetBuffers terem uma
implementação usada no XEX. Não é uma contagem transitiva, nem uma medida
de porcentagem do port. Battle_Main segue ausente.

Ainda precisam ser integrados carregamento, estado da partida, personagens,
câmera, cenário, IA, colisões, animações e serviços do motor. As estruturas
e dados reais desses módulos exigem adaptação de endianness, ponteiros,
heap e memória PS2. Os patches separados de animação não entram no combate
só porque o transporte de registradores foi adaptado.

Também faltam os produtores de modelos/VU, tabelas, ordenação, efeitos e
recursos referenciados. Em particular, os caminhos Dma_AddData/BeginDirect/
EndDirect/Alloc/Ref/Next/PutTexStrips, buffers emprestados e envio à IOP não
foram fornecidos por este módulo. Os callers originais que os usam continuam
sem adaptação. Não compile o dma.c original junto com o módulo novo para
tentar preencher essas funções: isso duplicaria símbolos e manteria MMIO
PS2 e misturas nativas u32/u64 incompatíveis.

O Gfx_EndFrame original e a simulação ainda não chamam o novo transporte.
O callback configurado pelo diagnóstico não fornece pacing, renderização
de fim de frame ou serviços para uma luta. O backend GS completo e o limite
de 1.024 vértices do replay nativo também continuam pendentes para o combate.

Nenhum XEX foi executado no Xbox/emulador. DATE, FBA e stencil continuam sem
pixels validados em hardware. Não há FPS medido, partida completa, IA ou
colisão validadas em jogo. A ausência de hardware não é o único bloqueio.

A ISO não acompanha o pacote. Todos os comandos do diagnóstico continuam
sintéticos. O visualizador separado continua em pose de referência.
