# D48 (2026-10-31) — atualização

XEX D47 recompilado pelas Actions (console e xenia, verificação estática OK)
e luta varrida no Xenia Canary/llvmpipe com ISO real: **321 quadros,
`Battle_StepFrame returned=0` em todos os checkpoints, zero
`DR_DMA_INVALID`/`-420`** (inclui o quadro 88 do console). Evidência em
`validation-d47/xenia/original-battle-321/`; leia [RELATORIO-D48.md](RELATORIO-D48.md).

**Hardware real (RGH) confirmado com o log do usuário
(`validation-d47/console/simple-run/`): boot completo no Xbox, Battle_Init OK,
luta até o quadro 192 com `returned=0` em todos os checkpoints e ZERO
`DR_DMA_INVALID`/`-420` — o quadro 88 que travava passou no hardware com o XEX
console compilado do logo após os fontes D47.**
Bloco 1 "luta estável" segue nos testes longos (minutos contínuos, KO,
resultado). Medição de desempenho em hardware: ~2 FPS, com o **replay de
comandos a ~93% do quadro** (231 ms dos ~268 ms/quadro; simulação 15 ms,
produtores 22 ms). Gargala já medido: reescrever o caminho de desenho é o
alvo do bloco 2.

# 0.44-D10 — novo XEX: objetos, BtlChar e BtlAi (histórico)

Novo default.xex compilado. O boot original de objetos e o reset/inicialização
de BtlChar/BtlAi estão integrados ao diagnóstico e passaram em CPU i386 com
recursos reais. Ainda não existe luta jogável: Battle_Update/Battle_Main e
apresentação GPU do combate continuam pendentes; sem teste em Xbox/Xenia.

Leia TESTAR-PRIMEIRO.md e RELATORIO-0.44-D10.md. O XEX anterior D8 foi preservado.

---
Documentação histórica da versão anterior:

ATUALIZAÇÃO 0.42-D8: seis janelas comuns/IA e oito janelas por modelo preparadas; estado facial CPU exercitado. BtlChar/BtlAi, boot completo, cenário/simulação/controle/apresentação continuam pendentes. Luta bloqueada; XEX recompilado, não executado no Xbox. Leia RELATORIO-0.42-D8.md.

DOCUMENTAÇÃO HISTÓRICA ABAIXO:

ATUALIZAÇÃO 0.38-D4: reprodução/retarget, poses, matrizes, BobjWork e volumes ligados ao caminho original; texturas e referências faciais preparadas. A lista AINDA NÃO está concluída: faltam tabelas/binders completos, renderer/facial e estado/registro nos gerenciadores. Novo XEX de diagnóstico; luta bloqueada e versão não testada no Xbox/Xenia. Consulte RELATORIO-0.38-D4.md.

DOCUMENTAÇÃO HISTÓRICA ABAIXO:

ATUALIZAÇÃO 0.37-D3: catálogo nativo complete=true com exclusões comprovadas de pools originais sem consumidor. Colisão instalada com NOT_READY; dois modelos/animações preparados. Lutadores completos, cenário completo e boot da luta AINDA PENDENTES. Novo XEX de diagnóstico, sem teste no Xbox. Consulte RELATORIO-0.37-D3.md.

DOCUMENTAÇÃO HISTÓRICA ABAIXO:

ATUALIZAÇÃO 0.36-D2: consulte RELATORIO-0.36-D2.md. O XEX da raiz é o diagnóstico D2, sem luta jogável. Restam 4 faixas/54 bytes no catálogo e o boot/instalação de recursos. Colisão do cenário validada em host, não instalada no combate. Sem teste desta versão no Xbox.

DOCUMENTAÇÃO HISTÓRICA ABAIXO:

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

# Estado atual: 0.25 integrada

381/381 unidades PowerPC compilaram; zero rejeicoes de ABI; 157 nomes pendentes.
Novo XEX de diagnostico com NEXT/retorno mapeado. O combate permanece ausente.
Dma_PutTexStrips, VU1, ADXT/RPC, enderecos dos produtores e dados continuam
pendentes. Dos 96 nomes func_, 95 aparecem apenas em tabelas.
Veja RELATORIO-0.25-integrado.md.

--- Historico ---

# Estado atual da 0.25 (resumo; detalhes em RELATORIO-0.25.md)

Produtores Dma_Alloc/AddData/AddRef/BeginDirect/EndDirect adaptados (teste no host, BE simulado). Combate NÃO integrado; XEX não regerado.


--- Histórico ---

# Estado atual da 0.24

O port completo continua pendente. Não há luta jogável.
Leia RELATORIO-0.24.md para a implementação nova e evidências de CPU.
O novo default.xex foi compilado e ligado para PowerPC. Não houve execução
em Xbox ou emulador; pixels dos shaders e targets continuam sem validação.

| Área | Estado atual | Falta |
| --- | --- | --- |
| Corpo e hierarquia | Leitor validado em 698 pmdl e 32.454 listas VIF | Matrizes animadas e integração no motor |
| VU | Programa 0 original executado continuamente em um corpo real no host | Três shaders VU nativos adicionados; faltam poses reais integradas ao combate e fidelidade de hardware |
| Renderização | Captura GS original e GsBackend experimental ligados ao XEX; núcleo MRT/DATE/FBA, snapshots, stencil e targets amostrados com alfa bruto; feedback por primitiva; shader TFX/TCC/ATST e upload de mips | Concluir o GsBackend e integrar a simulação de combate; concluir os demais consumidores de buffers (contorno/CLUT por ID e Z adicionados), sequência de sombras com dados reais, iluminação/VU no combate e demais estados; validar DATE/FBA/stencil e auras/explosões |
| Facial | Leitor de 2.722 listas DMA/VIF; cinco substituições manuais no modelo de teste | Animação, fala, olhos e validação dos demais personagens |
| Dados | ISO/AFS/BIN, animações e corpo real validados em CPU | Colisão/física de cenário, tabelas e demais contêineres; adaptador do motor |
| Animações | Conversor e patch PlayModel separados | Binders restantes e captura/reconstrução da arena no rollback |
| Plataforma | ISO, tempo, controle, interface e rede do launcher; consumidor canônico DMA/VIF e subconjunto de produtores dma.c com serialização LE | Memória/endereço PS2, heap, áudio, temporização e controles dentro do motor |
| Combate | Ainda sem loop integrado; Dma_Flush/ResetBuffers do transporte nativo ligados, 85 dependências diretas restantes em battle.c | Executar o combate com IA, colisões, animações e renderização |
| Conteúdo | Menu externo | Seleção, história, torneio, treino, loja, galeria e transições |
| Áudio/vídeo | Nenhum conteúdo do jogo no executável | Decodificar, tocar e sincronizar |
| Saves | Exportação RAW separada | Ler/gravar durante o jogo e validar no console |
| Online | Lobby UDP LAN separado e codec do protocolo | Input de luta, determinismo/rollback, NAT, matchmaking pela internet e antitrapaça |
| Desempenho/estabilidade | Testes de CPU e chamadas simuladas | Pixels reais, execução no Xbox, partidas, modos, memória e medições CPU/GPU |

Relatórios antigos descrevem as versões indicadas em seus nomes. As alegações
antigas de 272 arquivos compilados ou cerca de 460 símbolos externos não
representam um jogo vinculado e executado. Os fontes novos não completam os
subsistemas acima. A falta de um log de console não é o único bloqueio.
