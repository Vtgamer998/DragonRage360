# Atualização de diagnóstico 0.33-HW1

Leia TESTE-AUTOMATICO-HW1.md primeiro. Novo default.xex com teste gráfico
configurado para abrir automaticamente. Ainda não testado em hardware.
Modelo pode ser escolhido com startup_test.txt contendo model. Luta ausente.
Os relatórios e patches abaixo são históricos e têm escopos diferentes.

# Atualização 0.33 — decodificador de vídeo PSS

O novo decoder nativo lê os dois vídeos completos pela ISO: 7.010 quadros
passaram em host com ASan/UBSan. Os primeiros 32 de cada vídeo foram comparados
com FFmpeg. 389/389 unidades PowerPC compilaram, sem conflito de ABI.
O vínculo completo continua falhando nas mesmas 207 referências (104 código,
103 somente dados). As nove sceMpeg e a integração do player seguem pendentes.
O default.xex 0.33 contém as APIs XbMpeg_* para inspeção estática, mas o launcher
não as chama. Não reproduz os filmes nem roda Game_Main/Battle_Main.
**Não há luta jogável ou teste em Xbox/emulador.**
Consulte RELATORIO-0.33.md e TESTE-XBOX-0.33.md.

--- Histórico ---

# Atualização 0.32 — overlay nativo e ADXT

As 13 APIs ADXT e a consulta de prefetch têm implementação nativa.
Overlay_Load deixou de copiar MIPS sobre código PowerPC: usa catálogo gerado
para os 72 objetos nativos dos menus (119 regiões e 46 relocações de ponteiros).
Restam 207 nomes no vínculo com Game_Main e Battle_Main, antes 221.
387/387 unidades PowerPC, zero conflitos de ABI. Testes de host com ISO/ADX
reais passaram; GPU/áudio/kernel foram simulados onde indicado.
**O pedido completo ainda não está atendido. Não há luta executável.**
O default.xex é o diagnóstico 0.32, sem Game_Main/Battle_Main e sem o catálogo
dos menus. Consulte RELATORIO-0.32.md e TESTE-XBOX-0.32.md.

--- Histórico ---

# Atualização 0.31 — ISO e DMA da inicialização

Cinco chamadas de arquivo e duas de DMA implementadas e verificadas.
384/384 unidades PowerPC, zero conflitos ABI. Vínculo com Game_Main e
Battle_Main: 228 → 221 referências pendentes (133 combate + 88 adicionais).
ISO real: ELF/overlay conferiram por SHA-256; testes de memória e captura
passaram com ASan/UBSan e transportes NT/GPU simulados.
**O pedido de vínculo completo ainda não está atendido. A luta não roda.**
O default.xex é o diagnóstico 0.31. Game_Main e Battle_Main continuam ausentes.
Consulte RELATORIO-0.31.md, inclusive o bloqueio do overlay MIPS, e TESTE-XBOX-0.31.md.

--- Histórico ---

# Atualização 0.30 — chamadas GS e auditoria do boot

Quatro chamadas gráficas reais adaptadas e ligadas ao novo XEX de diagnóstico.
383/383 unidades PowerPC, zero conflitos ABI. Battle_Main: 137 → 133 nomes
pendentes. Incluindo Game_Main, o total é 228: o boot não era coberto antes.
1.841 comparações de pacotes e testes de leitura RGB/sincronização passaram
em host com ASan/UBSan e GPU simulada.
**A luta ainda não roda. Battle_Main e Game_Main seguem fora do XEX.**
Sem teste em Xbox/emulador ou validação de pixels. Consulte RELATORIO-0.30.md.

--- Histórico ---

# Atualização 0.29 — recursos completos e heap do motor

ISO completa baixada, com executáveis e três AFS extraídos e validados.
Heap nativo corrigido: 3.016 verificações passaram. Dados reais: 698 modelos
e 57.259 animações não vazias validados em host; modelo VU em pose de referência.
383/383 unidades PowerPC, sem conflito ABI; vínculo com 137 nomes pendentes.
**A luta ainda não roda. Battle_Main segue fora do XEX. Sem teste em Xbox.**
O default.xex é o diagnóstico 0.27 inalterado. Consulte RELATORIO-0.29.md.

