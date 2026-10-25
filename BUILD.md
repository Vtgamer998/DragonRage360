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

# Atualizacao 0.25 integrada

Dependencias do patch incluidas nesta base. Compilacao xsdk e testes:
RELATORIO-0.25-integrado.md. O XEX continua sem Battle_Main.

--- Historico ---

# Plataforma 0.4 integrada

O alvo battle-link agora usa --float-mode native-ps2 --platform.
A ABI compilada passou; o vinculo continua bloqueado por 163 nomes.
Ha novo XEX de diagnostico, sem Battle_Main. Reproducao e limites:
RELATORIO-plataforma-0.4.md.

--- Historico ---

# Build da demonstração 0.24

A demonstração é C e usa diretamente o xsdk. O Makefile inclui os leitores
facial/corpo/ELF, o pipeline VU1, o visualizador e os targets nativos.
A captura GS original e o backend Xenos experimental agora entram no link.
A simulação original de combate continua fora dele.
As instruções históricas estão em extras/BUILD.md.ate-0.13.

Fontes fixadas:

- xsdk: https://github.com/xenosworks/xsdk — 57b02f1e60667e37d22c3aa613d23d2486514ff8
- Tenkaichi3DecompAndroid: https://github.com/dfdx047/Tenkaichi3DecompAndroid — 3ca419c7966b11438e2dc3c9a7e1914f102ed545

Prepare o toolchain conforme xsdk/docs/GETTING_STARTED.md. São necessários
os patches e specs do xsdk, incluindo seu ABI, GPRs de 64 bits e as correções
de alinhamento. GCC PowerPC genérico não é um substituto. A demo requer
somente C; os shaders pré-gerados e seus fontes estão incluídos.

```sh
make -C /caminho/xsdk TOOLCHAIN=/caminho/xsdk-gcc/bin/powerpc-xsdk-elf- build/libxsdk.a
make XSDK=/caminho/xsdk TOOLCHAIN=/caminho/xsdk-gcc/bin/powerpc-xsdk-elf-
python3 tests/check_xex.py
```

Saídas: build/default.elf, build/default.map e build/default.xex.
check_xex.py verifica o ELF, a presença dos símbolos do visualizador e as
estruturas/hashes do XEX. Isso não é um teste de boot ou de pixels.
O empacotador é xsdk/tools/elf2xex.py, sem ferramentas Microsoft.
Title ID de desenvolvimento: 0x44523630.

O estado efetivamente obtido nesta entrega será registrado em BUILD-RESULT.txt
e VALIDACAO.json. As instruções de build não significam que um binário foi
compilado ou executado: consulte esses resultados.

A captura GS é compilada dos fontes fornecidos em engine/capture/.
O backend continua incompleto; os materiais e o visualizador anteriores permanecem.
Leia RELATORIO-0.24.md para evidência e limites.

## Testes de CPU

```sh
make host-test
python3 engine/tools/test_materials.py /caminho/SLUS_21678.ISO /tmp/materials --sanitize --viewer /caminho/xsdk
python3 engine/tools/test_texfile.py /caminho/SLUS_21678.ISO /caminho/upstream /tmp/texfile-test --sanitize
python3 engine/tools/test_body_pipeline.py /caminho/SLUS_21678.ISO /tmp/body-test --sanitize
python3 engine/tools/test_body_demo.py /caminho/xsdk /tmp/body-demo --sanitize
ASAN_OPTIONS=detect_leaks=0 python3 engine/tools/test_targets.py /caminho/xsdk /tmp/targets --sanitize
ASAN_OPTIONS=detect_leaks=0 python3 engine/tools/test_gs_replay.py /caminho/upstream /caminho/xsdk /tmp/replay --sanitize
python3 engine/tests/test_vu1.py /caminho/upstream /tmp/vu-test
```

