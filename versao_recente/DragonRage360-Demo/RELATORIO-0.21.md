# 0.21 — consumidores de profundidade/IDs ligados à captura GS

**A solicitação completa continua pendente.** Esta versão implementa duas
classes de consumidores de buffers no backend/XEX: contorno e consulta de
paleta por profundidade ou ID de objeto. Não liga a simulação original de
combate, não executa uma luta e não valida pixels de Xbox.

## Implementado

- `native=1` da captura GS: contorno a partir dos IDs congelados. Reproduz a
  tabela do shader upstream e compara quatro vizinhos. O pass subtrai o
  contorno de RGB, preservando alfa, auxiliar, profundidade e stencil.
- `native=3`: consulta a uma CLUT real da captura, com duas fontes. A fonte 0
  calcula o índice a partir da profundidade; a fonte 1 lê o ID do snapshot
  independente de alfa bruto. O shader produz cor/alfa normalizados e o byte
  de alfa exato no MRT auxiliar, respeitando as máscaras de escrita.
- A CLUT vem de `clut_texture()` do `gs_draw.c` original. A ponte resolve seu
  handle, verifica a textura RGBA 256x1 e usa amostragem de ponto. O caminho
  novo não exige uma tabela embutida no renderizador.
- O pass resolve as escritas anteriores antes de amostrar profundidade. Os
  IDs vêm do snapshot de efeitos (`native=4`), separado do snapshot DATE e
  do auxiliar ativo. Um consumidor não pode usar uma cópia de outro target.
- O contorno limita as consultas à região válida da cópia. O snapshot tem
  alocação 1280x1024; um target menor não pode consultar pixels não copiados
  ao olhar o vizinho da borda direita.
- Snapshot ausente/dono incorreto, fonte inválida, textura CLUT inadequada ou
  um coeficiente de blend 1+C não representável por esse caminho são erros
  antes de emitir primitivas da lista inteira. A pré-validação acompanha a
  mudança de dono dos snapshots que ainda serão executados na lista.

Os shaders são adaptações dos shaders públicos `outline.frag` e `dclut.frag`
da revisão upstream fixada. O cálculo de índice por profundidade segue essa
implementação de referência do port; não foi comparado a pixels PS2/Xenos.
Fog completo do GS, demais consumidores, reinterpretações de buffers e todos
os modos de blend não são concluídos por essas duas implementações.

## Caminho no executável

Em **Controle**, **X** abre o diagnóstico GS; **Y**, dentro dele, abre a
captura original. Ela agora envia os registros que o motor usa para congelar
IDs e reconhecer o pass CLUT, chama o marcador de contorno e executa as duas
fontes de consulta. A paleta sintética é carregada pela transferência GS,
com a ordem de índices CSM1. A primeira consulta modifica o auxiliar ativo;
o contorno continua ligado à cópia congelada anterior. **B** retorna.

Isso usa a captura original e os novos consumidores nativos, mas **os modelos,
constantes e paleta desse diagnóstico são sintéticos**. Não é uma luta ou uma
sequência de sombras de personagens originais. O visualizador separado da
ISO continua em pose de referência, como na versão anterior. O procedimento
no console ainda não foi confirmado.

## Evidências

- Shaders compilados para Xenos por glslang e xost/xsc: **7.950** casos de CLUT,
  **1.280** de contorno e **4** cantos da região válida passaram no simulador
  CPU de microcódigo. Verificam índices, alfa 0..255, premultiplicação, tabela,
  vizinhos e limites. Não há rasterização, GPU ou pixels de console nesses testes.
- Captura completa + núcleo GS + backend reais: **9** ciclos com ASan/UBSan.
  A chamada de desenho recebe a profundidade D24S8 e a CLUT 256x1 corretas;
  contorno e consulta por ID apontam ao snapshot congelado, distinto do
  auxiliar ativo. Máscaras de MRT, subtração RGB e ausência de escrita Z
  são verificadas nas chamadas. As APIs GPU são simuladas.
- Falhas no último comando — inclusive snapshot ausente, dono trocado na
  própria lista, CLUT inadequada, fonte inválida e blend não representável —
  impedem a emissão das primitivas anteriores dessa lista.
- Testes anteriores de backend/registro, replay, targets e visualizador
  passaram novamente com ASan/UBSan. Detecção de leaks desativada.
- Build PowerPC BE com `-O2 -Wall -Wextra -Werror`; captura, núcleo, backend
  e ponte também compilam com `-mcheck-fpr-alignment`. Verificações ELF/XEX
  em `BUILD-RESULT.txt` e `VALIDACAO.json`.

Relatórios atuais em `engine/reports/0.21/`. Os relatórios anteriores descrevem
apenas as versões indicadas em seus nomes. Não há medição de FPS, CPU/GPU,
memória total ou estabilidade de partidas. Os resolves/restores por pass
podem ter custo significativo; não foram medidos em hardware.

## Ligação à simulação: bloqueio medido

`audit_battle_link.py` compilou o `src/battle/battle.c` ORIGINAL para PowerPC,
com o header de compatibilidade do port. Esse único objeto tem **87 símbolos
diretos** que não existem no ELF atual: carga e estado de batalha, lutadores,
IA, câmera, cenário, jobs, entrada, áudio e fronteiras de quadro, entre outros.

A lista exata e o hash dos fontes estão em
`engine/reports/0.21/battle-audit/battle-link-audit.json`. Esse número não é a
contagem de todas as dependências transitivas do motor. Compilar o objeto
não adapta endereços PS2, dados little-endian, inicialização, heap ou pacing.
`Battle_Main` não está ligado ao XEX nem foi executado. O objeto da auditoria
não entra no título; o script permite reproduzir a análise com o upstream.

Portanto, ligar apenas o callback gráfico não completa o combate. As camadas
e os dados que alimentam essas chamadas ainda precisam ser integrados.

## Pendências da lista solicitada

| Item | Estado atual |
| --- | --- |
| GsBackend completo e simulação | Captura/backend no XEX; simulação de combate não ligada, dependências e plataforma ainda ausentes |
| Consumidores de buffers | Contorno e CLUT por profundidade/ID implementados; demais consumidores e reinterpretações continuam incompletos |
| Sombras com dados reais | Projeção/stencil da 0.20 presentes; sequência completa de uma luta original não validada |
| Iluminação e VU no combate | Shaders nativos da 0.20 presentes; poses, materiais e quadro de combate originais não integrados |
| Demais estados GS | Ainda faltam amostragem 2D completa, region-repeat/mips PS2, máscaras parciais, formatos/precisões e outros estados |
| Auras e explosões | Recursos e sequências originais não integrados |
| DATE, FBA e stencil | Sem validação de pixels em Xbox ou emulador; os testes de CPU não a substituem |

Permanece o limite de 1024 vértices agregados por replay, incluindo seis por
consumidor e por limpeza de stencil; 256 texturas e orçamento de 128 MiB de
superfícies/snapshots. O backend ainda não acomoda um quadro completo de luta.
Nenhum XEX novo foi executado no Xbox ou emulador.

## Procedência

Upstream AGPL `3ca419c7966b11438e2dc3c9a7e1914f102ed545`, XSDK
`57b02f1e60667e37d22c3aa613d23d2486514ff8`. Shaders-fonte, headers gerados,
ponte, testes e scripts acompanham a entrega. Não há ISO ou recursos
comerciais no pacote. Veja `LICENSES.md` e `BUILD.md`.
