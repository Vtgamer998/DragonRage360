# 0.26 — produtor de faixas de textura do motor

Battle_Main continua fora do XEX. A tentativa estrita de vínculo ainda falha;
não há luta jogável nem execução em Xbox ou emulador.

## Alteração concreta

Dma_PutTexStrips agora tem implementação adaptada em engine/combat/dma_tex.c,
incluída pelo construtor de vínculo do combate. É o produtor original de GIF
REGLIST usado nos efeitos de cenário em stg_b.c/stg_c.c, com checagem dos
parâmetros e da região de destino antes da primeira escrita. Não é uma função
vazia. O arquivo original src/sys/dma.c permanece excluído porque contém MMIO
PS2 e produtores substituídos pela camada nativa.

O produtor preserva o ramo de largura inteira, o ramo fracionário, blend,
RGBAQ e o truncamento do número de faixas pela largura UV. O ramo fracionário
preserva inclusive o x0 sem deslocamento do original. Multiplicações inteiras
limitadas substituem os deslocamentos de coordenadas negativas; a aritmética
float passa pelo adaptador PS2 existente, com assinaturas hard-float.

A nova verificação dr_dma_w64_range aceita apenas a região Direct ativa,
alocações W64 ainda não convertidas ou mapas explicitamente registrados como
W64. Rejeita ponteiros arbitrários, desalinhamento e ultrapassagem. Falhas
mantêm o ponteiro do chamador e o conteúdo do destino; o erro é persistente.
As duas cópias de dr_dma_producer.c/.h foram mantidas iguais.

## Verificação

| Verificação | Resultado |
| --- | --- |
| Compilação PowerPC | 382/382 unidades |
| Conflitos de ABI | 0 |
| Símbolos pendentes | 157 → 156; Dma_PutTexStrips resolvido |
| Objetos com aritmética adaptada inspecionados | 294; nenhuma operação FP nativa detectada |
| Testes do produtor de textura | 5.768 casos totais com ASan/UBSan |
| Comparações válidas contra a função original | 5.760 pacotes byte a byte |
| Erros e propriedade de buffers | 8 casos; não escrevem fora do destino autorizado |
| Serialização | LE e stores W64 BE simulados em casos alternados |
| Regressão dos produtores DMA | 1.329 comparações passaram |
| NEXT/retorno e cópias IOP | Teste com ASan/UBSan passou |
| Captura GS/backend | Nove ciclos com GPU simulada passaram |
| XEX | Reconstrução e validação estática passaram; binário inalterado |
| Xbox, emulador, pixels e desempenho | Não testados |

A comparação usa a função original extraída do checkout fixado, compilada
separadamente, com o mesmo passe de aritmética e os helpers PS2 reais. Testa
as larguras inteiras/fracionárias, as duas opções de blend, origens diferentes
e larguras UV não múltiplas de 32. Os testes não usam recursos reais da ISO.
O passe aritmético é testado separadamente na versão anterior; esta comparação
valida os pacotes, não constitui prova independente do passe ou determinismo.

## Limites de integração

A nova função entra apenas na tentativa de vínculo do motor. O diagnóstico
não a chama e o construtor do launcher não a inclui. default.xex continua o
mesmo diagnóstico 0.25, com SHA256
7875d2762bf7d64fd60cee389ea7231cb44afe2d9c98512aae4bffcb0f4e1521.
Nenhum XEX com Battle_Main foi gerado.

Os chamadores ainda precisam respeitar o tipo de cada trecho do pacote.
Um bloco com stores u32 e u64 misturados não pode ser convertido por inteiro
como W64. O adaptador rejeita regiões incompatíveis em vez de adivinhar.
Os limites atuais são os campos GS representáveis nas coordenadas de tela,
UV entre 0 e 4095 e frações entre 0 e 15; isso ainda precisa ser confrontado
com recursos e chamadas reais. A verificação protege esta função, não torna
seguros todos os produtores que recebem ponteiros crus do motor.

Persistem GsVu1_*, áudio ADXT/IOP/RPC, leituras e outros serviços Port_*,
96 func_00xxxxxx e referências de dados. As tabelas de assembly compiladas
só preservam os formatos, sem os valores originais. Mesmo resolver os 156
nomes não bastaria: faltam inicialização nativa, dados/endian, relocação de
endereços e integração com o fluxo original antes de executar Battle_Main.
IA, colisões, animações, online e saves em luta não foram validados.

O próximo trabalho no caminho gráfico é adaptar o consumidor VIF legado
alcançado pelo motor para o interpretador VU/endian nativo, preservando um
único estado VU. Criar wrappers vazios para GsVu1_* não resolveria isso.

## Reproduzir

Use engine/tools/test_dma_tex.py UPSTREAM OUTPUT --sanitize e um GCC host
com os headers de plugin correspondentes (--cc e --plugin-include, como no
teste de ABI float). O checkout requerido é
3ca419c7966b11438e2dc3c9a7e1914f102ed545.
A tentativa PowerPC usa engine/tools/link_battle.py com
--float-mode native-ps2 --platform. Ela termina com erro enquanto houver
nomes pendentes e não modifica o XEX do launcher.

As evidências estão em engine/reports/0.26. SDK, toolchain e checkout
upstream são dependências externas; não estão incluídos no ZIP.
