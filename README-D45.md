# DragonRage360 D45 — replay experimental

Esta revisão continua a D44 e preserva as correções dos pacotes CLAMP e dos seis leitores de ki. NOT_READY permanece 2. O jogo ainda não está completo; não há 30 FPS comprovados no Xbox 360 físico.

## Usar

Extraia DragonRage360-D45-pacote.zip. No RGH, use o default.xex da raiz; xenia/default.xex é exclusivo para emulação. Coloque sua ISO em isos/. A ISO não acompanha o pacote. startup_test.txt contém menu. Na Biblioteca, A inicia a luta local experimental e X verifica a ISO. Back + Start encerra entre quadros completos.

Para comparar com o log D43, use original-battle no startup_test.txt e a mesma ISO/configuração. Os tokens original-actions-fast e original-actions-cpu são testes exclusivos da variante Xenia. O primeiro acelera apenas a espera READY; o segundo não faz replay nem apresentação GPU. Nenhum deles mede o FPS do Xbox.

## Mudanças

- A lista inteira continua sendo verificada antes de tocar no eDRAM. A emissão reaproveita os dados imutáveis já checados, evitando repetir a varredura de vértices e constantes. Capacidade, snapshots, disponibilidade de alvos, fences e falhas GPU continuam verificados. Chamadas avulsas ao desenho mantêm a validação normal.
- Agrupamento conservador de draws consecutivos com estado, textura e constantes idênticos, dados contíguos e até 1.024 vértices (1.023 em triângulos). Snapshots, leitura de alvos, DATE, efeitos e stencil mantêm suas fronteiras. A ordem original não muda.
- O teste sintético de 1.000 triângulos compatíveis gera 3 draws. No cenário real testado, o agrupamento adicional foi **0**: o front-end já combina seus estados compatíveis. Não se afirma redução real de draws nessa cena.
- Novos contadores: replay_commands, merged_draws, scanned_vertices e reused_vertices. O último registrou **7.330.368** vértices cuja segunda checagem foi evitada até o último perfil da sequência. É uma contagem de trabalho evitado, não FPS.

## Evidências

14 grupos de testes de host passaram, incluindo ASAN/UBSAN onde aplicável. O verificador de agrupamento cobre 144 mutações de bytes do estado, 228 mudanças nas constantes, limites e barreiras. Um vértice inválido no último draw ainda impede a lista inteira de ser emitida.

O XEX final completou 66 passos, incluindo 64 quadros FIGHT, com ataque, carga de ki e entradas B/Y. Houve aumento de ki de 40.000 para 40.650, 58 recursos convertidos sem rejeições, zero erro DMA e encerramento limpo com retorno à interface. As capturas dos quadros 32 e 64 foram idênticas, pixel a pixel, às da execução anterior à mudança de validação. Isso vale somente para esse teste com Xenia/llvmpipe.

Hashes, contadores e logs estão em RESULTADO-D45.json e validation-d45/. As evidências D44 são históricas.

## Pendências

Medição no console físico e estabilidade a 30 FPS; partida completa até KO/resultados; fluxo original dos menus; cenários 1 a 38; áudio audível, saves e online. Permanece uma diferença visual no recorte do clarão de impacto. O limite diagnóstico de 3.600 passos permanece. Não houve publicação no GitHub nesta entrega.

## Reconstrução

Use o kit de compilação D43, GCC/G++ 15.3 powerpc-xsdk-elf, XSDK 8c58c7a3fb37bb261c5a11a8a1f4fa8a8bfd0d28 com keep-kernel-LUT e a base nativa 3ca419c7966b11438e2dc3c9a7e1914f102ed545.

    python3 tools/test_d45.py --upstream /caminho/tenkaichi --toolchain-prefix /caminho/toolchain/bin/powerpc-xsdk-elf-
    python3 tools/build_d45.py --xsdk /caminho/xsdk --upstream /caminho/tenkaichi --toolchain-prefix /caminho/toolchain/bin/powerpc-xsdk-elf- --plugin-include /caminho/includes-host --output build/d45 --jobs 4

Os 566 fontes em vendor/tenkaichi-original permanecem íntegros.
