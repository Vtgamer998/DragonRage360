# Atualização 0.13: formato dos 2722 blocos identificado

Os blocos antes chamados de texturas não interpretadas são listas DMA/VIF de
geometria facial. O leitor foi implementado e passou nos 2722 blocos reais.
Os 173798 UNPACKs passaram em transferência VU isolada, sem executar MSCNT.
A varredura de animações e esqueletos também passou novamente.
Leia LEITURA-FACIAL-0.13.md e engine/reports/0.13 para evidência e limites.

Isso ainda não liga a geometria ao renderizador nem cria luta jogável. O XEX
continua exatamente o diagnóstico 0.9. Não houve execução no Xbox.

--- Histórico anterior; a classificação de textura dos 2722 blocos foi corrigida acima ---

# Testes com recursos reais da ISO — 0.12

O teste de leitura e conversão em CPU passou. Não é um teste de luta ou execução
do XEX no Xbox. O binário distribuído permanece exatamente o diagnóstico 0.9.

| Verificação | Resultado |
| --- | --- |
| Entradas AFS com limites válidos | 68601 em 3 arquivos |
| Catálogos de animação percorridos | 161 |
| Slots BPE descompactados | 66654 |
| Animações não vazias validadas | 57259 |
| Comparações semânticas após conversão BE | 57259 aprovadas |
| Decodificações comparadas com referência upstream | 336 aprovadas |
| Slots com descompactação vazia | 9395, contabilizados separadamente |
| Arquivos de modelos/skins percorridos | 1288 |
| Marcadores exatos de skins ausentes | 590 |
| Cadeias de esqueleto validadas | 698 |
| Nós com campos de 64 bytes comparados após conversão | 43121 |
| Estruturas de arquivo de textura reconhecidas | 2148 |
| Outros blocos de textura ainda não interpretados | 2722 |
| ASan/UBSan em recursos reais | Aprovado |
| Casos de truncamento de uma animação real | 317 |

A comparação semântica lê cabeçalhos, eventos, shapes, tracks e chaves de
animação antes e depois da conversão, com a ordem de bytes correspondente.
Os campos são comparados como bits, sem arredondar floats. O teste de esqueleto
cobre só o núcleo de 64 bytes por nó; bounds e streams VU não estão convertidos.
Não foram decodificados pixels reais de textura nem executados shaders.

A primeira varredura tratava blocos que descompactam para zero bytes como
animações inválidas e os marcadores de skins ausentes como modelos incompletos.
A classificação foi corrigida apenas para essas formas verificadas. O relatório
inicial foi preservado para mostrar a origem dos números; os relatórios atuais
não contam esses espaços como animações ou modelos aprovados. Não foi necessário
alterar os conversores de produção nesta rodada.

ASan/UBSan foi executado em programa C independente usando os mesmos módulos
C do projeto e os mesmos arquivos da ISO. Leak scan ficou desativado pela
restrição de /proc; não se afirma que vazamentos foram testados.

## Reproduzir

Na pasta da demonstração, com GCC instalado e o upstream na revisão
3ca419c7966b11438e2dc3c9a7e1914f102ed545:

```sh
python3 engine/tools/test_real_iso.py /caminho/game.iso /caminho/upstream /tmp/real-iso

gcc -std=c17 -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer -Iengine/resource engine/tests/real_iso_sanitize.c engine/resource/dr_disc.c engine/resource/dr_container.c engine/resource/dr_bpe.c engine/resource/dr_anim.c engine/resource/dr_skeleton.c -o /tmp/real-iso-sanitize
ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 /tmp/real-iso-sanitize /caminho/game.iso
```

O teste abre a ISO somente para leitura. A distribuição não inclui a ISO,
recursos comerciais extraídos ou dados das animações; inclui somente os fontes
do teste e relatórios com contagens e limites.

## O que este resultado não comprova

Não houve execução do XEX, GPU, VU integrado, loop de combate, colisão/física,
IA, áudio, saves em jogo, online, rollback, NAT ou medição de FPS. O ambiente
não executou Xbox real nem Xenia. A conversão de bytes BE foi verificada em
CPU little-endian, não através de execução PowerPC.
