# D50 — marcos 1 e 2 (camada xsdk + quadro de teste)

Branch `d50-bt3xbx`. Não altera o `main`.

## Estado

| Item | Estado |
| --- | --- |
| Camada de arquivo/memória/tempo/log (`xsdk_compat.c`) | Implementada e testada no host |
| Quadro de teste `clear` + triângulo via `dr_xenos` | Implementado e testado no host (mocks) |
| Instruções de build PPC ELF/XEX | Documentadas abaixo |
| Execução no Xbox 360 RGH | **Não realizada** |
| Migração das ~271 chamadas D3D9 do bt3xbx | Não iniciada |

O teste host usa os mocks de `engine/tests/test_xenos.c`: ele prova a ordem de
chamadas e a validação do `dr_xenos`, **não** pixels, GPU real nem console.

## Rodar o teste

```sh
make d50-probe-test
```

Saída esperada:

```
PASS: D50 gfx probe clear+triangle size=640x480 ...
PASS: D50 gfx probe clear+triangle size=1280x720 ...
PASS: D50 milestones 1-2 (compat layer + clear/triangle probe), host mocks only
```

## O que a camada cobre

- `D50_ReadFile` / `D50_ReleaseFile`: leitura inteira em memória, sem buffer
  parcial em erro (`-1` ausente, `-2` falha de leitura).
- `D50_Alloc` / `D50_Release`: alocação zeroada.
- `D50_NowUs`: relógio monotônico em microssegundos.
- `D50_Log`: `xsdk_log_write` no console, `stdout` no host.

Ela **não** substitui `xtl.h`, Direct3D, XInput nem threads do bt3xbx. Isso é o
marco 3 e depende do backend gráfico.

## Build PowerPC (ELF e XEX)

O build do título usa os mesmos pinos do workflow `native-d46.yml`:

```sh
# xsdk (commit 8c58c7a3fb37bb261c5a11a8a1f4fa8a8bfd0d28)
# upstream do jogo (commit 3ca419c7966b11438e2dc3c9a7e1914f102ed545)
# toolchain: powerpc-xsdk-elf-gcc

python3 tools/build_d46.py \
  --xsdk            /caminho/xsdk \
  --upstream        /caminho/Tenkaichi3Decomp \
  --toolchain-prefix /caminho/bin/powerpc-xsdk-elf- \
  --plugin-include  /usr/include \
  --output          build/d50 --jobs 4
```

Saída: `build/d50/console/default.xex` e `build/d50/xenia/default.xex`, com
`static.json` e `engine-diagnostic.json`. O script roda os testes host antes e
falha se algum quebrar.

Para compilar só o caminho ELF/XEX sem os testes, use o mesmo script em um
checkout limpo; não há atalho suportado que pule a validação.

## Próximo marco (3)

1. Ligar `D50_GfxProbeRun` a um token de `startup_test.txt` no console, com o
   `present` real (hoje o probe só submete e sincroniza).
2. Rodar no RGH e anexar o `default.log`.
3. Só então começar a substituir as chamadas D3D9 do bt3xbx por `dr_xenos`.

Sem validação física, nada aqui autoriza dizer que o console renderiza.
