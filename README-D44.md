# DragonRage360 D44 — batalha experimental

Esta revisão corrige a rejeição dos pacotes CLAMP dos efeitos e reduz trabalho repetido de VU e submissão de shaders. O jogo ainda não está completo. Não há medição de 30 FPS no Xbox para D44. NOT_READY permanece 2.

## Instalação e início

Extraia DragonRage360-D44-pacote.zip. Use o default.xex da raiz no Xbox 360 RGH; a variante xenia/default.xex é exclusiva para emulação. Coloque sua ISO na pasta isos/. A ISO não acompanha o pacote.

O startup_test.txt contém menu, que abre a interface e procura as ISOs. Entre em Biblioteca; selecione a ISO e pressione A para iniciar a luta local experimental. X verifica a ISO. Back + Start encerra entre quadros completos e retorna à interface. O limite de 3600 passos permanece; menus originais e fluxo completo de resultados não estão integrados. Para repetir o teste de desempenho D43, troque somente o conteúdo de startup_test.txt para original-battle.

## Alterações

- Serialização numérica de CLAMP_1/CLAMP_2 em REGLIST. EftSprAnim_QueueTri usava esses registros e era rejeitado como -420 ao atacar. Não era falta de espaço no DMA. Registros e fontes desconhecidos continuam rejeitados.
- Seis leitores de ki agora preenchem personagem/tipo em uma instrução separada antes de calcular custo, velocidade ou reação. A expressão original lia valores ainda não inicializados; no PowerPC a reação ao acerto recebia um personagem nulo e bloqueava a luta.
- Listas OT maiores que 64 KiB são divididas entre primitivas completas em janelas DIRECT limitadas; ordem original e verificações de capacidade preservadas.
- Programas de sombra VU 2a/2b reconhecidos por tamanho e hash exatos, com remapeamento validado para o shader nativo e fallback para entradas desconhecidas. Memória original permanece intacta.
- Cache de literais e constantes de shader por passe. Invalida ao trocar gravação/alvo e restaura constantes sobrepostas por literais.
- Resolve de profundidade limitado ao efeito que lê profundidade viva. Contornos e CLUT de ID continuam usando o snapshot de alpha preservado. Efeitos não foram desligados.
- Diagnóstico separa preparação de descritores de submissão Xenos e conta uploads evitados. Sem reordenar draws nem exceder o limite do buffer XSDK. O número de draws continua semelhante; agrupamento amplo ainda está pendente.

## Validação

Os 12 grupos de testes de host passaram. Incluem ABI de pacotes PPC/LE dos efeitos reais, limites de OT, cache entre passes, ASAN/UBSAN onde aplicável, shaders e interface. As sombras sintéticas passaram de 202/211 para 16 instruções interpretadas; isso não é uma medida de FPS. Logs nativos e resultado final estão em RESULTADO-D44.json e validation-d44/.

O token original-actions-fast é apenas diagnóstico: acelera a espera READY através do handler original, injeta quatro entradas durante FIGHT e cancela após 64 quadros FIGHT. Não altera o combate usado por menu/original-battle. Não use tempos de llvmpipe como desempenho de console.

Ainda precisam de validação e integração: estabilidade a 30 FPS, partida até KO/resultados, menus originais, cenários 1 a 38, áudio audível, saves e online. Não foram declarados completos.

## Bloqueio de projétil reproduzido e corrigido

A primeira execução GPU, ainda sem a correção dos leitores de ki, parou no processamento do acerto. O mesmo bloqueio foi reproduzido sem replay GPU e rastreado até BtlKiBlast_GetReactOfHit. A versão com as seis funções sequenciadas completou 64 quadros FIGHT e encerrou corretamente no teste isolado. Os 78 casos de regressão dos leitores também passaram. A execução GPU final também completou 64 quadros FIGHT, sem erro DMA, com 58 recursos convertidos sem rejeições e retorno limpo à interface. O teste usou Xenia/llvmpipe, não o Xbox; o READY foi acelerado apenas no diagnóstico. Evidências e hashes estão em RESULTADO-D44.json. A captura do quadro 64 ainda mostra um recorte retangular no clarão do impacto; equivalência visual perfeita não está comprovada.

## Reconstrução

Use a toolchain relocável do kit D43 e as revisões fixadas nele.

    python3 tools/test_d44.py --upstream /caminho/tenkaichi --toolchain-prefix /caminho/toolchain/bin/powerpc-xsdk-elf-
    python3 tools/build_d44.py --xsdk /caminho/xsdk --upstream /caminho/tenkaichi --toolchain-prefix /caminho/toolchain/bin/powerpc-xsdk-elf- --plugin-include /caminho/includes-host --output build/d44 --jobs 4

XSDK 8c58c7a3fb37bb261c5a11a8a1f4fa8a8bfd0d28 com patch keep-kernel-LUT; base nativa 3ca419c7966b11438e2dc3c9a7e1914f102ed545. Os 566 fontes originais em vendor/ permanecem íntegros. Fontes D43 e evidências anteriores são históricas. Publicação no GitHub não foi concluída nesta entrega.
