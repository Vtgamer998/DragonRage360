# D11 — cenário, texturas do comum 3 e áudio

**O pedido completo ainda não está concluído. Battle_Update e Battle_Main
não foram executados. O ciclo Battle_Load/Battle_Init completo, a cena/HUD
e a apresentação GPU continuam pendentes. Marcador 2 e NOT_READY mantidos.**

## Alterações implementadas e executadas em CPU

Preparação nativa das vinte seções MAPD, com ponteiros próprios, números de
32/16 bits e transformação de coordenadas conforme os leitores originais.
Cores, curvas e dados opacos são preservados. Não se troca a ordem dos bytes
de todas as palavras de um arquivo indiscriminadamente.

Caminhos EVPp: cabeçalhos, ponteiros e posições preparados; outros bytes de
cada ponto continuam opacos. Pontos MEF0 opcionais preparados. O grafo de
navegação tem posições, objetos e oito links de 16 bits por nó validados.
O gerenciador original recebe esses dados e aloca suas listas. O cenário
usado tem 165 nós e cinco caminhos. Sua liberação é recusada enquanto o
navegador o referencia. A busca pública de rotas continua dependente da
inicialização integral do cenário.

O cenário NÃO está pronto para renderização: ainda faltam seus modelos,
animações, física, temporizadores e recursos internos completos. A preparação
de metadados não remove NOT_READY.

O comum 3 ganhou uma representação própria de **111 recursos TexFile,
totalizando 154 imagens**. Cada recurso aceito passa pelo decodificador e
conversor existente, com descritores numéricos nativos e payloads canônicos.
O getter original BtlScene_GetCommonEntry fornece essas representações; o
consumidor original EftTexSet_Load32 lê os descritores e TEX0 preparados.
Os **outros 526 recursos permanecem indisponíveis** nesse caminho: incluem
formatos não preparados e candidatos não aceitos pelo conversor. Não se
publica um bloco little-endian cru como se todos os seus recursos fossem
nativos. Esse subconjunto não inicializa a cena ou seus efeitos.

BGM/streams ADX da partida não foram iniciados nesta sessão.

Áudio: bancos com cabeçalho externo nativo e HD/seq/ADPCM canônicos, com
limites de arquivo, capacidade, amostras e metadados validados. Os leitores
originais recebem metadados preparados. A sessão executa Snd_Init,
Snd_LoadBank, transferências adiadas de Snd_Update, fila de reprodução e
Snd_Term. Usa bancos comum/cenário e vozes dos personagens 0/1.

Corrigido Snd_Term: descarrega bancos ocupados antes de terminar o RPC e
libera o bloco RPC, que o código anterior deixava alocado. O launcher solicita
uma voz do lado 0 e atende o mixer durante os quadros da interface. Saída
audível no Xbox ainda não foi verificada.

## Testes recuperados e repetidos

Linux i386 sob QEMU, catálogo original e callbacks reais, UBSan/bounds:

- Vinte seções MAPD comparadas com BtlStage_BindData original em uma cópia
  independente. Sete corrupções de dados/pais/caminhos/grafo recusadas.
- Binder, consultas de nó próximo e encerramento de navegação originais;
  cinco caminhos publicados sem declarar o cenário completo.
- 111 recursos/154 imagens do comum 3 entregues ao getter original e a
  EftTexSet_Load32; ponteiros e TEX0 conferidos após liberar a origem.
  Índices inválidos retornam NULL no caminho nativo.
- Dois ciclos de objetos, BtlChar/BtlAi, navegação, áudio e entrada;
  heap do motor restaurado exatamente depois de cada ciclo.
- Quatro bancos reais carregados com transferências adiadas, metadados e
  bits de loop comparados aos leitores originais. Cabeçalho truncado,
  máscara, tamanho e capacidade inválidos recusados.
- **192 blocos de 256 amostras e 60.768 valores PCM não nulos** nos canais
  estéreo; valores finitos em [-1,1], demais canais zerados.

O dispositivo SDK do teste é um coletor PCM síncrono simulado. O sampler,
o mixer, os comandos originais e as transferências IOP/SPU são executados em
CPU. Isso não prova saída física, PowerPC, GPU, uma partida completa ou
sincronização com mandíbula/lábios. Os blocos de áudio não são quadros de
Battle_Update. Mantêm-se as exceções documentadas de sanitização da matemática
newlib de terceiros.

Evidências CPU em checkpoint-D11-CPU. Novo default.xex D11: 13.705.216 bytes, SHA-256 157f37311c29d9e90d2a3ccc84ba1ed4b6cf333657f2f8b4dfdfeaab2f6444cf.
Vínculo PowerPC: 405/405 unidades, zero símbolos pendentes e zero conflitos
ABI. Catálogo final: 33 regiões e 2.738 operações verificados. Integridade
XEX2/PE e hashes da imagem: PASS. Verificações C do toolchain xsdk: PASS.
Tabelas PS2 antigas continuam isoladas: forçá-las expõe os 103 símbolos
não implementados; o probe Game_Main continua com 69 nomes ausentes.
O D10 anterior está em diagnostico-anterior-D10/default.xex.
Evidências finais em checkpoint-D11-final.

## O que ainda falta do pedido

| Parte | Situação |
|---|---|
| Battle_Load/Battle_Init | Ciclo integral não integrado. |
| Battle_Update/Battle_Main | Não executados; bloqueio preservado. |
| Cenário | Dados de MAPD/caminhos/grafo preparados; cenário gráfico completo ainda ausente. |
| Comum 3 | Subconjunto de texturas integrado; 526 recursos indisponíveis. |
| HUD, cena e apresentação | Não concluídos. |
| Skinning/pixels da batalha, mandíbula/lábios, auras/efeitos | Sem comprovação de apresentação GPU. |
| Áudio | Ciclo e mixer testados em CPU; saída no Xbox e sincronização facial não comprovadas. |

Sem execução em Xbox ou Xenia. Recursos reais utilizados nos testes não
acompanham o pacote. O XEX usa o SDK real; o coletor simulado é exclusivo do
teste host. Não force Battle_Main retirando os bloqueios.

## Reprodução

Upstream 3ca419c7966b11438e2dc3c9a7e1914f102ed545; SDK
57b02f1e60667e37d22c3aa613d23d2486514ff8. Veja BUILD.md.
test_native_managers.py conserva a ordem de argumentos D10. Além dos
recursos anteriores, requer common-3.bin, sound-common.bin, sound-stage.bin,
voice-left.bin e voice-right.bin. Bancos USA: IDs 0x14A, 0x14E, 0xBDA, 0xBDB.
Esses arquivos devem vir da ISO local e não estão incluídos.
