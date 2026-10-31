# Renderização experimental 0.18

**A renderização solicitada ainda não está completa.** Esta atualização cria
um GsBackend experimental com callbacks reais e registro no código do motor,
e acrescenta DATE/FBA, alfa auxiliar, snapshots, targets amostrados e
restauração de stencil ao núcleo nativo. A iluminação original, VU/GPU,
auras, explosões e outros passes continuam pendentes. Não existe luta.

O backend e a captura original ainda não estão ligados ao XEX. O novo XEX
contém o núcleo ampliado e um diagnóstico sintético que exercita o caminho
MRT/DATE/FBA. Nenhum código novo foi executado no Xbox ou emulador.

## Backend e registro

`engine/graphics/dr_gs_backend.c` define `sXenosBackend` com a interface real
GsBackend. Implementa init/fechamento, texturas e níveis RGBA, transferência
de posse dos uploads, destruição com espera de GPU, handles com geração,
slots, pipelines, targets color/aux/depth, leitura 512x448, cópia de cor,
replay e apresentação pelo xsdk. A cópia de cor não altera o alfa auxiliar.

O patch `engine/patches/xenos-backend-registration.patch` registra o objeto
em GsGpu_Init sob DR_XBOX360, seleciona Xenos e impede fallback para
GL/Vulkan nesse alvo. Acrescenta limpeza das alocações em falhas de init.
Foi aplicado à cópia upstream usada nos testes; o patch acompanha a entrega.
O cabeçalho de ABI usado para compilação separada está em engine/upstream/.

Os callbacks foram exercitados juntos com o corpo verdadeiro de GsGpu_Init,
extraído literalmente do arquivo modificado. As demais funções de gs_draw.c
e o resto do motor não foram compilados/ligados nesse teste. A GPU, o display,
as configurações e os globais da captura eram mocks. Isso testa seleção e
contratos, não prova integração de um frame produzido pelo jogo.

O backend rejeita frames não suportados, registra o erro e o índice do draw,
e não apresenta um frame como se tivesse sido processado. Ele permanece
parcial: suporta escala 1x, RGBA e a ponte limitada a 1.024 vértices por replay.
O núcleo agora admite 256 texturas com slots reutilizáveis. As texturas só
podem ser desenhadas após todos os níveis declarados terem sido enviados.

## Buffers, DATE/FBA e stencil

Os targets GS novos têm cor, alfa bruto separado e D24S8. Os targets antigos
de diagnóstico color/depth continuam disponíveis. A separação do alfa é
necessária: a cor usa alfa com 0x80 como opaco e pode saturar em 1.0; o buffer
auxiliar conserva o byte original, inclusive ids acima de 0x80.

O shader publica os dois outputs. O blend de cor não é aplicado ao buffer
auxiliar, e uma máscara que não escreve alfa também não altera o byte auxiliar.
DATE testa o bit 7 de uma cópia congelada. FBA faz OR 128 no byte escrito,
sem mudar o alfa usado para misturar RGB. A ponte aceita os marcadores native
5 (cópia DATE) e 4 (cópia auxiliar para pós-efeitos), além de DATE/FBA em draws
sobre targets GS. A cópia para pós-efeitos existe; seus efeitos consumidores
ainda não estão implementados.

Os targets GS usam os passes em faixas do xsdk, com color/aux/depth, para
superfícies que ultrapassam a eDRAM. A gravação termina antes de resolves,
fences e troca de target. Há espera antes de reutilizar o command buffer.
Duas texturas de snapshot 1280x1024 são pré-alocadas. O orçamento de 128 MiB
inclui seus layouts e as superfícies, com arredondamento de páginas e pico
de substituição. Não é capacidade ilimitada para todos os modos do jogo.

Na restauração, cor, alfa bruto e profundidade são repostos. O stencil é
zerado e reconstruído em oito passes mascarados. Uma view RGBA8 emprestada
do resolve D24S8 fornece o byte S, separado da amostragem numérica de depth.
A interpretação segue a ordem de bytes descrita por xsdk/src/gpu_resolve.c
e texture.c; a view não tem alocação própria e nunca é liberada como dona.
**A correspondência dessa view com os bytes reais no Xenos permanece sem
validação em hardware.** O simulador verifica os seletores dos bits, não a
leitura real desse alias nem a execução das faixas.

Targets podem ser amostrados após resolve, inclusive feedback da própria
superfície. O shader de target mantém o alfa já normalizado, evitando aplicar
duas vezes a escala das texturas PS2. A implementação atual exige Q positivo
e UVs dos vértices dentro do retângulo permitido. Coordenadas que exigiriam
clamp adicional por fragmento são rejeitadas; region clamp/repeat e todos
os padrões de feedback do GS ainda não estão completos.

## Diagnóstico acessível no XEX

Na aba Controle, X abre o diagnóstico GS; B retorna. Ele renderiza a textura
sintética de paleta para um target GS, captura DATE, desenha passes com
DATE/FBA, resolve e amostra a cor no framebuffer de apresentação. O texto
agora identifica 0.18 e default.log registra a abertura do diagnóstico MRT.
O teste não depende da ISO e não contém recursos comerciais do jogo.

Y continua abrindo o visualizador de um corpo real da ISO, em pose fixa;
X dentro dele alterna as geometrias faciais disponíveis. Esse visualizador
não passou a executar a iluminação original nem o combate.

