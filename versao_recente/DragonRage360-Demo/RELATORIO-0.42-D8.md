# 0.42-D8 — tabelas comuns, kit dos modelos e facial CPU

**Entrega parcial do pedido: ainda não existe luta jogável.** O XEX desta revisão é um diagnóstico com Battle_Main vinculado e bloqueado. BtlChar_AllocAll e BtlAiMgr_Init não foram chamados. Não houve execução em Xbox, Xenia ou outro emulador de Xbox.

## Implementado nesta revisão

O caminho typed-data lê o arquivo comum 2 e mantém cópias próprias das seis janelas que BtlChar_AllocAll usa: flags de animação, cortes de câmera, vozes, encadeamento de ataques, parâmetros do roster e prompts. As capacidades dos spans são 448/40/64/64/164/19 registros; incluem preenchimento do arquivo e não significam esse número de animações/personagens válidos. Os leitores de combate ainda não recebem essas tabelas através de gBtlChars.

A seção comum de IA foi validada e convertida: 228 passos, oito listas contendo 537 regras e 28 perfis presentes. A conversão respeita campos de 16 bits, palavras e tabelas de bytes. Os ponteiros internos são reconstruídos em destinos de 32 bits. Eles não são endereços MIPS copiados da ISO. A estrutura reconstruída foi comparada com AiThink_BindData original em teste Linux de 32 bits.

Cada modelo agora prepara oito janelas nativas, antes quatro: parâmetros, ataques, ki blasts, movimento, supers, skills, parâmetros de IA e câmera. Os quatro novos ponteiros entram no template utilizado pelo BtlObj_BindTables adaptado e pelo Setup real. Janelas: 256/7824/676/160/580/200/512/360 bytes. Os tails fora delas e campos sem leitor revisado não são declarados prontos para o combate. Em particular, copiar a parte opaca de uma estrutura não comprova sua conversão.

O modelo ativo ganhou APIs que executam as rotinas faciais originais na CPU: olho forçado/blink, seleção de estado da boca e trilha LIPS por índice. Elas rejeitam índice inválido, estado sem face e modo que recebe ponteiro arbitrário. Também reportam a recusa original de transição de um estado de boca mantida. O diagnóstico avança oito ticks e registra o estado. Não há sincronização com voz e não há envio desse estado ao renderizador da luta.

## Situação de cada tópico pedido

| Tópico | Situação nesta entrega |
|---|---|
| BtlChar e BtlAi | Permanecem sem inicialização/registro. Recursos selecionados preparados não são um gerenciador inicializado. |
| Tabelas comuns/roster/super/skill | Seis janelas do comum 2 e oito janelas por modelo preparadas. Instalação nos consumidores de combate, arquivos comuns 3/4 e recursos restantes não concluídos. |
| Boot BtlObj_Init/Term | Permanece a sessão própria de modelos da D7. Ela usa armazenamento próprio e não pode ser encerrada com BtlObj_Term original. |
| Cenário/simulação/controle/apresentação | Colisão parcial instalada com NOT_READY. Configuração, saves de execução, posicionamento inicial e demais sistemas da partida não inicializados em conjunto. Nenhum Battle_Update ou Battle_Main executado. |
| Facial/GPU | Estado facial CPU exercitado nos dois modelos. Os modelos testados não têm o flag de mandíbula animada; o teste de hold confere o valor do estado, não skinning de uma mandíbula real. Blink/lábios apresentados, vozes, auras, efeitos, skinning e pixels da GPU não comprovados. |

Não é seguro chamar BtlChar_AllocAll após este diagnóstico: ele executa ResetAll imediatamente. BtlChar_PlaceAtStart chega a BtlStage_GetStartPlace, que lê stage->data e as posições mesmo com NOT_READY; esse caminho também pode usar um resultado de chão não preenchido enquanto o cenário não está pronto. A sessão atual só contém o cabeçalho/zonas de colisão. O reset também usa a configuração completa dos membros, controles e save, e os recursos internos dos objetos. Nada disso foi substituído por sucesso vazio ou liberação da trava.

## Verificação

- Recursos de teste lidos novamente e conferidos contra o manifesto: comum 2 e os dois modelos/animações USA. Não acompanham o pacote.
- Comparação independente dos campos de saída big-endian das oito janelas de cada modelo e dos seis spans comuns mais IA; bytes não numéricos preservados. Isso valida bytes emitidos, não execução PowerPC.
- IA: 11 corrupções recusadas sem escrever no destino; capacidades menores, sobreposição e cabeçalhos truncados recusados.
- Linux 32 bits sob QEMU-i386, UBSan/bounds: equivalência com AiThink_BindData original, posse após liberar a origem, ambos os modelos com CreateChara/Setup, animações, matrizes, volumes e liberação dos pools.
- Facial CPU em cada modelo: 600 ticks de blink (576 abertos/24 fechados nesta execução), olho forçado, estado de hold em 21 graus, 200 ticks de fala e trilha 0 com 13 chaves, terminando no frame 134. Nenhum relógio de áudio participa.
- Linux 64 bits, ASan/UBSan: construção/vida útil e recusa das APIs que exigem ponteiros de 32 bits. LeakSanitizer não é usado porque não funciona neste ambiente; não se afirma teste de vazamentos.
- 403/403 unidades PowerPC; zero símbolos indefinidos e zero conflitos de ABI. Inventário explícito: 31 módulos de plataforma. As três implementações alteradas foram comparadas com os hashes da compilação final.
- Catálogo principal regenerado: C idêntico, 33 regiões, 2738 operações, 1779 bindings conferidos no ELF final. complete=true refere-se apenas a esse catálogo selecionado, não aos recursos ou ao boot.
- 546 verificações do carregador de dados, exclusões auditadas, isolamento dos 103 nomes antigos de dados, regressões do inventário e interface: PASS. O probe do Game_Main ainda encontra 69 dependências.
- XEX2/PE, ABI, imagem e hashes: PASS. gPortDataStripped permanece 2. Integridade do executável não comprova funcionamento no console.

## Arquivos e uso

default.xex identifica-se como 0.42-D8; startup_test.txt continua typed-data. Leia os novos registros `native common battle tables`, `native common AI resource` e `native CPU facial` no default.log. Erro zero significa que aquele diagnóstico terminou, não que uma partida iniciou.

Fontes já atualizados; não reaplique o patch sobre eles. O patch incremental em extras parte somente da D7. O XEX anterior está em diagnostico-anterior-D7/. Evidências desta revisão: checkpoint-D8/. O pacote não contém ISO, programas MIPS, recursos extraídos ou constantes do jogo geradas para os testes.

SHA-256 do XEX: `f6e056b983ff0459250763518c328dfc66efcdd8e423e5962bf9ea2bd991fb75`.