--- Histórico ---

# Atualização 0.28 — fontes do carregador de dados

Carregador BT3X com destinos explícitos, validação das origens/ELF e checksum
antes da escrita. 546 verificações passaram. 382/382 unidades PowerPC,
sem conflito de ABI; o vínculo segue com 139 referências pendentes.
**O jogo e a luta não rodam. Battle_Main segue fora do XEX. Sem teste em Xbox.**
O default.xex continua o diagnóstico 0.27. Consulte RELATORIO-0.28.md.

--- Histórico ---

# Atualização 0.27 — caminho VIF/VU/GIF nativo

Novo default.xex de diagnóstico com as entradas gráficas Port_Gs*, uma sessão
VU compartilhada e conversão do ambiente/clear DBuff. 382/382 unidades do motor
compilaram; ABI sem conflitos; 139 nomes pendentes no vínculo do combate.
**O jogo e a luta não rodam. Battle_Main segue fora do XEX. Sem teste em Xbox.**
Consulte RELATORIO-0.27.md e TESTE-XBOX-0.27.md.

--- Histórico ---

# Atualização 0.26 — fontes do produtor de textura

Dma_PutTexStrips adaptado e compilado no vínculo do motor; 382/382 unidades,
sem conflito ABI, 156 nomes pendentes. 5.768 casos de teste passaram.
**Battle_Main continua fora do XEX, sem luta nem teste em Xbox/emulador.**
O default.xex é o diagnóstico 0.25 inalterado. Consulte RELATORIO-0.26.md.

---

# Atualizacao 0.25 integrada

Patch DMA aplicado, NEXT/retorno com mapas e copia IOP com limites.
381 unidades PowerPC compilaram; 157 nomes ainda bloqueiam Battle_Main.
Novo XEX de diagnostico; nenhuma luta ou teste de hardware.
Veja ../RELATORIO-0.25-integrado.md e reports/0.25/.

--- Historico ---

# Plataforma 0.4 integrada e adaptacao da ABI float

380 unidades PowerPC compilaram; zero rejeicoes por ABI e 163 nomes pendentes.
O motor continua sem vinculo completo e fora do XEX. Novo launcher inclui
ADXF/ISO, gancho de recursos, mapa explicito e sondas de leitura.
Relatorio: ../RELATORIO-plataforma-0.4.md. Evidencias: reports/platform-0.4/.

--- Historico ---

# Tentativa de integração de Battle_Main

Compilado como objeto; vínculo ainda falha. Não está no XEX.
Veja combat/README.md e ../RELATORIO-Battle_Main.md.

--- Última versão do executável ---

# Atualização 0.24 — subconjunto de produtores DMA adaptado

Doze produtores de registradores/retângulos de src/sys/dma.c foram adaptados
para stores LE explícitos e comparados byte a byte com o código original.
O diagnóstico usa os produtores Frame/Zbuf/Scissor/XYOffset/AD e os símbolos
Dma_Flush/ResetBuffers no caminho para DMA/VIF, GS e backend. Não há simulação
de combate. Battle_Main permanece fora do XEX; IA, colisões e animações de
combate não foram integradas. Estado: ../RELATORIO-0.24.md.

--- Histórico ---

# Atualização 0.23 — consumidor DMA/VIF limitado

O título liga dr_gs_stream: oito tipos de tag DMA, regiões de endereço
explícitas, TTE, payloads VIF fragmentados e a VU de referência little-endian.
O diagnóstico sintético usa CNT/TTE/DIRECT antes da captura GS. O corpo e
as substituições faciais reais usam o novo consumidor VIF no visualizador;
a comparação do corpo com a 0.22 foi idêntica byte por byte na CPU.

Battle_Main continua ausente. O consumidor NÃO adapta automaticamente os
produtores de pacotes da luta, a memória PS2 ou as dependências da simulação.
Não houve execução do XEX, luta ou validação de pixels no Xbox/emulador.
Estado detalhado: ../RELATORIO-0.23.md e ../PENDENCIAS.md.

--- Histórico ---

# Atualização 0.22 — alfa bruto e feedback de targets

