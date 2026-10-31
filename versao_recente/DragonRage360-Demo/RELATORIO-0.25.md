# Documento original do patch recebido

Integracao posterior, recompilacao xsdk e extensoes:
RELATORIO-0.25-integrado.md. O relato abaixo e historico.

--- Historico ---

# 0.25 — Alloc/AddData/AddRef/BeginDirect/EndDirect adaptados; combate ainda ausente

**O port não está completo. Battle_Main continua fora do XEX e nada foi executado
em Xbox ou emulador.** Este passo ataca os cinco símbolos DMA que mais bloqueavam
o vínculo (referências na última tentativa: Dma_AddData 58, Dma_EndDirect 49,
Dma_BeginDirect 48, Dma_AddRef 28, Dma_Alloc 14).

## O problema real
Esses produtores devolvem um ponteiro e o *chamador* escreve o pacote com stores
nativos. No PS2 (LE) isso já é o formato do GIF/VIF; no Xbox 360 (BE) os bytes
saem invertidos. Não dá para adivinhar a largura do store, então cada região
registra **como** foi escrita e é convertida uma única vez em Dma_Flush
(ou em `dr_dma_producer_view`):

| Tipo | Quando | Padrão de |
| --- | --- | --- |
| RAW | bytes já LE (código de microprograma, dados de arquivo) | Dma_AddRef |
| W32 | u32/f32 nativos (pacotes VU1, arrays `u32 pkt[]`) | Dma_Alloc, Dma_AddData |
| W64 | u64 nativos (GIF tag, pares A+D, XYZ/UV) | Dma_BeginDirect/EndDirect |

Variantes explícitas: `dr_dma_alloc_kind`, `dr_dma_add_data_kind`, `dr_dma_add_ref_kind`.
Região com stores u32 e u64 misturados NÃO é convertível; o chamador tem de ser
dividido/reescrito.

## Diferenças em relação ao original (deliberadas)
- `Dma_AddRef` **copia** os bytes atrás de uma tag CNT no momento da chamada. O
  original gravava um endereço PS2 e o dado era lido depois. O fluxo VIF/GS é o
  mesmo se a origem não mudar antes do Flush. Origens grandes podem estourar o
  buffer de 1 MiB (erro persistente DR_DMA_FULL).
- `Dma_BeginDirect` exige 64 KiB livres; `Dma_EndDirect` rejeita ponteiros fora
  de [tag+32, tag+16+64 KiB]. Em falha, escritas do chamador vão para um *sink*
  (sem corromper memória) e o erro fica persistente.
- Dma_AddNext continua sem adaptação (precisa de mapa de endereços de cadeia).
  Endereços de cadeia embutidos por chamadores (`chain & VU1_ADDR_MASK` em
  vu1_packet.c) também ainda não têm tradução.

## Evidências (host x86-64, LE)
- `engine/tools/test_dma_native.py`: 368 verificações, ASan/UBSan, contra as
  funções originais de `src/sys/dma.c` (hash fixado) compiladas à parte e
  renomeadas: Alloc (várias larguras, tamanho 0 e 15), Direct (vazio e 1–150
  pares), AddData (múltiplos de 16 e truncados), sequência misturada com
  produtores antigos, conversão única na view/Flush, limites e erros.
- Host BE **simulado**: swap forçado + stores byte-invertidos. Não é execução em PowerPC.
- Regressão: `test_dma_producer.py` continua com 1.329 comparações.
- `gcc -Wall -Wextra -Werror` limpo. **Não recompilado com o toolchain xsdk**
  (indisponível neste ambiente); o XEX não foi regerado.

## Auditoria dos chamadores (`engine/tools/audit_dma_callers.py`, heurística)
169 pontos de chamada no upstream: Direct 54 "só u64?", 9 REVIEW (stores u32/u8/.w[]
em eft_c, eft_d_b, gfxm_a, gfxm_b, gfxm_d_c) e 1 sem End visível (eft_d_b.c:758);
Alloc 16 "só u32?"; AddData 63 e AddRef 26 exigem conferir o tipo/endianness da
origem. Regex sobre texto, sem fluxo de dados: não é prova.

## Efeito esperado no link (NÃO verificado)
Cinco dos 163 nomes pendentes passam a ter definição (esperado ~158). Os outros
bloqueios continuam: ADXT_*, Dma_PutTexStrips, Dma_SendToIop, GsVu1_*, sceSifCallRpc,
Port_*, ~110 `func_00xxxxxx` e tabelas.
