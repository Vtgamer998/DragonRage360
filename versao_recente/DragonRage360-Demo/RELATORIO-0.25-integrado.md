# 0.25 integrada — DMA e buffers IOP

O patch foi aplicado na base completa e compilado com o xsdk. Há novo
default.xex de diagnóstico. **Battle_Main continua fora do XEX. Nada foi
executado em Xbox ou emulador.**

## Dependências incluídas

O anexo era uma atualização de nove arquivos. Esta entrega inclui a base,
dr_gs_stream.c/.h, dr_vu1.c/.h, test_dma_producer.c, test_dma_producer.py,
Makefile, plataforma, backend e demais fontes. Os testes comparativos ainda
precisam do checkout upstream fixado; SDK e toolchain são externos.

## Verificação

| Verificação | Resultado |
| --- | --- |
| Unidades PowerPC compiladas | 381/381 |
| Rejeições de ABI float | 0 |
| Símbolos sem resolução no motor | 163 → 157 |
| Objetos com adaptação numérica inspecionados | 293; nenhuma aritmética FP nativa encontrada |
| Teste novo contra dma.c original | 368 verificações passaram |
| Regressão de produtores | 1.329 comparações de bytes passaram |
| NEXT/retorno e buffers IOP | ASan/UBSan passaram; BE simulado |
| Captura GS com GPU simulada | ASan/UBSan passaram; nove ciclos de abrir/fechar |
| XEX | Formato, símbolos e hashes estáticos passaram |
| Console, pixels, luta e desempenho | Não executados / não medidos |

Resolvidos no vínculo: Dma_Alloc, Dma_AddData, Dma_AddRef, Dma_BeginDirect,
Dma_EndDirect e Dma_SendToIop. Isso não comprova a adaptação de todos os
chamadores. Não foram geradas funções vazias para satisfazer o linker.

## Alterações

- Os cinco produtores recebidos preservam a classificação RAW/W32/W64 e a
  conversão única. As cópias em src/ e engine/graphics/ são idênticas.
- Alinhamento de 16 bytes garantido. Comparações entre alocações diferentes
  usam endereços inteiros validados. BeginDirect/EndDirect controlam o estado:
  outro Begin, Alloc ou Flush durante um Direct aberto é rejeitado. O chamador
  ainda deve respeitar 64 KiB: EndDirect não impede uma escrita anterior fora
  do limite feita por quem recebeu um ponteiro cru.
- Dma_AddNext usa mapas explícitos. Destino e retTag são verificados; retTag
  deve ser NEXT e não pode misturar stores W64 com atualização u32. Não há
  conversão de ponteiros arbitrários em endereços de transporte.
- O callback recebe todos os mapas. Flush cria snapshots canônicos externos
  sem alterar o conteúdo original, exceto o campo de retorno que AddNext deve
  atualizar. Há 16 mapas e até 8 MiB externos por submissão. São emprestados
  até o retorno síncrono; expiram após Flush bem-sucedido ou Reset.
- O diagnóstico do XEX percorre NEXT → bloco externo → NEXT de retorno →
  FINISH/END. Se executar, registra mapped DMA NEXT/return diagnostic rc.
- xb_iop.c registra propriedade e tamanho dos buffers. Dma_SendToIop copia
  bytes para essa memória, verificando destinos internos e limites. Isso não
  executa IOP, SPU2, RPC ou áudio. Transferências inválidas registram erro.

## Impedimentos restantes

dr_dma_chain_address traduz apenas regiões registradas. Os produtores de
vu1_packet.c que ainda gravam ponteiros por cast precisam usar esse registro.
Dma_AddNext não estava entre os 163 nomes anteriores: não havia chamada C
alcançada a ele, por isso sua implementação não reduz a contagem.

Permanecem as diferenças do patch: AddRef copia no ato da chamada; AddData
usa quadwords completos; Direct tem limite de 64 KiB. Origens mutáveis e
stores mistos precisam de adaptação. A auditoria de chamadores é heurística.

Os 157 nomes incluem Dma_PutTexStrips, ADXT, GsVu1, sceSifCallRpc, Port_*,
globais e dados. Restam 96 nomes func_: 95 observados apenas em tabelas e um
chamado por C, func_002967C0, identificado pelo upstream como sceSdRemote,
para transferência/status do áudio. Ele não recebeu uma resposta falsa.

Faltam valores e conversões dos dados reais, relocação de tabelas, produtores
de modelos/efeitos, inicialização do motor e combate integrado. Menus completos,
IA, áudio, vídeos, saves em jogo e online de combate continuam pendentes.
O backend nativo é experimental; DATE/FBA/stencil não têm pixels validados.

## Reprodução

```sh
make XSDK=/caminho/xsdk TOOLCHAIN=/caminho/xsdk-gcc/bin/powerpc-xsdk-elf-
ASAN_OPTIONS=detect_leaks=0 python3 engine/tools/test_dma_native.py /caminho/upstream build/test-native --sanitize
ASAN_OPTIONS=detect_leaks=0 python3 engine/tools/test_dma_producer.py /caminho/upstream build/test-producer --sanitize
python3 engine/tools/test_dma_mapped.py build/test-mapped --sanitize
ASAN_OPTIONS=detect_leaks=0 python3 engine/tools/test_capture.py /caminho/xsdk build/test-capture --sanitize
python3 tests/check_xex.py
```

make battle-link continua falhando nos 157 nomes. Configuração da ABI e dos
cabeçalhos de plugin: RELATORIO-plataforma-0.4.md. Motor fixado em
3ca419c7966b11438e2dc3c9a7e1914f102ed545; SDK
57b02f1e60667e37d22c3aa613d23d2486514ff8; GCC xsdk 15.3.0.

Evidências: engine/reports/0.25/ e VALIDACAO.json. ASan/UBSan são do host;
LeakSanitizer foi desativado por limitação de /proc neste ambiente.
No Xbox com suporte a homebrew, extraia a pasta e abra default.xex pelo
Aurora. Ele continua sendo diagnóstico, sem luta. Não há ISO ou recursos
extraídos do jogo neste pacote.
