# DragonRage360 0.49-D15 — relatório de execução

## O que avançou

A partir do D14, integrei o ciclo original `BtlScene_CreateChar` e `BtlScene_FreeChar` dos dois personagens ao adaptador de cena, depois de preparar e vincular os gerenciadores de efeitos no campo nativo do objeto. O encerramento da cena libera as tarefas de personagem antes de destruir a raiz. A validação de estado impede criação duplicada ou sem cena inicializada.

No QEMU i386 com UBSan/bounds e dados reais, dois ciclos completos criaram e liberaram as tarefas dos dois lutadores; cada ciclo executou três pares de `BtlScene_Update/PostUpdate`. O conjunto anterior também passou: 39 cenários, temporizadores e objetos animados, comum 3 com 635/637 entradas, gerenciadores de efeitos e câmeras, objetos, BtlChar/BtlAi, áudio em dispositivo PCM simulado e fluxo de dados tipados. Resultado: `PASS` em `checkpoint-D15-final/host-result.json`.

No Xenia Canary 291f0c9 com a ISO USA externa de teste, o diagnóstico chegou a `BtlScene_Init`, registrou a criação das tarefas dos dois personagens com erro 0 e executou ao menos 60 pares de atualização da cena em CPU após `BtlStage_Update`. A interface abriu em 1280×720 e seu pixel de verificação foi `FFFF8A1F`. O log bruto está em `checkpoint-D15-final/`.

O link PowerPC do código original de combate não teve símbolos pendentes. Os XEX de console e Xenia são variantes separadas; apenas a segunda conserva a LUT do kernel para evitar o timeout de autofill observado no D13. As fontes, patches e o compilador completo do SDK foram preservados separadamente.

## Pendências reais

O ciclo completo `Battle_Load/Battle_Init`, `Battle_Update` e `Battle_Main` não foi executado. O marcador NOT_READY continua em 2. Os cinco pacotes de técnicas de cada lutador não são preparados nem ligados às tarefas de golpes; as duas entradas 0x23D/0x23E do comum 3 permanecem ausentes. Os recursos internos de cenário, o HUD completo, o desenho da cena e da luta, skinning/pixels dos personagens, mandíbula/lábios animados, auras e efeitos GPU não foram comprovados. O teste de pixel acima é da interface. O áudio solicita quatro bancos e uma voz, mas o Xenia não abriu o dispositivo SDL no ambiente de teste; saída audível não foi comprovada.

Nenhuma ISO nem recursos comerciais do jogo acompanham estes arquivos. `default.xex` é a versão de console; `xenia-291f0c9/default.xex` é a variante do emulador. Para o diagnóstico com dados reais, ponha sua própria ISO USA ao lado do XEX e crie `startup_test.txt` contendo `typed-data`.