Os testes com mocks não rasterizam shaders. Os testes de ISO não incluem a
imagem nem extraem arquivos de jogo no pacote. Leia MODELO-0.14.md para o
corpus e os limites. A detecção de leaks está desativada nos comandos com
ASan desta entrega; isso não valida ausência de vazamentos em gameplay.

Os módulos duplicados em src/ e engine/ devem permanecer idênticos. O gerador
engine/tools/build_vu_reference.py exige upstream com o patch de endian
aplicado; o interpretador gerado já está incluído. Preserve
-ffp-contract=off para dr_vu1.o e dr_body_pipeline.o.

Para regenerar shaders, veja engine/graphics/TARGETS.md e REPLAY.md; os fontes
usam glslang e o compilador aberto xost/xsc. A regeneração não valida pixels
no Xbox. Não são necessários compiladores ou SDKs proprietários.


Na 0.24, use também `make engine-backend-check` para compilar os objetos
experimentais adicionais da interface e ponte. O build padrão já liga ambos
a default.elf junto da captura original.
O patch de registro está em engine/patches/xenos-backend-registration.patch;
a captura completa não é compilada por esse alvo. Leia RELATORIO-0.24.md.

Teste da captura completa com chamadas de GPU simuladas:

```sh
ASAN_OPTIONS=detect_leaks=0 python3 engine/tools/test_capture.py /caminho/xsdk /tmp/capture --sanitize
```

`--gc-sections` elimina funções não usadas, incluindo caminhos de DMA/VIF
sem adaptação de endereços PS2. `dr_capture_platform.c` é apenas o perfil
fixo de renderização desta demonstração, sem plataforma de combate.

Shaders VU nativos e teste numérico (sem GPU):

```sh
python3 /caminho/xsdk/tools/build_shaders.py --glslang /caminho/glslangValidator --output engine/graphics/dr_vu_shaders.h engine/graphics/shaders/dr_vu0.vert engine/graphics/shaders/dr_vu4.vert engine/graphics/shaders/dr_vu6.vert
python3 engine/tools/test_vu_shaders.py /caminho/xsdk /caminho/glslangValidator /tmp/vu-shaders
```

Depois de regenerar, copie dr_vu_shaders.h e dr_gs_shaders.h de engine/graphics/
para src/. Ambos os headers acompanham a entrega; o build padrão não exige
recompilar os shaders. A simulação de combate continua fora do título.

Consumidores e auditoria do combate:

```sh
python3 /caminho/xsdk/tools/build_shaders.py --glslang /caminho/glslangValidator --output engine/graphics/dr_effects_shaders.h engine/graphics/shaders/dr_outline.frag engine/graphics/shaders/dr_dclut.frag
python3 engine/tools/test_effect_shaders.py /caminho/xsdk /caminho/glslangValidator /tmp/effects
python3 engine/tools/audit_battle_link.py /caminho/upstream /caminho/xsdk-gcc/bin/powerpc-xsdk-elf-gcc build/default.elf /tmp/battle-audit
```

Copie também o header gerado dr_effects_shaders.h para src/ ao regenerá-lo.
A auditoria compila somente battle.c e compara dependências diretas com o
título; não implementa ou liga o combate nem valida a estrutura dos dados.

Regeneração do shader de targets (alfa bruto separado da cor):

```sh
python3 /caminho/xsdk/tools/build_shaders.py --glslang /caminho/glslangValidator --output engine/graphics/dr_gs_shaders.h engine/graphics/shaders/dr_gs.frag engine/graphics/shaders/dr_gs.vert engine/graphics/shaders/dr_gs_target.frag
python3 engine/tools/test_gs_buffers.py /caminho/xsdk /caminho/glslangValidator /tmp/gs-buffers
```

Copie dr_gs_shaders.h para src/ depois de regenerar. A cópia do alfa bruto
usa o mesmo sampler/UV da cor, no binding 2; DATE permanece no binding 1.
As leituras CT32 do próprio target são separadas em primitivas, cada uma
resolvida antes de ser amostrada. Isso não estabelece equivalência com o
cache do GS original, nem adapta os construtores de pacotes do combate.

