# D50/D51 — camada xsdk, quadro de teste e present real

Branch `d50-bt3xbx`. Não altera o `main`.

## Estado

| Item | Estado |
| --- | --- |
| Camada de arquivo/memória/tempo/log (`xsdk_compat.c`) | Implementada e testada no host |
| Quadro de teste `background` + triângulo via `dr_xenos` | Implementado e testado no host (mocks) |
| `present` real com leitura de volta do front buffer (`gfx_present.c`) | Implementado e testado no host (mocks) |
| Token `d51-gfx-probe` no XEX diagnóstico | Implementado; compila no build do console |
| Execução no Xbox 360 RGH | **Não realizada** |
| Migração das ~271 chamadas D3D9 do bt3xbx | Não iniciada |

O teste host usa os mocks de `engine/tests/test_xenos.c`: ele prova a ordem de
chamadas, o preflight do `dr_xenos` e a lógica de leitura/comparação. **Não**
prova pixels de GPU real, scanout nem console.

## Rodar o teste host

```sh
make d50-probe-test
```

Saída esperada (resumida):

```text
PASS: D50 gfx probe background+triangle size=640x480 ...
PASS: D50 gfx probe background+triangle size=1280x720 ...
PASS: D51 gfx probe INIT logical=640x480 front_buffer=1280x720 frames=3; ...
D51 gfx probe: first frame presented address=0x...
PASS: D51 gfx probe PRESENT frames=3/3 ...
PASS: D51 gfx probe VISUAL center ... tolerance=1; front-buffer readback only ...
D51 gfx probe: PHYSICAL CONFIRMATION PENDING; ...
PASS: D50 milestones 1-2 and D51 present/readback path, host mocks only
```

## Token no console

Coloque **apenas uma linha** em `startup_test.txt` no diretório do jogo:

```text
d51-gfx-probe
```

ou, para o segundo passo:

```text
d51-gfx-probe-720
```

O token roda no tick 2, **depois** da checagem do primeiro pixel da interface.
Ele abre o `dr_xenos`, desenha um fundo e um triângulo amarelo, apresenta 180
quadros (cerca de 3 s a 60 Hz) e lê de volta o último quadro do front buffer.

- `d51-gfx-probe`: área lógica 640×480, apresentada no front buffer atual
  (1280×720 no console). A área fora dos 640×480 não é desenhada pelo probe e
  mantém o que o quadro anterior deixou; o fundo e o triângulo ficam dentro
  dos 640×480.
- `d51-gfx-probe-720`: área lógica 1280×720.

### O que cada log prova

| Log | Prova |
| --- | --- |
| `PASS: D51 gfx probe INIT` | `dr_xenos` abriu e a textura branca 1×1 foi criada |
| `PASS: D51 gfx probe PRESENT` | Cada `xsdk_present_begin` recebeu um buffer, `present_end` foi chamado e o `gpu_wait` passou |
| `PASS: D51 gfx probe VISUAL` | O front buffer contém o amarelo no centro do triângulo e o fundo no canto, com tolerância 1 por canal RGB |
| `PHYSICAL CONFIRMATION PENDING` | O log **não** prova que o display escaneou o quadro nem que uma pessoa viu o triângulo |

Para fechar o marco 3 ainda são necessários:

1. Rodar o XEX no RGH com o token.
2. Anexar o `default.log` com as três linhas acima.
3. Confirmar visualmente o triângulo amarelo por cerca de 3 segundos.

Sem os três, o estado continua `NOT_READY=2` e não há autorização para dizer
que o console renderiza pelo novo caminho.

## O que a camada cobre

- `D50_ReadFile` / `D50_ReleaseFile`: leitura inteira em memória, sem buffer
  parcial em erro (`-1` ausente, `-2` falha de leitura).
- `D50_Alloc` / `D50_Release`: alocação zeroada.
- `D50_NowUs`: relógio monotônico em microssegundos.
- `D50_Log`: `xsdk_log_write` no console, `stdout` no host.

Ela **não** substitui `xtl.h`, Direct3D, XInput nem threads do bt3xbx.

## Build PowerPC (ELF e XEX)

Os objetos `build/d50/*.o` já estão na lista `OBJECTS` do `Makefile`, então o
build normal e o diagnóstico os incluem. O build do título usa os mesmos pinos
do workflow `native-d46.yml`:

```sh
# xsdk (commit 8c58c7a3fb37bb261c5a11a8a1f4fa8a8bfd0d28)
# upstream do jogo (commit 3ca419c7966b11438e2dc3c9a7e1914f102ed545)
# toolchain: powerpc-xsdk-elf-gcc

python3 tools/build_d46.py \
  --xsdk            /caminho/xsdk \
  --upstream        /caminho/Tenkaichi3Decomp \
  --toolchain-prefix /caminho/bin/powerpc-xsdk-elf- \
  --plugin-include  /usr/include \
  --output          build/d51 --jobs 4
```

Saída: `build/d51/console/default.xex` e `build/d51/xenia/default.xex`, com
`static.json` e `engine-diagnostic.json`. O script roda os testes host antes e
falha se algum quebrar.

## Próximo marco (4)

1. Rodar o token no RGH e confirmar o triângulo.
2. Só então começar a substituir D3D9 → `dr_xenos` por blocos, começando por
   `Clear` (25) e `SetRenderState` (91), que são os mais frequentes.

Sem validação física, nada aqui autoriza dizer que o console renderiza.
