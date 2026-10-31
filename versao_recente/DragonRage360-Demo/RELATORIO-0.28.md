# 0.28 — carregamento verificado das tabelas do motor

**O jogo e a luta continuam sem execução. Battle_Main está fora do XEX.**
Os fontes avançaram no carregador de dados, uma etapa necessária antes do boot
do motor. Não houve redução das 139 referências pendentes nesta atualização.
O default.xex incluído é o diagnóstico 0.27, sem alteração de bytes.

## Problemas corrigidos

O carregador anterior aceitava destinos indicados pelo arquivo BT3X e escrevia
nas tabelas antes de conferir o checksum. Um checksum inválido retornava erro
quando a memória já estava modificada. A extração de seções do ELF também só
verificava parte do cabeçalho e podia confundir lacunas com dados válidos.

Agora XbGameData_Load prepara e valida todos os registros antes de gravar:

- As regiões DATA nativas precisam ser registradas por XbGameData_SetTargets.
  O manifesto contém endereços lógicos de 32 bits, mapeados nessas regiões;
  ele não fornece um ponteiro nativo que o carregador desreferencia livremente.
- Há verificação das regiões lógicas e físicas, sobreposição, limites, bias e
  alinhamento por largura. Uma atualização inválida dos mapas preserva a anterior.
- O leitor exige ELF32 little-endian MIPS executável e os campos/tamanhos de
  cabeçalho necessários para ler as seções. Seções alocadas sobrepostas,
  truncadas, com wrap ou que cruzem os limites da imagem são recusadas.
  A cobertura das seções é registrada: uma lacuna não conta como bytes do disco.
- Todos os offsets, comprimentos e larguras 1/2/4/8 são validados. Registros
  sobrepostos são rejeitados depois de ordenar os destinos, evitando a
  comparação quadrática de todos os pares de registros.
- O checksum de todos os bytes de origem é calculado antes da escrita.
  Quando allowModified=0, um erro deixa os destinos intactos. A opção explícita
  allowModified mantém a possibilidade de aceitar uma origem modificada.
- Os registros preparados são privados: o destino pode até sobrepor a memória
  que continha o manifesto, sem alterar os endereços dos próximos registros.
- A API é de uma única thread. Reentrada no carregador e mudanças dos mapas
  dentro do callback de leitura são recusadas. Os buffers retornados por read
  continuam tendo de ser alocações próprias liberáveis por free, como no contrato.
- Leituras completas dos executáveis no console têm limite de tamanho.

As cópias continuam produzindo bytes big-endian por unidade de largura, sem
operações float. Não são um relocador de ponteiros MIPS nem um conversor de
estruturas arbitrárias. O manifesto deve identificar os campos escalares e
preservar os campos que o linker já resolveu para endereços PowerPC.

## Ferramenta antiga de retirar dados

xb_strip_data.py agora aborta antes de produzir uma versão se não encontrar
objetos/runs de dados completos ou se algum objeto não corresponder às fontes.
Antes, o nome atual dos objetos podia fazê-la encontrar zero tabelas, marcar o
programa como dependente de um .dat e emitir uma lista vazia.

O teste usou o ELF/mapa reais do diagnóstico e binutils PowerPC, com arquivos
PS2 sintéticos, para confirmar essa recusa sem criar .dat ou executável de
release. O teste do matcher foi corrigido para exigir as quatro larguras e a
reconstrução de todos os bytes; havia uma asserção que sempre passava.

Essa proteção não torna o matcher um gerador semântico de layouts. Ele compara
bytes de um executável que já contenha os valores; não reconstrói os valores
que estão ausentes dos objetos skeleton atuais. Ainda falta gerar um layout
explícito de tipos/relocações para a compilação real do motor.

## Verificação

| Verificação | Resultado |
| --- | --- |
| Motor/plataforma PowerPC | 382/382 unidades compiladas |
| Conflitos de ABI | 0 |
| Objetos com aritmética PS2 inspecionados | 294, sem operações FP nativas detectadas |
| Vínculo estrito Battle_Main | Falhou, 139 nomes pendentes; lista inalterada |
| Carregador de dados | 546 verificações em host com ASan/UBSan passaram |
| Match/reconstrução de dados | Larguras 1/2/4/8 e todos os bytes passaram |
| Recusa de pacote vazio | ELF/mapa/binutils reais; nenhuma release foi emitida |
| ADXF, conversores, BPE e adaptadores | Regressões em host com ASan/UBSan passaram |
| XEX | Validação estática passou; binário 0.27 inalterado |
| Dados reais da ISO, Xbox, emulador e luta | Não executados nesta etapa |

Os casos incluem erro no último registro sem escrita parcial, falha de origem,
checksum incorreto, mapas inválidos, bias, lacunas e sobreposições de ELF,
recursão e manifesto sobrescrito pelo próprio commit. Os 512 casos finais
variam offsets inválidos de origem. Isso não representa 546 cenários de jogo.
ASan foi usado com detect_leaks=0; vazamentos não foram medidos.

## Integração que falta

Não há ainda um boot que registre os destinos e chame o novo carregador antes
do fluxo original. O módulo compila no teste do motor, mas é descartado por GC
do diagnóstico porque ele não o chama. Nenhum carregamento real foi acrescentado
ao default.xex e nenhum novo XEX com combate foi produzido.

Para integrá-lo, o chamador deve registrar somente as regiões de dados de sua
própria compilação, fornecer o manifesto dessa mesma compilação e abortar o
boot quando XbGameData_LoadFromDisc não retornar XBGD_OK. Um checksum correto
valida os bytes listados, não comprova que todas as tabelas necessárias foram
listadas, nem autentica o manifesto. A definição dos tipos e a cobertura
completa do jogo continuam pendentes.

Os nomes dvCi*, mfCi* e htCi* encontrados nesta análise estão em tabelas da
biblioteca CRI, sem implementação C no checkout. Não foram criadas respostas
vazias para resolvê-los. Permanecem os serviços de áudio/IOP/RPC, serviços
Port_*, leitura de imagem GS e os rótulos de funções das tabelas apontados no
relatório de vínculo. As tabelas compiladas ainda são skeletons sem os valores
originais. A inicialização, os dados/endian, os produtores de pacotes restantes
e o loop integrado de IA, colisão, animação e present continuam necessários.
Não houve validação de pixels DATE/FBA/stencil nem teste de desempenho no Xbox.

## Reproduzir

- engine/tools/test_gamedata.py OUT --sanitize
- engine/platform/tools/test_xb_strip.py
- engine/tools/test_strip_refusal.py NATIVE_ELF NATIVE_MAP PREFIX OUT
- engine/tools/test_platform.py XSDK OUT --sanitize
- engine/tools/link_battle.py UPSTREAM CC XSDK OUT --float-mode native-ps2 --platform
- tests/check_xex.py depois de make

SDK, toolchain e checkout upstream são externos. O checkout exigido continua
3ca419c7966b11438e2dc3c9a7e1914f102ed545. Evidências em engine/reports/0.28.
