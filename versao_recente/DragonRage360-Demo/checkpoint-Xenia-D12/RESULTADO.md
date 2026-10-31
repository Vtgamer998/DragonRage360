# Xenia Canary 291f0c9 — referência D12

Executado o AppImage Linux publicado em 26/10/2026 pelo projeto indicado.
SHA-256: 9b1b17f8c602420a8387da93a8d7ce1436fb5d9129ffd51e1009ff1b21a18eaa.
O XEX testado é o D12, SHA-256
e81885660384fe3788e37f8353223bb16eaa1fdaebba1583e67c666aee9bb139.

Vulkan usa llvmpipe/Mesa 25.2.8 em CPU, numa tela Xvfb de 1280x720.
O emulador criou o dispositivo e a swapchain, carregou o módulo XEX e
executou sua thread principal. O log do guest registra as verificações
sintéticas BPE, BIN/movimentos/esqueleto, endereçamento GS e VU.

A abertura da apresentação falhou: "the LUT autofill didn't finish",
status 0xC00000B5, seguido de "FAIL: present_open" e saída 1. Não houve
quadro do cenário ou da batalha comprovado. O marcador 2 e o bloqueio da
luta permaneceram. Este teste não valida a revisão D13 nem Xbox físico.

O log de kernel cita chamadas de arquivos indisponíveis em game: e usa
cache: para o log do guest. Isso também deve ser considerado na compatibilidade
do SDK com o emulador. A ISO não foi iniciada nesse teste de referência.
