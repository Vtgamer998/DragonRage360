# 0.31 — leitura de arquivos e inicialização do produtor DMA

O vínculo completo solicitado continua incompleto. Esta etapa implementa sete
referências reais da inicialização e leva o diagnóstico para 228 → 221 nomes
pendentes. Battle_Main e Game_Main continuam fora do default.xex.
O executável entregue é um novo diagnóstico 0.31, não executa o jogo.

| Raízes do vínculo estrito | Unidades PowerPC | ABI float | Pendências |
| --- | ---: | --- | ---: |
| Battle_Main | 384/384 | Sem conflitos | 133 |
| Battle_Main + Game_Main | Mesmos objetos | Sem conflitos | 221 |

As dependências adicionais alcançadas por Game_Main passam de 95 para 88.
O motor/plataforma foi compilado nesta etapa com ambas as raízes; os objetos
foram reutilizados no vínculo de comparação e após a correção final de posse
do produtor DMA. Os hashes dos objetos nativos constam nos relatórios.
A inspeção dos 294 objetos de aritmética rebaixada não encontrou aritmética
float nativa. Não houve supressão da verificação de ABI ou geração de stubs.

## Cinco chamadas de arquivo implementadas

sceOpen, sceRead, sceLseek, sceClose e sceCdSearchFile usam a ISO montada por
xb_disc.c e o parser ISO9660 real dr_disc.c. Os quatro primeiros operam em
bytes, separadamente da leitura em setores da API ADXF já existente.

Abertura aceita somente leitura e caminhos de disco, normalizando cdrom0:,
barras e a versão ;1. Não aceita caminhos de host, diretórios ou arquivos
maiores que INT32_MAX, pois a ABI de busca/retorno é assinada de 32 bits.
Mantém até 32 descritores, com tokens que não são ponteiros nem descritores
do sistema. Tokens fechados ou de outra montagem são recusados. A geração
da montagem muda inclusive em tentativas de remonte malsucedidas.

Leituras usam um buffer temporário de até 64 KiB, devolvem a quantidade real
lida e não acrescentam padding. EOF retorna zero. Falha antes do primeiro
bloco preserva posição e destino; falha posterior devolve os blocos completos
já copiados. Buscas validam sinal, origem e estouro antes de alterar a posição.
As APIs e a montagem continuam com um único dono/thread; não são sincronizadas
para uma futura thread de áudio.

sceCdSearchFile retorna LSN, tamanho e nome no layout público sceCdlFILE de
32 bytes, com stores nativos seguros para destinos desalinhados. O campo
date é indisponível e fica zerado; o único consumidor original identificado,
Disc_Identify, usa apenas o resultado de localização. Isso não implementa
todos os metadados nem os serviços de libcdvd.

O diagnóstico da plataforma chama as cinco entradas, encontra SLUS_216.78 e
verifica seu cabeçalho ELF32 little-endian/MIPS sem carregar código executável.
O log identifica esse teste como `raw ISO open/read/seek/search USA ELF probe`.

## Inicialização DMA e pacote gráfico de boot

Dma_InitBuffers abre o produtor nativo limitado, com dois buffers de lista e
o callback de submissão que liga o produtor à sessão GS/VU compartilhada.
Exige que essa sessão já exista. Repetições preservam comandos enfileirados;
um produtor de outro dono é recusado e permanece intacto. O fechamento libera
somente os buffers que este módulo abriu. Isso substitui as alocações de
display lists, não inicializa os antigos globais PS2 gDmaBuf/gDmaCur.

Dma_SendGif aceita somente o intervalo virtual temporário registrado pelo
adaptador tipado dr_dma_engine_send_u64. O único chamador original encontrado
é o pacote u64 de Gfx_Init: engine/combat/gfx.c adapta exatamente esse local.
Os u64 são serializados como little-endian, inclusive quando a fonte está
desalinhada, antes da transferência GIF limitada. Não há cast de ponteiro
nativo para um endereço PS2 inferido.

