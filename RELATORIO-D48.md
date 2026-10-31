# D48 — XEX D47 recompilado e validação Xenia da luta

Recompilação nativa a partir dos fontes D47 (o que o README-D47 deixava pendente:
"a compilação e os testes nativos D46 documentados no README-D46 não foram
repetidos nesta revisão") e varredura de luta no Xenia Canary com as ações
que derrubavam a partida no console.

## Binários (GitHub Actions, runner ubuntu-24.04, commit `99790ae`)

O pipeline `native-d46.yml` (menu Actions) compilou os dois variantes com o
toolchain PowerPC fixado (GCC 15.3, xsdk `8c58c7a...`) a partir dos fontes no
`main` e verificou ambos estaticamente (`tests/check_xex.py`):

- **console/default.xex**: SHA-256 `68fa346a17d3c790b8f02c70bc70009687ab568c2d2e012d462d6e27564b46c8`, 3.059.712 bytes, `entry 0x822a3bd8`, título `0x44523630`.
- **xenia/default.xex**: SHA-256 `cb5e5f865efdb2ae8406ee4c8fc18434ee9fb1c4b038a1646f3f0ef9a54e3804`, mesmo tamanho e entry.
- Verificações: flag ELF32 PowerPC xsdk ABI `0x100`, sem símbolos strong não resolvidos, PE PowerPCBE / subsystema Xbox, SHA-1 de cabeçalho.
- `validation-d46/summary.json`: `result=PASS`, 20 testes host (fontes D47).

Limites habituais: XEX não executado em hardware real nesta revisão; o teste
do console é a próxima etapa (token `default.log` a partir do RGH).

## Teste no Xenia Canary (llvmpipe) com ISO real

Ambiente: Ubuntu 24.04 sem GPU física; canary por AppImage + lavapipe
(Vulkan software), Xvfb. A ISO de referência usada no teste **não é
distribuída** (`.gitignore` de `isos/`). Configuração: `--gpu=vulkan
--readback_resolve=full --apu=sdl --storage_root=... --log_file=...` (a
apresentação em janela usa a fila do representante Vulkan 1280x720).

### original-fighter (boot novo)

- `typed-data: 2738 ops validated; complete=1; commit=1`
- 27 arquivos convertidos, 0 rejeitados, 0 leituras parciais
- `D46 original loading presentation frames=100 result=0`
- fechamento: `D46 fresh close result=0 complete=1; NOT_READY unchanged`

### original-charge-check (carregar ki com GPU)

- ticks de resposta 31–46, ação 55, ki 43.480 → 56.530 monotônico
- `Battle_StepFrame frame=0/32 returned=0`

### original-actions-fast (ataques com sprite/efeito)

- `Battle_Init returned=0 read_error=0`; frames 0 e 32 com `returned=0`
- submissões: `draws=2495, CPU_vertices=2313, VU_vertices=45730, culled_strips=0`
- ações scriptadas executadas (tick 36/48, buttons 2000/8000 taxa)

### original-battle (luta completa, 321 quadros)

| Métrica | Valor |
|---|---|
| `Battle_StepFrame` | frames 0, 32, ..., 320 (11 checkpoints) **todos `returned=0`** |
| Falha DMA | `DR_DMA_INVALID` / `-420`: **0 ocorrências** |
| quadros que antes travam | **frame 88 e vizinhos (64–128) passaram** |
| VU | 4.936.635 instruções em 321 quadros |
| Desenho | stage_GPU_draws=385.736, fallbacks=0 |
| lutadores | side 0 e side 1 ativos e vinculados em todos os checkpoints |
| HUD | flags gauge/timer/notice ativos |
| tempo | 1.395,6 s de perfil (~4,35 s/quadro em llvmpipe; **nenhuma promessa de FPS** |
| fechamento | limpo entre quadros completos |

Evidência completa em `validation-d47/xenia/original-battle-321/`:
`default.log` (cópia do log do console virtual), `xenia.log`,
`probe-report.txt` (harness), PNG por checkpoint de frame
(0/32/64/96/128/160/192/224/256/288/320) e última janela.

## Interpretação

O `DR_DMA_INVALID` (-420) que encerrava a luta no **console** não se reproduz
na rota emulada com binários compilados dos fontes TR-47 atuais — nem no boot,
nem no carregamento de ki, nem nas ações de ataque, nem em 321 quadros de
luta. Isso coincide com `RESULTADO-D46.json` (`DMA_error=false`) e sugere que
o travamento no RGH no quadro 88 pertence à variante **console** do caminho de
apresentação (GPU nativa), não à lógica do produtor DMA em si.

Para o hardware, o próximo passo de verificação é repetir o mesmo roteiro
(`original-battle`) no Xbox 360 com `console/default.xex` acima e enviar o
`default.log` gerado. O hash do binário de hardware deve acompanhar o log.
