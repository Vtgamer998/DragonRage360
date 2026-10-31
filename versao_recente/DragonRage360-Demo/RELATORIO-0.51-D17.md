# DragonRage360 0.51-D17 — preparação de dados e diagnóstico

## Entregue nesta rodada

- Os três pacotes de técnica antes desligados (esquerdo 3 e 4, direito 3) agora têm sua entrada extra de 2.816 bytes convertida com verificação estrita: 704 palavras de ponto flutuante, nos dados USA examinados apenas zero ou −160. O adaptador só aceita esse formato conhecido; rejeita valores inesperados e não publica o pacote parcial. Os dez slots dos dois lutadores foram ligados às estruturas originais da cena de efeitos. A função exata dessa entrada no desenho de golpes ainda não foi demonstrada.
- As entradas 0x23D e 0x23E do comum 3 foram preparadas em ordem nativa, com limites de tamanho, escalares finitos e verificação do campo inteiro conhecido. Resultado no arquivo USA testado: 637/637 entradas preparadas, nenhuma indisponível pelo conversor. Nenhum consumidor decompilado nomeia as duas entradas, portanto isso comprova preparação, não uso em tela.
- Os testes QEMU i386 com verificadores UBSan/bounds passaram em dois ciclos: nove corrupções do comum 3, onze corrupções dos recursos de efeito, dez slots originais da cena, seis pares de atualização em CPU, inicialização/atualização/encerramento originais de 39 cenários, 31 definições animadas e 57 voltas de temporizadores. O ciclo de áudio original gerou amostras em um dispositivo simulado. Nenhum desses testes desenhou a luta na GPU.
- O vínculo PowerPC estrito passou, sem símbolos indefinidos, e foram gerados XEX novos para console e Xenia.

## Execução no Xenia Canary 291f0c9

O diagnóstico D17 leu dados da ISO USA externa, registrou 637/637 entradas do comum 3, cinco pacotes de técnica por lutador, BtlObj_Init e inicialização separada de BtlChar/BtlAi, cenário e cena originais e 60 pares de atualização da cena em CPU. O processo continuou ativo até o limite de 55 segundos; foi encerrado pelo testador. O teste usou Vulkan llvmpipe e `--apu=nop`.

O pixel da interface foi `FF000000`, com FAIL, como no controle D15 anterior. Não há prova de pixel da luta, quadro real da luta, skinning, HUD, mandíbula, lábios, auras ou efeitos na GPU. O Xenia não provou áudio audível nesta execução. O XEX não foi executado num Xbox real nesta rodada; o último teste de console comunicado permanece o 0.27.

## Estado da lista solicitada

| Pedido | Estado verificável |
| --- | --- |
| Converter os três pacotes extras | Preparação e vínculo em CPU concluídos para o formato USA testado; apresentação do conteúdo ainda não validada |
| Resolver 0x23D/0x23E | Preparação nativa concluída; sem consumidor original identificado |
| Recursos internos do cenário | Pendentes, apesar da geometria, animações e temporizadores de 39 cenários já testados em CPU |
| Battle_Load e Battle_Init | Não executados como ciclo original completo |
| Battle_Update e Battle_Main | Não executados; NOT_READY continua em 2 |
| Primeiro quadro da luta e GPU | Não demonstrados; pixel da interface preto |
| Áudio audível | Pendente em dispositivo físico; Xenia rodou com APU desabilitada |
| Menus completos, saves, online | Pendentes |
| Xbox 360 real | Pendente de execução no console do usuário |

O diagnóstico constrói sua própria sessão para observar os consumidores. `Battle_Load` inicia BtlObj, scripts e uma fila de carregamento bloqueante antes de criar os objetos; `Battle_Init` chama esse carregador e então aloca os demais subsistemas e reinicia a luta. A sessão de diagnóstico já inicializou partes desse estado por outro caminho. Chamar as funções em cima dela duplicaria o boot e não comprovaria uma partida. O marcador NOT_READY foi preservado.

## Como repetir no Xbox real

Use o XEX de console em `default.xex` com sua própria ISO USA ao lado dele (ou em `isos`). Crie `startup_test.txt` com uma única linha `typed-data` e execute no seu Xbox 360 preparado para homebrew. Copie o log gerado e informe se a interface aparece, se o pixel do teste passa, se a cena registra 60 atualizações e se há som audível. Não atribua o teste ao Xbox até haver esse log e observação do vídeo.

Os pacotes distribuídos não contêm ISO, arquivos de jogo ou executável comercial. SHA-256 do XEX console: `c7b36182d9dd14d33013397e879529b6dc5e390b456c5174f8e8762120118a83`. SHA-256 do XEX Xenia: `3b77a903df8e18d1d7f8a9ee60f865655373a5e7a0c4ca27402aa60fbe0f15ce`.