## Testes executados

| Verificação | Resultado | Limite |
| --- | --- | --- |
| Materiais TFX/TCC/ATST | 5.760 casos do microcódigo nativo passaram | CPU xost; não rasteriza |
| DATE/FBA | 216 casos passaram | Decisão bit7 e byte auxiliar, com amostras controladas |
| Alfa de target amostrado | 16 casos passaram | Sem filtragem/resolve real |
| Seletores de stencil | 2.304 casos: todos os bytes 0..255, oito bits e seletor de clear | Não verifica alias D24S8 no hardware |
| Targets GS | Troca A/B/A, cor/alfa/depth/stencil, snapshots independentes, feedback, timeout e falha de gravação passaram com ASan/UBSan | Modelo de chamadas, não GPU |
| GsBackend/registro | Callbacks, posse, handles com geração, mips, replay DATE/FBA, present/read/copy e seleção do init passaram com ASan/UBSan | Captura completa e motor fora do teste |
| Comandos não suportados | VU rejeitado antes de desenhar, com erro e índice | Não é execução de VU/GPU |
| Regressões | Ponte antiga, 108 blends, mips, viewer e falhas/lifetime passaram | Chamadas simuladas |
| ISO real | Corpo padrão e cinco substituições faciais reabriram com 13 materiais cada | GPU simulada; sem animação/luta |
| PowerPC | Núcleo, targets, backend e ponte compilados; checks de alinhamento passaram | Execução PPC não realizada |
| XEX | Link sem símbolos pendentes e checks estáticos passaram | Não prova boot, pixels ou FPS |

A detecção de leaks dos testes sanitizados estava desligada. Não há medições
CPU/GPU no console, prova de estabilidade em partidas ou comprovação de 60 FPS.
Os shaders ampliados e os vários passes de restauração também precisam de
medição; o pacote não apresenta esses caminhos como otimizados.

## O que permanece para concluir o pedido

| Item | Situação |
| --- | --- |
| GsBackend completo | Objeto e registro implementados no código, compilados separadamente; motor/captura completa fora do XEX e estados ainda recusados |
| Stencil | Salvamento/restauração e estado de shadow disponíveis no núcleo; início de cada run e pipelines de sombras do motor ainda não integrados |
| DATE/FBA | Núcleo, ponte, snapshots e diagnóstico implementados; fidelidade de pixels pendente |
| Buffers amostrados | Cor de target e feedback com resolve implementados no subconjunto descrito; outline/depth-CLUT e demais consumidores nativos pendentes |
| Demais estados GS | Fragment depth quantizado, samplers GS, region clamp/repeat, fontes BC/ASTC e outros modos continuam parciais ou recusados |
| Iluminação original | Pendente: programas VU/GPU, normais, matrizes animadas e camada toon no replay |
| Auras/explosões | Pendentes: dados, passes e ligação ao loop original |
| Jogo e hardware | Sem loop de luta, frames reais do jogo, boot ou pixels confirmados no Xbox |

Registrar a interface não torna o renderizador completo. A ausência de teste
no Xbox não é a única pendência: ainda falta implementação e integração.

## Reprodução

```sh
# Aplicar ao upstream fixado, se ainda não aplicado:
git -C /caminho/upstream apply /caminho/projeto/engine/patches/xenos-backend-registration.patch
ASAN_OPTIONS=detect_leaks=0 python3 engine/tools/test_gs_backend.py /caminho/upstream /caminho/xsdk /tmp/backend --sanitize
ASAN_OPTIONS=detect_leaks=0 python3 engine/tools/test_gs_targets.py /caminho/xsdk /tmp/targets --sanitize
python3 engine/tools/test_gs_buffers.py /caminho/xsdk /caminho/glslangValidator /tmp/shaders
python3 engine/tools/test_gs_materials.py /caminho/xsdk /caminho/glslangValidator /tmp/materials
make XSDK=/caminho/xsdk TOOLCHAIN=/caminho/xsdk-gcc/bin/powerpc-xsdk-elf- all engine-backend-check
python3 tests/check_xex.py
```

engine-backend-check produz objetos experimentais separados; eles não são
acrescentados ao link do launcher. Os fontes correspondentes ao XEX estão
em src/, com suas cópias de referência em engine/graphics. O backend fica
em engine/graphics e usa o cabeçalho engine/upstream/gs_draw.h. Os shaders
nativos e GLSL estão incluídos. check_graphics.py também regenera os shaders
novos; depois copie os headers gerados para src/ antes de reconstruir o XEX.

Relatórios desta execução: engine/reports/0.18/.
Upstream base: 3ca419c7966b11438e2dc3c9a7e1914f102ed545, mais o patch incluído.
xsdk: 57b02f1e60667e37d22c3aa613d23d2486514ff8.
Toolchain C/bootstrap GCC 15.3.0, patches/specs xsdk e newlib instalada;
o estágio final C/POSIX não foi usado. Sem ferramentas Microsoft.

O pacote não inclui ISO ou recursos comerciais extraídos. As licenças e os
fontes do launcher, módulos novos, testes e patch acompanham a distribuição.

XEX SHA-256: fe1419f89879ae737605f2d8e733e5872883a5caf2d81d4cf0dc0b6b3bcd4b27
