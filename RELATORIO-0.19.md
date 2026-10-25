# 0.19 — captura GS original ligada ao XEX, renderização ainda incompleta

A lista solicitada **não está concluída**. Esta versão fecha a ligação de uma
parte real da renderização: o núcleo GS e a captura original entram no XEX,
registram o backend Xenos e têm um caminho de desenho acessível. Não há loop
de combate, VU em shaders ou auras/explosões originais funcionando.

## O que mudou

- O Makefile compila `engine/capture/port/src/gs/gs_core.c`, `gs_draw.c` e
  `gs_sony.c` para PowerPC e liga suas seções usadas com `dr_gs_backend.c` e
  `dr_gs_replay.c`. Os globais de captura vêm desses fontes, sem globais
  simulados no executável. O registro usa `sXenosBackend`.
- O caminho acessível é GIF little-endian → registradores/VRAM do núcleo GS →
  `GsGpu_Draw` → lista `GsDraw` → `GsDraw_XenosSubmit` → backend → XSDK.
  A entrada de diagnóstico envia dados sintéticos; não executa a simulação.
- O backend pode usar o dispositivo/present já aberto pelo launcher. Na forma
  anexada ele desenha e deixa a apresentação ao chamador, sem abrir outro
  dispositivo ou apresentar duas vezes. Fechamento espera as operações da GPU.
- GIF passa a ser lido por bytes, inclusive em endereços desalinhados. A escrita
  otimizada de VRAM que assumia little-endian fica desabilitada no PowerPC;
  os formatos CT32, CT16, T8 e T4 usam o caminho numérico de referência.
  Texturas/paletas numéricas viram bytes RGBA na fronteira do upload.
- A ponte agora aceita os 16 seletores de sampler da captura: filtro, clamp U/V
  e permissão de mips. O núcleo mantém DATE com sampler separado de ponto.
  Isso não implementa os registradores de mipmaps do PS2 nem region-repeat.
- Foram acrescentados fechamento das arenas/caches e limpeza do cache de
  pipelines para reabrir o diagnóstico. A interpolação rejeita peças vazias ou
  fora dos limites antes de indexar os vértices.

O perfil nativo preserva a composição original 4:3. Extensões opcionais do
port desktop (fontes vetoriais, PNGs do TBO e texture packs) ficam excluídas
por `DR_XBOX360`; pedido de arte TBO é rejeitado. Não são implementações dos
menus originais, do áudio ou da plataforma completa. DMA com endereços PS2,
VIF e combate não são habilitados por esse diagnóstico. Seções não usadas
são removidas do link; compilar o arquivo inteiro não significa executar todas
as funções dele.

## Como abrir

Em Xbox 360 com ambiente homebrew compatível, copie o pacote e abra
`default.xex`. Em **Controle**, **X** abre o diagnóstico MRT/DATE/FBA já
existente; dentro dele, **Y** abre o novo diagnóstico da captura original.
Ele deve desenhar um triângulo colorido, com os títulos e identificação de
dados sintéticos. **B** volta. Não exige ISO. O visualizador de corpo da ISO
continua separado e em pose fixa.

Não foi possível testar esse procedimento em console nesta sessão. Preserve
`default.log`. O desenho do triângulo apenas confirma esse caminho de captura;
não valida sombras, DATE/FBA/stencil, todos os targets nem efeitos originais.

## Evidências obtidas

- Build de todos os objetos do título com GCC 15.3.0/specs xsdk, PowerPC BE,
  `-Wall -Wextra -Werror`; captura/core/backend/replay também usam
  `-mcheck-fpr-alignment`. Link estático sem símbolos fortes/fracos pendentes.
- `tests/check_xex.py` verifica ABI 0x100, símbolos de captura/backend,
  PE PowerPC Xbox, cabeçalho SHA-1 e cadeia completa dos hashes das páginas.
- `engine/tools/test_capture.py`: compila os arquivos completos do núcleo e
  captura com o backend real. Exercita GIF desalinhado, uploads CT32/CT16/T8/T4,
  captura e replay de desenho, bytes RGBA, seletores de filtro/clamp, captura
  de FBA/DATE e marcador de snapshot, reabertura e rejeição explícita de VU.
- Testes de backend/registro, targets, replay e visualizador foram executados
  novamente com ASan/UBSan, com detecção de leaks desativada. As chamadas
  XSDK/GPU são simuladas nesses testes; não rasterizam pixels no Xenos.

Resultados atuais em `engine/reports/0.19/`; hashes do binário em
`VALIDACAO.json` e `BUILD-RESULT.txt`. Relatórios anteriores são históricos.
O teste de registro antigo continua extraindo somente o corpo de Init; o
novo `test_capture.py` é que compila a captura completa.

## Pendências exatas da solicitação

| Item | Estado nesta entrega |
| --- | --- |
| GsBackend ligado ao motor e XEX | Captura GS original e backend experimental ligados e exercitados no host; combate ausente |
| Sombras | Núcleo de stencil existente; runs originais de sombra ainda não integrados |
| Consumidores de buffers | Amostragem comum de target e snapshots existentes; outline/tabelas de profundidade/fog e demais consumidores ausentes |
| VU/GPU e iluminação original | Não implementados; VU no descriptor é rejeitado. Visualizador separado usa interpretação CPU e pose fixa |
| Demais estados GS | Ainda faltam quantização de profundidade, region clamp/repeat completo, máscaras parciais, precisões de formatos e outros estados; replay limitado a ALWAYS sem escrita Z |
| Auras e explosões | Efeitos originais não integrados; formas sintéticas não os substituem |
| DATE, FBA, stencil | Código e testes de CPU disponíveis; pixels/alias de memória/resolves/tiling não validados em hardware |

Outros limites: 1024 vértices agregados por replay, 256 texturas nativas,
128 MiB de orçamento de superfícies/snapshots. Isso ainda não acomoda uma
luta inteira. Não houve medição de memória total, CPU/GPU ou FPS no console.
A ausência de Xbox para validar não é o único bloqueio: falta implementação.

## Procedência e distribuição

Captura/núcleo: Tenkaichi3DecompAndroid revisão
`3ca419c7966b11438e2dc3c9a7e1914f102ed545`, AGPL, com alterações locais
reproduzidas no patch `engine/patches/xenos-backend-registration.patch`.
Os fontes usados acompanham `engine/capture/`; o patch também documenta o
perfil sem SDL. XSDK fixado em `57b02f1e60667e37d22c3aa613d23d2486514ff8`.
A ISO e recursos comerciais não fazem parte do pacote.
