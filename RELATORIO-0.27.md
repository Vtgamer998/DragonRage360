# 0.27 — caminho nativo de DMA/VIF/VU ligado às entradas gráficas do motor

**O jogo e a luta ainda não rodam. Battle_Main continua fora do XEX.**
Esta versão entrega fontes e um novo default.xex de diagnóstico com o caminho
gráfico abaixo integrado. Nada foi executado em Xbox ou emulador.

## Implementado

- As entradas nativas Port_GsVif1Chain e Port_GsGifChannel usam mapas explícitos
  de bytes canônicos little-endian. O leitor antigo de ponteiros e palavras
  nativas ficou restrito ao perfil de PC. Não foram criadas funções GsVu1_*
  vazias para satisfazer o linker.
- A nova sessão dr_gs_native é compartilhada pelo submit dos produtores DMA e
  pelas entradas do motor. Há um único interpretador VU, preservado entre as
  transferências. Os comandos MPG, UNPACK, MSCAL e XGKICK atravessam o mesmo
  caminho até o GS real e a captura do backend.
- O canal GIF aceita transferências diretas e cadeias com os oito tipos de
  tags DMA, pelo mesmo leitor com limites, CALL/RET e orçamento contra ciclos.
  As palavras TTE não são interpretadas como VIF no canal GIF.
- Os mapas têm limites, validação de sobreposição e duração explícita. Erros
  persistem até fechar/reabrir a sessão. Reentrada por callbacks é rejeitada;
  buffers emprestados não são guardados depois de unbind. A API continua de
  uma única thread e não deve ser misturada com outro dono do VU global.
- sceGsPutDrawEnv serializa os campos u64 nativos do DBuff antes da submissão.
  Isso preserva as alterações feitas pelo jogo na cor e nos cantos do clear,
  sem tratar um ponteiro nativo como endereço DMA. Os ambientes conhecidos
  de 8/14 registros, com/sem clear, são aceitos; pacotes diferentes são recusados.
- O diagnóstico inclui uma sequência sintética MPG/UNPACK/MSCAL/XGKICK que
  altera ALPHA_1, uma transferência GIF que altera o mesmo registro e um
  ambiente/clear de sceGsSetDefDBuff/sceGsSwapDBuff. As entradas usadas pelo
  motor estão efetivamente retidas no ELF/XEX do diagnóstico.

A entrada VIF nativa decodifica e submete a captura ao backend instalado. O
aplicativo ainda é responsável pelo present. Isso não implementa o fluxo de
frames da luta nem o boot original; não basta chamar Battle_Main.

## Evidências

| Verificação | Resultado |
| --- | --- |
| Motor/plataforma compilados em PowerPC | 382/382 unidades |
| Conflitos de ABI | 0 |
| Objetos com aritmética PS2 inspecionados | 294; sem operações FP nativas detectadas |
| Vínculo estrito do combate | Falhou: 156 → 139 nomes pendentes |
| Sessão nativa em host | ASan/UBSan passaram; persistência VU, mapas, canais GIF/VIF e erros |
| Ambiente/clear DBuff | ASan/UBSan passaram; dois buffers, cor alterada, com/sem clear e rejeições |
| Regressão DMA/VIF | ASan/UBSan passaram |
| Produtores DMA | 1.329 comparações contra o original passaram |
| Captura/core/backend completos | ASan/UBSan passaram; nove ciclos; GPU simulada |
| Novo XEX | Formato, símbolos, entradas gráficas e cadeia de hashes passaram |
| Luta, recursos reais neste caminho, pixels Xbox e desempenho | Não testados |

As 17 referências que deixaram de bloquear o vínculo são 13 GsVu1_* do leitor
antigo e quatro dependências do encerramento de frame de PC (MenuText_Poll,
TexPack_Count, gPortNetWindowClose, gPortOverlayOpen). Estas quatro não foram
implementadas: o perfil nativo agora submete pelo backend instalado. Não há
implementação nova de menus/overlays ou prova de equivalência dos efeitos do
encerramento de frame do PC.

O vínculo foi refeito após a atualização final do diagnóstico; o relatório
registra os hashes dos objetos nativos usados. O motor não mudou nessa etapa.
Os logs e a lista dos 139 nomes estão em engine/reports/0.27.

## O que bloqueia o jogo e a luta

Ainda faltam áudio ADXT/IOP/RPC, serviços Port_* e de arquivos, leitura de
imagem do GS e 96 func_00xxxxxx. A maioria desses rótulos está nas tabelas de
dados. Os objetos de tabelas ainda preservam apenas o formato, sem carregar
os valores originais. Não é seguro executar o motor nessas condições.

É necessário adaptar os produtores restantes, suas regiões RAW/W32/W64 e
seus endereços de cadeia. As entradas nativas só aceitam mapas registrados;
não adivinham a extensão de um ponteiro do jogo nem convertem um buffer misto.
A sequência real de modelos, animações e microprogramas da ISO não foi rodada
nesse caminho. Os testes usam dados sintéticos, não personagens comerciais.

Continuam pendentes a inicialização nativa do fluxo original, a adaptação dos
dados/endian, a integração do frame/present da luta, IA/colisões/animações em
execução, saves, áudio, online e validação/desempenho em Xbox. O estado VU desta
sessão não implementa captura/restauração de rollback; Gs_StateKeep ainda não
foi adaptado a ela. O backend e os estados DATE/FBA/stencil não têm pixels
confirmados no hardware.

A saída default.xex desta entrega é somente o diagnóstico 0.27. Battle_Main
foi forçado como raiz apenas no teste separado de vínculo, que falhou e não
gerou um XEX de combate.

## Reproduzir os testes

- engine/tools/test_gs_native.py OUT --sanitize
- engine/tools/test_gs_sony_native.py OUT --sanitize
- engine/tools/test_gs_stream.py OUT --sanitize
- engine/tools/test_capture.py XSDK OUT --sanitize
- engine/tools/test_dma_producer.py UPSTREAM OUT --sanitize
- tests/check_xex.py após make

O vínculo do motor usa engine/tools/link_battle.py, com
--float-mode native-ps2 --platform e o checkout fixado em
3ca419c7966b11438e2dc3c9a7e1914f102ed545. SDK/toolchain/upstream são externos.
ASan foi usado com detect_leaks=0 neste ambiente; isso não testa vazamentos.
