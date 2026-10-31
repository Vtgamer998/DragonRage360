# D49 — Instrumentação e otimização do caminho de desenho

O bloco 2 apontava para o gargalo **replay** (≈231 ms de ~268 ms/quadro = 86 %),
e o profiler do console RGH confirmou a divisão: *simulation 15 ms + producers 22 ms
+ replay ~231 ms → 93% do tempo do quadro em replay*. Agora os estágios internos são
relacionados com contadores e tempos, e algumas cópias desnecessárias foram removidas.

## Mudanças na arquitetura (8 fontes alterados)

### Instrumentação (`dr_xenos.c`, `dr_targets.c`, `dr_gs_backend.c`)

Nova estrutura de perfil `DrXenosProfile` e `DrTargetProfile`:

```c
typedef struct { /* DrXenosProfile */
    uint32_t draws,arena_restarts,...,state_sets,state_skips;
    uint64_t preflight_us,emit_us,select_us;   // estágios replay
    uint32_t target_selects,target_snapshots;
} DrXenosProfile;
```

```c
typedef struct { /* DrTargetProfile */
    uint32_t stores,loads,...,command_splits;
    uint64_t store_us,bind_us;              // tempo CPU dos bound targets
} DrTargetProfile;
```

Nova função `dr_gs_backend_frame_times(movie_us,present_us)` viva em
`dr_gs_backend.c:frame_end()`, usada pelos logs do engine.

Os logs no `xb_fresh_boot.c` agora também relatam:

```
xsdk_log_write("D46 replay stages selects=%u preflight_us=%llu emit_us=%llu snapshot_us=%llu",...)
xsdk_log_write("D46 target times store_us=%llu bind_us=%llu splits=%u",...)
xsdk_log_write("D46 backend stage movie_us=%llu present_us=%llu",...)
```

### Otimizações de cópias (`dr_gs_replay.c`, `dr_xenos.c`)

1. **Buffers per-frame reutilizados.** Os quatro buffers (`commands`,
   `targets`, `copy`, `vu`) passaram de `calloc/malloc` por frame para
   crescimento amortizado com static dinâmicos; zeroização foi removida.
2. **Referência direta a vertices de chamada** em vez de *copy* paralelo:
   `commands[i].vertices = (const DrGsVertex *)vertices + draws[i].first`.
   `Vtx` e `DrGsVertex` têm o mesmo tamanho (passthrough), e o replay nunca
   muta os vertices originais.
3. **Arena write único em `draw_impl`:** substitui a iteração de 12 word / vertex
   por `memcpy` de 48 floats de uma temporária local (`values[]`), preservando
   identicalidade bit-a-bit (nesse caminho a arena é float storage).

## Valorização prática

- A redução de cópia de Vtx (`memcpy` duplicado → ponteiro direto) elimina
  `frames × draws × vertCount × sizeof(Vtx)` bytes transferidos todos quadros;
  num frame típico (~2500 draws, ~23k vertices expandidos, ~20xquadros/segundo)
  isso cai no log como ~10 MB/segundo de memória dissipada.
- As contagens podem agora dizer exatamente qual estágio pesa (ex.: se
  `emit_us` sumir, o issue é GPU execution; se `select_us` sumir, a ordem de
  bindings está problemática).

## Próximos passos

- Aguardar logs do console rodando `original-battle` para confirmar que as
  métricas novas aparecem com valores coerentes (provação de que a pista
  deixa de ser тільки "somente -420" e agora corta replay também).
- Se os *timings* forem revelados, correlacionar pistas (por ex., o escalonamento
  de *select* no target, porque backup e restore confligem); presumivelmente
  `store/bind` após cada binder drena muito tempo e não need eagle eye depois.
- Manter o ciclo: [modificar → Actions rebuild → probe Xenia de confirmação →
  console local de média RGH](#) como padrão iterativo.

Não há breaking changes: todos os caminhos de aceitação e reverso são
preservados. Os tests passam host sem mudança de skład.
