# Leitura dos 2722 blocos — fontes 0.13

A classificação anterior estava errada: os 2722 blocos não eram texturas.
São listas DMA/VIF de geometria facial presentes nos slots examinados dos
modelos. A referência é BtlObj_BindTables -> mdl.mouthA[] e o leitor VIF de
gs_core.c, revisão 3ca419c7966b11438e2dc3c9a7e1914f102ed545.

## Implementado

`engine/resource/dr_face_vif.c/.h` lê o formato observado com bytes LE explícitos:
DMA RET inline, palavras VIF dentro da tag, payload, guard END e padding zero.
Aceita NOP, UNPACK V4_32 e MSCNT. Retorna erro para formatos ou comandos que
não implementa; não percorre endereços externos de DMA. Comprimentos, NUM=0
(256 vetores), limites e guard são checados antes de chamar qualquer visitor.
O código mantém payloads canônicos LE e não reloca ponteiros dentro do arquivo.

O scanner de ISO passou a chamar esse leitor para os blocos que não são
TexFile, em vez de contá-los como texturas não interpretadas. Nesse conjunto
específico de slots, não restaram recursos sem classificação.

## Resultado real

| Verificação | Resultado |
| --- | --- |
| Listas DMA/VIF lidas e validadas | 2722 |
| UNPACKs V4_32 | 173798 |
| Vetores de quatro palavras transportados | 5132078 |
| MSCNTs lidos, não executados | 86899 |
| NOPs | 3659 |
| Arquivos TexFile reconhecidos separadamente | 2148 |
| Recursos sem classificação nos slots examinados | 0 |
| Regressão de animações / esqueletos | 57259 / 698 aprovados |

Cada UNPACK foi também testado isoladamente na API real `dr_vu1_unpack`:
reset, CL=WL=1 e BASE/OFFSET/TOPS=0, aplicação dos bytes reais e leitura de
volta com comparação exata. Não é execução contínua de uma lista: os MSCNTs
não foram rodados e o teste não preserva as transformações que um microprograma
faria entre dois UNPACKs. O número de vetores inclui dados e constantes VU;
não é uma contagem de vértices visíveis.

Os testes passaram com ASan/UBSan e 10000 buffers hostis, truncamentos, falhas
de callback e verificações de não emitir callbacks antes de validar tudo.
A varredura completa da ISO foi repetida, sem falhas nos checks implementados.
Leak scan ficou desligado pela restrição de /proc deste ambiente.

## Reproduzir

```sh
ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 python3 engine/tools/test_face_vif.py /caminho/game.iso /tmp/face-test --sanitize
python3 engine/tools/test_real_iso.py /caminho/game.iso /caminho/upstream /tmp/iso-test
```

## Limites que permanecem

A leitura desses blocos foi implementada e validada para o formato observado.
Ainda falta executar seus microprogramas no estado completo do motor e ligar
as saídas ao GS/GPU. O leitor não é um renderizador nem um driver DMA do Xbox.
Os 2148 TexFile tiveram sua estrutura reconhecida; seus pixels reais não foram
validados no backend nativo nesta rodada. O esqueleto continua limitado aos
campos de 64 bytes por nó já suportados.

Não houve compilação PowerPC dos fontes novos, execução de XEX/GPU/Xbox,
medição de FPS ou luta. O default.xex permanece o diagnóstico 0.9. O pacote
contém fontes e relatórios; não inclui ISO nem recursos comerciais extraídos.