Corrigidos os consumidores de targets que perdiam alfa acima de 128 e as
cópias que deixavam o MRT auxiliar do destino antigo. A captura Xbox mantém
primitivas de feedback separadas e resolve antes de amostrar. O XEX foi
recompilado. Battle_Main continua ausente; o backend completo e a ligação
à simulação de combate permanecem pendentes. Nenhum pixel de Xbox foi
validado. Leia ../RELATORIO-0.22.md e ../PENDENCIAS.md.

--- Histórico ---

# Atualização 0.21 — consumidores de buffers

Contorno e CLUT por profundidade/IDs congelados agora entram no XEX e no
caminho da captura original. As chamadas de GPU são testadas no host, sem
pixels de console. A auditoria de battle.c aponta 87 dependências diretas
ausentes do título; não é a contagem transitiva nem uma luta executável.
Estado atual: ../RELATORIO-0.21.md e reports/0.21/.

--- Histórico anterior ---

# Atualização 0.20 — shaders VU nativos e stencil de sombras

A ponte usa índices e uniformes da captura original para três shaders de
modelos/cenário/projeção. Z quantizado, clamp de target e início dos grupos
de stencil estão implementados. O diagnóstico continua sintético; não liga
Battle_Main nem comprova pixels em Xbox. Estado atual: ../RELATORIO-0.20.md.

--- Histórico anterior ---

# Atualização 0.19 — captura GS ligada ao XEX

A captura original e o GsBackend experimental agora fazem parte do executável.
O caminho de diagnóstico usa pacotes GIF e registradores reais da captura,
com dados sintéticos. Continua sem simulação de combate, shaders VU, sombras
completas, consumidores nativos completos ou auras/explosões originais.
Não houve execução em hardware. Estado atual: ../RELATORIO-0.19.md,
../PENDENCIAS.md e reports/0.19/. Fontes efetivamente usados: capture/.

--- Histórico de versões anteriores ---

# Atualização 0.16 — materiais e geometrias faciais

O XEX novo liga 13 materiais reais à malha do primeiro modelo, preservando
TEX0/STQ da camada base. Cinco substituições de geometria facial estão
acessíveis por X no visualizador. São poses fixas, sem lip-sync ou combate.
O teste integrado usa a ISO real e chamadas GPU simuladas. Não houve
rasterização de hardware ou execução do XEX no console/emulador.
Leia ../RELATORIO-0.16.md, ../PENDENCIAS.md e reports/0.16/.

--- Histórico anterior ---

# Atualização 0.15 — leitor TexFile e prévias

O XEX atual foi recompilado com o leitor de texturas reais e duas prévias
separadas no visualizador. A malha continua com cores por parte. Leia
../RELATORIO-0.15.md e reports/0.15/ para resultados e pendências atuais.
Não há combate, vínculo de materiais ou execução confirmada em Xbox.

--- Estado e evidência anterior da 0.14 ---

# Estado do motor na 0.14

O XEX novo inclui o visualizador de corpo real, o pipeline de referência da
VU1 e os leitores novos. Não inclui o motor original de combate nem um
GsBackend completo e registrado. A ponte GsDraw continua separada do link.

Leia ../MODELO-0.14.md para o caminho executado, comandos de teste e limites;
../PENDENCIAS.md para os subsistemas restantes; ../BUILD-RESULT.txt para o
build PowerPC efetivamente usado. Relatórios atuais: reports/0.14/.
O histórico deste README está em ../extras/engine-README.ate-0.13.md.

A varredura real validou 698 corpos, 43.121 nós e 32.454 listas VIF. Um corpo
foi processado continuamente pelo programa 0 da VU1: 53 partes habilitadas,
564 MSCNT, 564 pacotes GIF e 4.492 triângulos. Os testes de CPU passaram;
o XEX não foi executado e os pixels não foram validados no Xbox.

Os patches de PlayModel, binders e netcode não estão automaticamente ligados
por esse visualizador. Não há loop de combate, gameplay online ou validação
de IA/física. Os diretórios reports/0.12 e 0.13 descrevem apenas suas versões;
qualquer afirmação antiga de XEX 0.9 preservado é histórica, não o estado atual.
