# Estado da integração

- Motor: original Tenkaichi3Decomp, fixado por submódulo; não há uma luta nova criada do zero.
- Online PC: implementação upstream preservada; o novo adaptador encaminha abrir/entrar por endereço.
- Trapaça PC: fila original de desbloqueio, confirmação preservada, recusada durante sessão online.
- Xbox: último binário D42; a integração upstream completa ainda não foi recompilada como XEX.
- Testes novos: GCC e G++, ASAN/UBSAN, chamadas de APIs com providers de teste e transformação da UI original.
- Não testado nesta atualização: compilação integral da UI, fim por KO, dois clientes reais, rollback PowerPC, GPU ou FPS no console.
- O protótipo de aceleração VU dos lutadores da tentativa D43 não é entregue como correção validada.
  Ele atingiu o limite de gravação de comandos; a D42 validada foi preservada.

Publicação remota: bloqueada por 403 Resource not accessible by integration; nenhum commit foi enviado. `publish.py` permite publicar pelo Git autenticado, preservando o histórico.

## Incorporação efetiva dos fontes de luta e menus

- Snapshot em vendor/tenkaichi-original: 566 arquivos reais conferidos por hash Git.
- 272 unidades C do jogo; 72 unidades de menus; nenhum corpo C em falta no preparador original.
- 74 unidades passaram pela checagem de sintaxe GCC no host.
- Os arquivos importados não foram alterados para simular sucesso.
- Novo alvo make original-source-audit, executável sem baixar a ISO.
- Continua pendente: integrar esse fluxo inteiro na inicialização Xbox e recompilar/testar o XEX.

## D43 — correções gráficas em fontes

Caminho GPU dos modelos 0/1 e margem/divisão pelo cursor vivo dos comandos. Testes em host passaram; não houve nova compilação PowerPC nem execução no Xbox. Veja README-D43.md. Menus no XEX, fim por KO e 30 FPS continuam pendentes.