O diagnóstico usa essas mesmas entradas e deixou de ter um callback privado
duplicado para abrir o produtor. As novas rotinas são retidas e verificadas
no ELF/XEX do diagnóstico; isso não liga a simulação do combate ao renderizador.
Dma_InitBuffers tem a assinatura void original; o boot nativo ainda precisará
verificar dr_dma_engine_error antes de avançar. O diagnóstico já o verifica.

## Testes realizados

A ISO fornecida foi recuperada novamente e conferiu com os 3.020.816.384 bytes
e SHA-1 a1c4d21c154da8b1d4b15055907edb0abb055e63 validados anteriormente.
O teste de arquivos compila xb_iso_file.c, xb_disc.c e dr_disc.c reais;
apenas NtCreateFile/NtReadFile/NtQueryInformationFile/NtClose são transportados
para stdio no host. Asserções cobrem EOF, canários, falha de IO que escreve
no buffer temporário, buscas inválidas, capacidade, nomes/flags, descritores
fechados e obsoletos e remonte. O ELF e DBZP.BIN lidos por essas APIs conferiram
via SHA-256 com os hashes da extração independente anterior.

O teste completo de captura/GS/backend passou com o novo módulo DMA e GPU
simulada. Verifica init repetida sem apagar comandos, posse de um produtor
externo, u64 desalinhados, envio real ao GS compartilhado, recusa de endereço
não registrado e argumentos inválidos, fechamento e reabertura. As regressões
de leitura RGB, VU, texturas e consumidores continuaram passando.
Também passaram as 1.841 comparações dos produtores com o código original.
ASan/UBSan foram usados com detect_leaks=0; não é uma medição de vazamentos.

A validação estática do novo XEX confirmou as sete entradas, formato PowerPCBE,
ABI, ausência de símbolos fortes indefinidos no diagnóstico e hashes da imagem.
Não houve execução PowerPC, Xbox, emulador, medição de FPS ou validação de pixels.

## O que impede atender ao pedido completo

Os 221 nomes não são todos funções: incluem globais e referências em tabelas.
A auditoria dos objetos encontrou 118 nomes com referência em código e 103
somente em dados, incluindo seções depois descartadas por GC. Isso não prova
desuso em execução; nenhuma tabela ou callback foi removido para baixar a conta.

Continuam pendentes as 13 entradas ADXT e nove sceMpeg*, RPC/IOP, resets GS,
serviços de interface/menus/rede e rótulos func_ usados pelo motor e tabelas.
A lista exata e cada referência estão em engine/reports/0.31/boot.

Há ainda um bloqueio além dos símbolos: Overlay_Load original lê DBZP.BIN
para MainMenu_StartBallLoops, que no port é uma função PowerPC. Copiar o overlay
MIPS para esse destino corromperia código nativo. Os testes desta etapa extraem
esses bytes para arquivos de teste; nunca os copiam para uma função nem os
executam. O carregamento nativo de tabelas, relocações e dados do overlay precisa
substituir esse fluxo, e o manifesto tipado completo ainda não existe.

O Game_Main original também depende do boot PS2, dados de tabelas incompletos,
animações, IA, colisões e produtores de pacotes do combate ainda não integrados.
Áudio, vídeo e interface completos não foram implementados nesta etapa. Um
linker sem pendências, por si só, ainda não seria uma luta executável.

## Reprodução

Upstream: 3ca419c7966b11438e2dc3c9a7e1914f102ed545.
xsdk: 57b02f1e60667e37d22c3aa613d23d2486514ff8.
Toolchain: powerpc-xsdk-elf GCC 15.3.0 com o plugin de aritmética do projeto.

```sh
python3 engine/tools/test_iso_files.py XSDK ISO OUT --sanitize
python3 engine/tools/test_capture.py XSDK OUT --sanitize
python3 engine/tools/test_dma_producer.py UPSTREAM OUT --sanitize
python3 engine/tools/link_battle.py UPSTREAM CC XSDK OUT --float-mode native-ps2 --platform --root Game_Main
python3 engine/tools/audit_engine_refs.py OUT AUDIT_OUT
```

O teste de arquivos requer a ISO USA validada. Binários e recursos do jogo não
fazem parte do pacote distribuído. extras/port-0.31.patch contém as alterações
textuais desde 0.30, incluindo o Makefile; aplicação de teste passou.
