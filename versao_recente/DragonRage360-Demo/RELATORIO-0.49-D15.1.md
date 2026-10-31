# DragonRage360 0.49-D15.1 — continuidade da análise

## Estado herdado do D15

No QEMU i386, dois ciclos criaram e liberaram as tarefas originais de efeitos dos dois lutadores; a cena executou seis pares de Update/PostUpdate. No Xenia Canary 291f0c9, a variante D15 do XEX criou as tarefas e registrou ao menos 60 pares de atualização da cena em CPU. O pixel comprovado pertence à interface de diagnóstico.

## Trabalho desta etapa

Confirmei no código original que `BtlObj_BindTables` mapeia as entradas 1 a 5 do segundo arquivo de animações para os cinco slots de técnica do objeto, enquanto a entrada 6 é o gerenciador de efeitos já preparado no D15. `EftShotChar_Init` só cria a tarefa de um slot quando seu ponteiro de pacote existe. Sem ele, monta parâmetros de reserva. Portanto, a criação de tarefas de personagem no D15 não comprova efeitos de técnicas.

Adicionei `tools/inspect_technique_packs.py`. Ele valida limites, monotonicidade e tags da tabela de seis entradas e resume contagem/tamanho/formato das cinco entradas internas sem executar código do jogo ou despejar recursos. A checagem com contêiner sintético cobriu slot vazio, tabela truncada e offsets inválidos. Uso com os arquivos externos: `python3 tools/inspect_technique_packs.py --depth 2 /caminho/left-effects.bin /caminho/right-effects.bin`.

O workspace temporário foi limpo entre sessões. Restaurei o projeto D15, mas seus arquivos reais de teste e a ISO não integram o pacote por direitos autorais. Por isso o inspetor ainda não foi executado contra as dez entradas reais e não há XEX novo nesta etapa.

## Bloqueios mantidos

Os dez pacotes de técnica (cinco por lutador) ainda exigem conversão validada antes de ligação ao objeto. `Battle_Load/Battle_Init`, `Battle_Update/Battle_Main`, as entradas 0x23D e 0x23E do comum 3, HUD completo, desenho da cena/luta na GPU, skinning, mandíbula/lábios, auras, efeitos e saída audível continuam sem comprovação. O marcador NOT_READY permanece em 2. A variante Xenia D15 é o último XEX testado e seu log está em `checkpoint-D15-final/`.
