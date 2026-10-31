# DragonRage360 0.36-D2 — dados revisados e colisão do cenário

24/10/2026. Novo XEX de diagnóstico, recompilado para PowerPC e vinculado sem símbolos indefinidos ou conflitos de ABI. **Não inicia uma luta. Esta versão não foi executada no Xbox nem no Xenia.**

## Resultado desta revisão

| Item | 0.35-D1 | 0.36-D2 |
|---|---:|---:|
| Faixas sem tipo confirmado | 168 / 5.751 bytes | 4 / 54 bytes |
| Trechos suprimidos não revisados | 23 / 62 bytes | 0 |
| Bytes cobertos por tipos ou invariância de zero, neste catálogo | 29.683 | 35.310 |
| Bytes considerados para carga | 35.434 | 35.364 |
| Operações do catálogo | 2.157 | 2.736 |
| Regiões | 33 | 33 |
| Slots de ponteiro verificados no vínculo final | 1.694 | 1.779 |

As contagens medem somente o catálogo de tabelas selecionadas; não representam uma porcentagem do port ou de todos os recursos do jogo.

`reviewed_types.py` acrescenta 80 descrições limitadas pelos layouts e leitores públicos: menus e submenus de pausa, UTF-16 das fontes, nomes e definições de ícones, tabelas de ícones terminadas em ponteiro nulo, scripts de mira, chaves de mandíbula, constantes matemáticas, cores RGBA, códigos de IA explicitamente lidos como bytes e strings usadas pelas APIs de arquivos/filmes/áudio. Contagens e ponteiros dos menus são percorridos e validados; não se converte qualquer bloco desconhecido em uma matriz de palavras.

Os 23 trechos suprimidos estão antes do primeiro símbolo de seus blocos. `port/tools/gen_data.py:convert` insere `addr & 15` bytes nesse ponto para preservar alinhamento. Eles não pertencem aos objetos originais. O gerador agora comprova essa posição e o módulo 16, registra as exclusões em `alignment_prefixes` e deixa esses bytes nativos intactos. Ponteiros para esse preenchimento são recusados. Isso explica a redução do denominador de bytes considerados; os bytes não foram restaurados como dados do jogo.

O gerador completo foi executado novamente a partir do DWARF do novo vínculo PowerPC. Suas operações coincidiram com a revisão independente do catálogo anterior. `refine_typed_manifest.py` permite repetir essa revisão preservando as ligações simbólicas anteriores; ele não comprova um novo vínculo. O catálogo distribuído vem do gerador completo desta compilação.

## Colisão real do cenário 0

Novo `src/dr_stage_collision.c/.h`, com cópia idêntica em `engine/resource/`. A implementação lê dados little-endian para valores nativos, valida offsets contados em palavras a partir do primeiro membro do pack e fornece triângulos e consultas por caixa fechada em ordem esquerda-direita. Não dereferencia ponteiros MIPS e não modifica o recurso.

Validação do cenário USA, arquivo lógico 369 / AFS índice 368:

- 100 zonas; 151 ocorrências de malha estática ou de objetos destrutíveis;
- 27.015 registros de nó e 13.583 ocorrências de triângulo nesses percursos;
- limites lidos: raio 1.200, topo -350, fundo 1.000;
- os triângulos nativos bateram com uma leitura independente em Python: FNV64 `04645362575aed17`;
- ASan/UBSan: offsets/contagens inválidos, ciclo, vizinho fora da faixa, vértice inválido, NaN e truncamento recusados; entrada inalterada;
- sete zonas de borda têm offset zero para a malha estática: o adaptador as mantém ausentes e nunca interpreta o cabeçalho do cenário como malha.

Essas são ocorrências nos percursos, não uma contagem de triângulos únicos do cenário. A visão conserva os valores e as coordenadas originais; não executa a física, nem prepara objetos, animações, renderização ou estado de destruição. Ainda **não instala `gBtlStage` ou substitui os leitores originais no combate**.

O startup `typed-data` no novo XEX também lê esse recurso pela ISO montada e registra o diagnóstico de colisão no `default.log`, incluindo uma consulta à zona 0. Essa etapa ainda precisa de execução no console. Ela não libera o combate.

## Compilação e verificações

