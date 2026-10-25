# Teste do novo XEX 0.44-D10

Este é um diagnóstico de boot CPU, ainda sem luta jogável.

1. Copie o novo `default.xex` e `startup_test.txt` para a pasta existente do
   projeto no Xbox 360. Preserve a sua ISO USA original em `isos/`.
2. Inicie pela Aurora. A interface deve identificar `0.44-D10`.
   O startup `typed-data` executa o diagnóstico na ISO selecionada.
3. Consulte `default.log`. No fim do teste devem aparecer:
   - `original CPU manager boot: error=0`
   - duas linhas `original initialized fighter`, lados 0/1,
     `HP=40000 Ki=40000 AI_type=0 AI_level=13`.
4. Se aparecer um erro, guarde o log completo. O resultado zero confirma
   apenas o boot CPU indicado; não significa que uma partida foi renderizada.

O pacote menor de atualização contém o executável e estas instruções.
O pacote completo contém também os fontes e o relatório de validação.
O XEX D8 anterior foi preservado no pacote completo em
`diagnostico-anterior-D8/default.xex`.

Ainda não são executados Battle_Update/Battle_Main. A apresentação GPU do
combate, efeitos e vozes continuam pendentes. Leia RELATORIO-0.44-D10.md.
