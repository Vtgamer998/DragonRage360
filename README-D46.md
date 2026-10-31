# DragonRage360 D46 — falhas tardias e estados gráficos

A D46 continua experimental. NOT_READY permanece 2. Não há 30 FPS comprovados no Xbox 360; o jogo, os menus originais, KO/resultados, áudio audível, saves e online ainda não estão completos/validados.

## Executar

Extraia o pacote. No Xbox 360 RGH, abra `default.xex` da raiz. A variante `xenia/default.xex` serve para emulação. Coloque a ISO em `isos/`; ela não acompanha o pacote. `startup_test.txt` contém `menu`. Na Biblioteca, A inicia a luta e X verifica a ISO. Back + Start encerra entre quadros completos. Não misture XEX de versões diferentes.

## Correções e evidências

1. **Fonte e cabeçalho GIF:** `Font_CloseSpriteTag` gravava a contagem com um ponteiro `u16`. No PowerPC, isso substituía os bits altos (NREG/FLG), em vez dos baixos. A sequência de carga prolongada reproduziu exatamente o cabeçalho inválido `00bd000000008000` para 189 letras; a forma numérica correta é `64000000000000bd`. A gravação agora usa máscara numérica e verifica a extensão do pacote antes de modificar o cabeçalho. A corrupção fazia dados de texto serem interpretados como comandos GS, contaminando o quadro e encerrando a sessão.
2. **Recursos vivos:** o histórico de 64 leituras sobrescrevia registros ainda usados por cadeias VIF quando novos recursos eram carregados. O registro agora conserva até 512 recursos vivos; liberação/leituras sobrepostas revogam os registros. Se atingir a capacidade, recusa o novo registro sem substituir um proprietário vivo. A busca usa o maior índice ocupado, não percorre sempre os 512 lugares. A sequência antes recusada no quadro 249 avançou após essa mudança.
3. **Contrato do caminho VU:** distingue um descarte explícito do front-end, sem vértices nem índices emitidos, de uma emissão parcial ou erro. Emissão silenciosamente incompleta, falhas do backend e capacidades excedidas continuam sendo recusadas. Essa proteção não foi a causa primária das duas falhas acima; no teste posterior às correções, o contador de descartes ficou em zero.
4. **Pressão de texturas e sprites:** o front-end retornava a textura branca imediatamente quando os slots do backend acabavam, antes da rotina que libera texturas antigas. A pressão agora aciona essa rotina, preservando todas as texturas referenciadas no quadro atual. O limite de 256 slots continua. Há contadores de varreduras e fallbacks brancos. O replay também aceita a grade positiva de sprites somente quando a textura resolvida é a branca constante, para a qual a posição da amostra não altera o resultado; não ignora a grade de texturas reais.
5. **Triângulos imediatos de efeitos:** `EftMesh_SendTri` copiava diretamente um pacote com RGBAQ/ST em campos nativos e XYZF em bitfields para uma região tratada como u64. Agora usa o mesmo serializador por registro da fila OT, com verificação prévia da região DMA. Isso corrige a representação BE desse produtor; o teste isola as cores/Q/XYZF, o comprimento e a recusa sem mutação. A correção de todos os efeitos ainda não foi comprovada visualmente.
6. **Estados gráficos:** evita repetir profundidade/stencil, blends, scissor, constantes de pixel e descritores de textura idênticos. Compara os valores dos descritores, não seus endereços. Trocas de alvo, restores, snapshots e rollovers invalidam o cache pelo epoch. A validação completa do replay da D45 e as verificações de capacidade/fences permanecem.
7. **FTOI do VU:** conversão inteira definida para FTOI0/4/12/15, com truncamento e saturação por sinal, inclusive expoente 255. Evita conversões C indefinidas e multiplicação em double. Semântica conferida com o código público `floatToInt` de PCSX2/VUops.cpp; implementação independente por expoente e mantissa. Isso não foi identificado como o gatilho da falha de ki.

Os 20 grupos de testes incluem 80.160 casos executados nas instruções FTOI reais do interpretador, 18 contratos dos callbacks VU, nove casos do cabeçalho da fonte e retenção de 512 registros com saturação/revogação, seis casos da rotina real de liberação de texturas e o produtor real de triângulos imediatos. Há ASAN/UBSAN onde aplicável e compilação de fixtures para PPC; executar no host não demonstra execução das fixtures no Xbox.

O candidato anterior à correção isolada de `EftMesh_SendTri` avançou por pelo menos 276 quadros FIGHT com GPU, mas o host sofreu SIGBUS com /dev/shm cheio de arquivos abandonados pelo emulador. Esse teste está marcado INCOMPLETE, sem encerramento limpo; não é uma aprovação de 384 quadros com GPU. O teste do XEX final é identificado separadamente. Não se atribui o hash de um binário ao teste de outro. Resultados dos XEX, logs de reprodução e testes com Xenia estão em `RESULTADO-D46.json` e `validation-d46/`. Os relatórios D43–D45 são históricos. O teste curto de cache preservou os pixels dos quadros 32 e 64 da D45; esse resultado não certifica todos os efeitos.

## Desempenho e limites

O log D45 recebido do console para no quadro 160, sem exceção final. Os blocos de 32 quadros perfilados equivalem a aproximadamente 497–524 ms por quadro; o replay custa aproximadamente 221–239 ms. A D46 registra também tempos de Battle_Update e Battle_Draw para localizar o custo restante. Trabalho evitado pelo cache não é uma medição de FPS.

A sequência de teste mantém LT em dois intervalos, com liberação entre eles, e inclui a IA adversária. Uma reação do adversário pode interromper a ação de carregar. O teste não comprova carga ininterrupta até MAX POWER, partida até KO, todos os golpes/personagens/cenários ou estabilidade indefinida. A espera READY é acelerada apenas no teste; os quadros FIGHT não são acelerados.

O recorte retangular do clarão de impacto ainda não tem correção visual comprovada. A corrupção causada pelo cabeçalho de texto foi corrigida no caminho reproduzido; não se afirma que seja o mesmo defeito visual relatado sem uma captura desse efeito.

O loop normal mantém o limite experimental de 3.600 passos. Cenários 1–38, fluxo original dos menus, áudio audível, saves e online permanecem pendentes. Não houve publicação no GitHub.

## Reproduzir

Use o kit D43, GCC/G++ 15.3 powerpc-xsdk-elf, XSDK 8c58c7a3fb37bb261c5a11a8a1f4fa8a8bfd0d28 com keep-kernel-LUT, e upstream 3ca419c7966b11438e2dc3c9a7e1914f102ed545.

    python3 tools/test_d46.py --upstream /caminho/tenkaichi --toolchain-prefix /caminho/toolchain/bin/powerpc-xsdk-elf-
    python3 tools/build_d46.py --xsdk /caminho/xsdk --upstream /caminho/tenkaichi --toolchain-prefix /caminho/toolchain/bin/powerpc-xsdk-elf- --plugin-include /caminho/includes-host --output build/d46 --jobs 4

Somente na variante Xenia: `original-charge-check` realiza o teste prolongado com GPU; `original-charge-cpu` executa produtores e decodificação sem replay/apresentação GPU. Os 566 arquivos originais em `vendor/tenkaichi-original` permanecem íntegros.