- Motor/base: 396 de 396 unidades; motor com catálogo: 397 de 397. Vínculos estritos: zero indefinidos e zero conflitos de ABI. Não foi desativada a verificação do linker e não foram criados stubs para fechar nomes.
- Upstream: `3ca419c7966b11438e2dc3c9a7e1914f102ed545`.
- xsdk: `57b02f1e60667e37d22c3aa613d23d2486514ff8`; GCC 15.3.0 / binutils 2.47 / newlib 4.6.0.20260123, com patches do SDK. A biblioteca e as unidades do título foram recompiladas.
- O leitor novo compilou para PowerPC com `-mcheck-fpr-alignment`.
- Catálogo final: 33 destinos e todas as 2.736 operações verificados no ELF, incluindo 1.779 slots de ponteiro. 1.473 ligações mudaram de endereço e foram realocadas simbolicamente.
- O carregador real passou em buffers de teste com os dois programas USA, ASan/UBSan, hash incorreto, ponteiro incorreto, sobreposição, limites, reentrada e recusa de catálogo incompleto. Isso não é execução PowerPC.
- Os 18 recursos do perfil fixo foram relidos pelos adaptadores reais: 69 partes em cada modelo, 718 animações não vazias convertidas, 110 slots BPE vazios reconhecidos e zero falhas entre as animações não vazias examinadas.
- Nesta sessão os testes de recursos usaram uma imagem esparsa com intervalos HTTP verificados, não uma ISO completa validada novamente. Cada leitura do transporte de teste foi obrigada a cair em intervalos presentes. Os hashes dos 18 recursos coincidiram com o inventário anterior. Os programas USA foram conferidos por SHA-256.
- A auditoria das seções manteve os 103 símbolos de biblioteca legada isolados e reproduziu a falha quando eles foram forçados como raízes. A sondagem de `Game_Main` completo continua com 69 dependências. Não houve implementação de menus gerais por meio de retornos vazios.
- `Battle_Main` está no XEX; a primeira chamada no seu código PowerPC continua sendo `XbTypedData_RequireBattleData`, seguida da saída condicional antes de `Battle_GetWork`. O marcador permanece 2. O diagnóstico não chama `Battle_Main`.
- Formato XEX2, ABI PowerPC, importações, cabeçalhos e cadeia de hashes verificados estaticamente. GPU, pixels, controle durante luta, áudio de partida e estabilidade não foram testados em hardware.

SHA-256 do **novo default.xex**:
`8ede255841e24eec476c3e4ad292cc9b05a3b92ab0dfbf5a8b3ed21565e1ba58`

## O que continua bloqueando uma luta

O manifesto mantém `complete=false`. Quatro faixas não foram tipadas por palpite:

| Faixa | Bytes | Revisão necessária |
|---|---:|---|
| `D_002EC2C0 + 16` | 16 | Constante adjacente ao vetor declarado; falta provar seu tipo ou que não é consumida. |
| `D_002FE8D0` | 8 | Área do antigo pool de constantes; falta delimitar os campos realmente necessários. |
| `D_002FE958` | 22 | Dados que incluem marcadores de heap; falta identificar o leitor e os limites sem converter texto como palavras. |
| `D_002FEB88` | 8 | Constantes sem declaração/leitor suficiente para autorizar o layout. |

Depois disso ainda é necessário preparar os recursos vivos dos dois lutadores e do cenário: estruturas completas, conversões internas, binders e animações restantes, referências e estados iniciais; instalar o cenário e os objetos no motor; executar o boot mínimo e integrar controle, simulação e apresentação. A inspeção das malhas e o leitor de colisão não substituem essa inicialização. O renderizador original continua dependendo dos recursos e pacotes corretos. Não há partida jogável, validação de desempenho ou online de combate nesta versão.

## Uso e reprodução

O `default.xex` da raiz é 0.36-D2; o 0.35-D1 anterior foi preservado em `diagnostico-anterior-D1`. Os fontes da raiz já estão atualizados. Não aplique o patch incremental sobre eles outra vez.

No Xbox RGH, mantenha `startup_test.txt` com `typed-data` e a ISO do usuário nas pastas reconhecidas pelo launcher. O teste escolhe a primeira ISO encontrada. O esperado é um relatório de carga parcial e de colisão, com combate bloqueado. Guarde o `default.log`. Para o diagnóstico gráfico anterior, use a configuração `startup_test-graphics.txt`. Nada aqui confirma a saída gráfica do motor em luta.

Com toolchain e SDK públicos nas revisões acima:

```sh
make typed-data-diagnostic ISO=/caminho/jogo.iso XSDK=/caminho/xsdk TOOLCHAIN=/caminho/powerpc-xsdk-elf- UPSTREAM=/caminho/dragonrage-upstream
```

Se os headers GMP do host não estiverem nos caminhos padrão, passe os diretórios por `PLUGIN_ARGS='--plugin-include /caminho/include --plugin-include /caminho/include/x86_64-linux-gnu'`.

`engine/tools/test_stage_collision.py` recebe um pack do cenário do próprio usuário e um diretório de saída. `test_fixed_resources.py --coverage ...` acrescenta a verificação dos intervalos presentes quando o input local é esparso. Os relatórios e evidências desta compilação ficam em `checkpoint-dados-D2/`. ISO, programas PS2 e recursos extraídos não estão no pacote. O código modificado continua sob as licenças indicadas no projeto.
