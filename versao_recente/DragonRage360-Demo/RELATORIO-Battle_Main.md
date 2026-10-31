# Battle_Main — integração ainda bloqueada

O Battle_Main original foi incluído no código-fonte deste projeto e compilado
para PowerPC. **Ele não foi ligado ao XEX nem executado.**

## Resultado verificado

- 368 de 368 unidades selecionadas compilaram: motor C, referências VU0,
  matemática portátil e estruturas de tabelas upstream.
- O linker rejeitou 221 objetos por conflito de ABI de ponto flutuante:
  motor soft-float versus SDK/renderizador hard-float.
- Restaram 187 nomes de símbolos não resolvidos. A contagem inclui funções,
  globais e referências de tabelas; não representa 187 funções faltantes.
- Nenhum substituto vazio foi gerado. Nenhum ELF de combate incompleto foi
  mantido. Nenhum XEX novo foi gerado.
- O default.xex continua sendo o diagnóstico 0.24, SHA-256
  be4c62ac7759d064bdb8a5d257d9f46161d581ce47c8d88488ab392e7cf3aac3.
- Nenhum teste em Xbox 360 ou emulador foi realizado.

## Código entregue

engine/combat/battle.c contém a implementação original, sem alterações,
SHA-256 26f212e165d747b853520f959387b5b6a3d019f2633270f70d5c6e698b90c873.
engine/tools/link_battle.py e o alvo make battle-link reproduzem a tentativa
separada do launcher. O main original é renomeado para incluir seus auxiliares.
O shim de matemática recebeu os dois protótipos float ausentes.

## Impedimentos concretos

É necessário adaptar e validar a fronteira de chamadas float, sem desativar
as verificações do linker. Também faltam os serviços reais de arquivo/áudio
CRI, heap, temporização, controles/RPC, produtores DMA e interface GsVu1.
Há referências a endereços de bibliotecas PS2 nas tabelas que exigem análise
e relocação; não podem ser convertidas em funções vazias.

As tabelas compiladas são apenas estruturas: não contêm os valores da ISO.
O carregamento, conversão endian e adaptação dos endereços PS2 não estão
resolvidos. Só depois desses serviços e dados é possível inicializar o motor
e ligar Battle_Init, Battle_Loop e Battle_Term à entrada nativa. IA, colisões,
animações e renderização ainda não foram executadas juntas.

## Evidências e reprodução

engine/reports/battle-integration/ contém a lista completa de símbolos,
comando do linker, hashes dos fontes, diagnósticos de compilação e atributos
ABI dos objetos. Veja engine/combat/README.md para reproduzir.
O pacote inclui código e o XEX anterior de diagnóstico, sem ISO ou recursos.
