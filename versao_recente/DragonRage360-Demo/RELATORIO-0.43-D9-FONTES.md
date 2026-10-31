# 0.43-D9 — fontes parciais: posições iniciais e comum 4

**O pedido completo NÃO foi concluído. Não há luta jogável nem XEX novo nesta revisão.**
O `default.xex` incluído é exatamente o diagnóstico 0.42-D8 recebido, com
SHA-256 `f6e056b983ff0459250763518c328dfc66efcdd8e423e5962bf9ea2bd991fb75`.
As alterações abaixo estão nos fontes e precisam de uma nova compilação PowerPC.
Não se deve testar este XEX esperando que execute as alterações D9.

## Mudanças implementadas

- Comum 4: conversão por campo do cabeçalho, 161 personagens e dos registros de
  itens. Inteiros de 16/32 bits e floats são convertidos; links, bytes de tipo,
  slots e campos opacos permanecem bytes. Os spans e preenchimentos são
  validados antes de escrever. A cópia tem posse própria e é liberada junto do
  comum 2. Tipos de item 0, 1, 2 e 3 são aceitos.
- Consumidores originais: `XbBattleCommon_ProbeConsumers` instala temporariamente
  as seis tabelas em uma visão de `gBtlChars` e o comum 4 em `gCommonRes`.
  Executa `BtlAnim_GetFlags`, `BtlChar_GetVoiceEntry`, `ChrTbl_GetCost` e
  `ItemSet_GetStats`. Recusa globals ocupados e desfaz a instalação antes de
  retornar. A visão tem zero lutadores; **não é inicialização de BtlChar**.
- Cenário: preparação com posse própria dos dois pontos iniciais, alternativa
  e posição especial do MAPD. Valida offsets, contagens, sobreposição e floats
  finitos; aplica a inversão Y/Z do binder original. O chão vem do percurso
  original `StgCol_QueryZone` e do teste original `StgGround_TestTri`.
  Polígonos sem colisão e faces que não apontam para cima são filtrados.
- A adaptação do código original encaminha `BtlStage_GetStartPlace/GetPlace`
  para esse caminho quando o cenário pertence ao port. Fora dele, recusa
  cenário ausente/não pronto/sem parâmetros, com saídas zeradas e zona -1.
  A nova rotina também recusa ausência de chão. Não remove `NOT_READY`.
- Acrescentada verificação de alinhamento de 16 bytes nas janelas nativas de
  nós, polígonos e vértices, antes de usar os leitores tipados.
- O launcher fonte passa a carregar comum 4, chamar o probe dos consumidores
  e registrar os pontos iniciais. A limpeza após erro agora desinstala o
  cenário antes de tentar liberar sua memória.

## Verificação efetuada nesta revisão

Todos os dados desta nova verificação são **fixtures sintéticos**, gerados
pelos scripts a partir dos formatos documentados. Nenhum recurso real da ISO
foi retestado na D9. Resultados anteriores da D8 continuam históricos.

1. Linux 64 bits: oráculo independente verifica 2.455 campos numéricos do
   comum 4 em big-endian e a identidade em little-endian. Dez corrupções,
   capacidade insuficiente e sobreposição origem/destino são recusadas sem
   alterar o destino.
2. Linux 64 bits, ASan/UBSan: seis consultas de posicionamento atravessam os
   leitores originais de colisão/triângulo. A origem é sobrescrita e liberada
   antes das consultas. Testados lado inválido, três corrupções MAPD,
   ausência de chão, despacho nativo e desinstalação. LeakSanitizer desativado;
   não se afirma ausência de vazamentos no gameplay.
3. Linux i386 sob QEMU-i386: leitores originais consultam 414 flags de
   animação, 57 entradas de voz, 161 personagens e quatro itens sintéticos de
   tipos diferentes. Duas instalações/limpezas, recusa de global ocupado e
   posse das tabelas após sobrescrever a origem passam. O allocator deste
   teste é um arena de teste; não é o heap do Xbox nem teste de BtlChar reset.
4. Inventário dos 32 módulos da plataforma, recusas de módulos extras e
   cabeçalho sombreado: PASS. Testes de interface existentes: PASS.

Não houve compilação/link final PowerPC, execução em Xbox/Xenia, teste de
pixels, áudio de combate nem execução de `Battle_Update/Battle_Main` nesta
revisão. A preparação do despacho original foi conferida no código gerado;
o frame original completo não foi executado. O binário D8 permanece intacto.

## Situação da lista solicitada

| Pedido | Situação real após estas alterações |
|---|---|
| BtlChar / BtlAi | Reset e inicialização completos ainda ausentes. Não chamar `BtlChar_AllocAll` após o diagnóstico. |
| Instalação das tabelas | Probe temporário em leitores originais implementado; instalação permanente do combate ainda ausente. Comum 4 implementado; comum 3 e recursos restantes pendentes. |
| Cenário | Posições iniciais e chão implementados no caminho nativo. Configuração da partida, save de execução, recursos internos, render e física do cenário não concluídos. |
| BtlObj_Init/Term | Continua a sessão própria D7/D8. O ciclo original completo não foi integrado. |
| Simulação e controle | Nenhum frame original da partida foi executado. Marcador do XEX continua 2. |
| Facial e GPU | Nenhuma nova comprovação de voz, blink/lábios apresentados, mandíbula, auras, efeitos, skinning ou pixels. |

A presença de uma posição válida não prepara os demais leitores de cenário,
nem fornece objetos/partida/save ao reset. O probe temporário não deve ser
transformado em autorização para iniciar Battle_Main.

## Reprodução

Com os fontes upstream fornecidos (ou o checkout fixado descrito em BUILD.md):

```sh
python3 engine/tools/test_d9.py /caminho/upstream /tmp/d9-synthetic
python3 engine/tools/test_d9_consumers32.py /caminho/upstream \
  /tmp/d9-synthetic/common4.bin /tmp/d9-consumers32 \
  --runner /caminho/qemu-i386-static
python3 engine/tools/test_platform_sources.py /tmp/d9-inventory
make host-test
```

O segundo teste requer suporte i386 no host ou o runner. Quando ele não está
presente, registra COMPILED_NOT_EXECUTED, sem declarar sucesso em execução.
Os scripts geram seus próprios fixtures e não dependem de recursos extraídos.
Para produzir XEX novo ainda é necessário o toolchain xsdk e executar o build
PowerPC com catálogo tipado e as verificações da D8. Nenhum `make all` isolado
comprova que o combate foi integrado.