Consumidor DMA/VIF limitado (0.24):

```sh
ASAN_OPTIONS=detect_leaks=0 python3 engine/tools/test_gs_stream.py /tmp/stream --sanitize
ASAN_OPTIONS=detect_leaks=0 python3 engine/tools/test_capture.py /caminho/xsdk /tmp/capture --sanitize
python3 engine/tools/test_body_pipeline.py /caminho/ISO /tmp/body --sanitize
python3 engine/tools/test_materials.py /caminho/ISO /tmp/materials --viewer /caminho/xsdk --sanitize
```

`dr_gs_stream_chain` recebe regiões explícitas `DrGsSpan`: endereço do jogo,
ponteiro local e comprimento. Todos os bytes precisam estar no formato
little-endian canônico, inclusive tags, GIF, VIF e payloads. Não passe listas
criadas pelos produtores originais u32/u64 do Xbox sem adaptá-los primeiro.
A rotina não implementa `Dma_Flush`/`Gfx_EndFrame` originais nem memória PS2.

`dr_gs_stream_vif` aceita fragmentos múltiplos de quatro bytes; chame
`dr_gs_stream_finish` para detectar payload incompleto. O interpretador VU
é global e aceita apenas um contexto ativo por vez. Uma falha é persistente
até reinicializar; dados GS/VRAM/VU podem ter mudado antes do erro. Não é uma
transação de rollback. A submissão/presentação GPU fica a cargo do chamador.

O diagnóstico sintético agora passa por CNT/TTE/DIRECT/GIF. O visualizador
real passa comandos VIF pelo consumidor antes da VU de referência; sua
geometria segue pelo visualizador separado. Nenhum desses é o Battle_Main.

Produtores nativos adaptados (0.24):

```sh
ASAN_OPTIONS=detect_leaks=0 python3 engine/tools/test_dma_producer.py /caminho/upstream /tmp/producers --sanitize
```

Esse teste compila o dma.c original separadamente, renomeia os símbolos e
executa somente seus produtores de pacotes em um host Linux little-endian.
As 1.329 comparações são contra esse código original, não contra um segundo
encoder criado para o teste. O teste requer o arquivo da revisão fixada e
verifica seu SHA-256; usa MAP_32BIT para os ponteiros de 32 bits da referência.

No XEX, `dr_dma_producer.c` é o módulo nativo. Não compile também o dma.c
original: ele define os mesmos símbolos e ainda contém MMIO/ponteiros PS2.
Apenas o subconjunto listado em dr_dma_producer.h foi adaptado. Chamadas a
Dma_AddData, BeginDirect, Alloc, Ref, Next, PutTexStrips e envio à IOP ainda
precisam de port próprio. Use as APIs explícitas de palavras nativas ou bytes
canônicos; não passe uma lista misturada esperando conversão automática.

`dr_dma_producer_open` configura dois buffers de 1 MiB e o callback síncrono.
Dma_Flush acrescenta FINISH/END e entrega a lista ao callback; só alterna os
buffers se houver sucesso. O diagnóstico usa um callback que consome a lista
no GS, deixando submissão GPU/apresentação para seu próprio loop. Essa
configuração não é a plataforma da simulação original de combate.

64 bytes de cada buffer ficam reservados para FINISH/END. GetBufSize/FreeSize
informam a capacidade útil, menor que a alocação. Um erro é persistente até
fechar/abrir: ResetBuffers descarta comandos, mas não autoriza reutilizar uma
submissão falha. O callback deve consumir/copiar tudo antes de retornar.
## Tentativa de integração original de Battle_Main

O alvo separado `make battle-link UPSTREAM=/caminho/upstream` verifica as
dependências do combate original. Ele ainda falha e não gera XEX.
Veja `engine/combat/README.md` e `RELATORIO-Battle_Main.md`.
O executável distribuído continua sendo o diagnóstico 0.24.

