# DragonRage360 0.50-D16 — pacotes de técnicas e teste no Xenia

## Avanço confirmado

Os cinco pacotes de técnicas de cada lutador foram inspecionados nos arquivos de teste externos. Todos têm uma tabela consistente; o conjunto de emissores começa na entrada 4. Estendi a preparação nativa dos emissores, texturas e parâmetros aos slots que têm as entradas 1 a 3 vazias. O adaptador agora liga três slots do lutador esquerdo e quatro do direito em `BObjMdl.unk84[0..4]`. Os outros três slots (esquerdo 3 e 4, direito 3) têm um recurso extra de modelo na entrada 3 e continuam desligados até esse formato ser convertido.

No QEMU i386, com dados reais e verificadores UBSan/bounds, dois ciclos completos passaram. Os sete slots chegaram à estrutura original `gEftShot`; foram conferidos os parâmetros e a criação das tarefas de grupo quando há classe para o tipo. Cada ciclo executou três pares originais `BtlScene_Update/PostUpdate`, liberou as tarefas e encerrou a cena. Dez alterações malformadas dos recursos foram rejeitadas sem publicar a raiz. O link PowerPC estrito, o catálogo tipado e a verificação estática dos dois XEX passaram, sem símbolos pendentes.

No Xenia Canary 291f0c9, com Vulkan llvmpipe e a ISO USA externa de teste, o D16 registrou 3+4 pacotes preparados, inicializou a cena e as tarefas dos dois lutadores, e chegou a 60 pares de atualização em CPU. O processo permaneceu ativo até o limite de 30 segundos do teste; o encerramento pelo testador não equivale a uma partida concluída. O diagnóstico não solicitou golpes nem executou `Battle_Update` ou `Battle_Main`.

## Limite observado no vídeo

Nesta restauração do ambiente, a leitura do pixel da interface retornou `FF000000` e a verificação da primeira tela falhou. Repeti com o XEX D15 anterior no mesmo Xenia e ambiente: o mesmo pixel também retornou `FF000000`. Assim, a diferença aparece no ambiente de vídeo restaurado e não demonstra uma regressão específica do D16; tampouco autoriza afirmar que a GPU apresentou a cena. O pixel testado é da interface, nunca da luta. O teste usou `--apu=nop`, sem prova de áudio audível.

## Pendências

Restam três pacotes de técnica com modelo adicional, os recursos internos/draw do cenário, dois recursos desconhecidos do comum 3 (0x23D e 0x23E; 635/637 preparados), HUD e apresentação de combate, skinning/pixels dos lutadores, mandíbula/lábios, auras e efeitos na GPU. O ciclo completo `Battle_Load/Battle_Init` e `Battle_Update/Battle_Main` não foi executado. O marcador NOT_READY segue em 2; não foi removido para chamar combate incompleto.

`default.xex` é o diagnóstico para console; `xenia-291f0c9/default.xex` usa a política de display do emulador. Nenhuma ISO, executável comercial nem recurso de jogo está incluído. Para repetir o diagnóstico com sua ISO USA, coloque a ISO ao lado do XEX (ou na pasta `isos`) e crie `startup_test.txt` contendo somente `typed-data`.
