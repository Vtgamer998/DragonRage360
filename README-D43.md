# D43 — XEX nativo experimental

Esta revisão foi compilada para PowerPC com o XSDK oficial. Há variantes para console RGH e para Xenia. O jogo ainda não está completo e não há 30 FPS comprovados no Xbox. `NOT_READY` permanece em **2**.

## Correções

- Programas VU 0/1 dos lutadores ligados ao shader nativo de skinning/toon, com identificação por tamanho/hash, leitura LE, limites, validação e fallback para layouts desconhecidos.
- Divisão de comandos antes do limite do buffer, usando o cursor vivo do SDK. Preserva cor, alpha, profundidade, stencil, vértices e os oito buffers até suas fences.
- Snapshots de alpha com o layout e o pitch reais do alvo. Antes a cópia usava pitch 1280 para um alvo de 1024; o SDK também usa esse pitch para ler o eDRAM, corrompendo os blocos. DATE, contorno e CLUT agora usam as dimensões corretas. O contorno continua ativo.
- Vida inicial em 100%. O carregador original espera porcentagem e a configuração anterior passava 1.0, que significa 1%.
- Erros da GPU registram a operação e o NTSTATUS. Apenas a variante de Xenia permite espera de 30 segundos para seu rasterizador por software; a de console mantém 1000 ms. Timeout nunca autoriza liberar memória em uso.
- Corrigido o verificador do empacotador. O fluxo `.github/workflows/native-d43.yml` permite reconstrução com as revisões exatas; sua execução no GitHub depende da publicação.

## Usar o pacote compilado

Em `DragonRage360-D43-pacote.zip`, o `default.xex` da raiz é o de **console RGH**. A pasta `xenia/` contém a variante de emulação. Não troque as duas.

Coloque sua ISO em `isos/`, junto da estrutura do pacote. A ISO não acompanha a entrega. `startup_test.txt` contém `original-battle`, que abre o diagnóstico experimental com cenário, dois lutadores e HUD. Back + Start pede encerramento entre quadros completos. O laço ainda tem limite de 3600 passos; não é o fluxo completo dos menus e da partida original.

Confira hashes e evidências em `RESULTADO-D43.json` e `validation-d43/`. A interface inicial ainda é diagnóstica. Menus originais, fim natural por KO/resultados, saves, todos os cenários, online e áudio audível não estão validados.

## Evidências e limites

Os sete testes de host passaram com ASAN/UBSAN onde aplicável. Conferem microprogramas montados dos VSM originais, entradas malformadas, vida útil de buffers, divisão de comandos e snapshots com larguras diferentes. Os shaders embarcados passaram em 128 casos de vértices e 30 de profundidade simulados na CPU.

A validação da ISO conferiu 57.259 animações, 698 esqueletos e 43.121 nós sem falhas no escopo do conversor. Isso não comprova que todos os personagens e cenários estejam jogáveis.

A execução em Xenia fica separada em `validation-d43/xenia/`. As capturas usam llvmpipe e os tempos não medem o Xbox. As linhas retangulares do cenário desapareceram na comparação visual após a correção do pitch. Outras diferenças visuais e o combate completo continuam em validação.

## Compilar

Requer GCC/G++ 15.3.0 `powerpc-xsdk-elf`, XSDK `8c58c7a3fb37bb261c5a11a8a1f4fa8a8bfd0d28` com o patch keep-kernel-LUT e a base nativa `3ca419c7966b11438e2dc3c9a7e1914f102ed545`.

    python3 tools/test_d43.py
    python3 tools/build_d43.py --xsdk /caminho/xsdk --upstream /caminho/tenkaichi --toolchain-prefix /caminho/toolchain/bin/powerpc-xsdk-elf- --plugin-include /usr/include --output build/d43 --jobs 4

A toolchain completa foi reconstruída e passou nas verificações oficiais de ABI e alinhamento. O kit separado preserva o compilador relocável, fontes GNU verificados, bundles públicos das duas bases e o patch. Não redistribui dados da ISO.

Os 566 fontes em `vendor/tenkaichi-original/` permanecem íntegros na revisão `7b43a75`. O fluxo `Game_Main -> Progress_Main -> Battle_Main` ainda exige integração; o diagnóstico usa `Battle_Init`, `Battle_StepFrame` e `Battle_Term`.
