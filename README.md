# DragonRage360 — D46, XEX nativo experimental

**Fontes D47:** a correção do cache de paletas adaptada do upstream está descrita em [README-D47.md](README-D47.md). Esta atualização dos fontes não inclui um XEX recompilado. As referências a executáveis e testes D46 abaixo são históricas.

**Novos XEX para RGH e Xenia. O jogo ainda não está completo e 30 FPS no console não foram comprovados.**

A D46 corrige a gravação do cabeçalho GIF da fonte no PowerPC e a perda de registros de recursos vivos após novas leituras. Corrige também a tentativa de liberar texturas antigas sob pressão e reutiliza estados gráficos idênticos. Consulte [README-D46.md](README-D46.md) e [RESULTADO-D46.json](RESULTADO-D46.json) para evidências e limites.

No RGH, use `default.xex` da raiz. Coloque a ISO em `isos/`. Na Biblioteca, A inicia a luta; X verifica a ISO. Back + Start encerra entre quadros completos. A ISO não acompanha os arquivos. NOT_READY permanece em 2.

Os relatórios D43–D45 são históricos. A base de referência contém mais serviços que a variante Xbox efetivamente integra; a presença de código de menus, rede ou saves não comprova esses recursos no console.

## Histórico de integração até D43

## Luta e menus originais agora dentro do pacote

`vendor/tenkaichi-original/` contém os arquivos reais de `src/` e `include/`,
não apenas um endereço de download: 566 arquivos verificados, incluindo 272
unidades C do jogo e 72 unidades de menus. Há o menu principal, seleção,
modos, telas de opções e o fluxo original de batalha já implementados.
O preparador original encontra os corpos C de todos os trechos que revisa.

```sh
make original-source-audit
```

A conferência passou para todos os hashes e para a sintaxe de 74 unidades
(72 de menus, entrada principal e batalha). Isso foi feito em GCC no host:
não comprova compilação PowerPC, desenho, uso do controle ou o fim de uma partida.
O fluxo original é `Game_Main -> Progress_Main -> Battle_Main`; a D43
continua no caminho experimental de diagnóstico, sem ligar os menus originais.
A cópia importada mantém o código original íntegro; a migração dos patches de
plataforma para essa revisão ainda precisa ser concluída.

## Bases preservadas

- `upstream/Tenkaichi3Decomp`: `7b43a75bd8b0aeabcce0a22c45e1ad66162ac9d7`.
  O jogo, menus, online experimental, rollback e trapaças são os do projeto de referência.
- `upstream/Tenkaichi3Decomp-xbox-base`: `3ca419c7966b11438e2dc3c9a7e1914f102ed545`.
  Base compatível com os scripts de construção e patches PowerPC da D43.
- `src/`, `engine/`, `manifesto-tipado/`: adaptação nativa Xbox D43. A publicação depende de acesso de escrita; as tentativas anteriores foram recusadas (403).
- `adaptation/`: integração dos recursos originais, validação e testes C/C++.

```sh
git clone --recurse-submodules https://github.com/Vtgamer998/DragonRage360.git
cd DragonRage360
```

O repositório remoto ainda não recebeu esta atualização. A tentativa por Git nesta sessão não encontrou credenciais de escrita; as tentativas anteriores pela integração retornaram 403.
Para publicar os fontes preparados com seu Git autenticado:

```sh
python3 adaptation/tools/publish.py /caminho/checkout-dragonrage --push
```

O script usa fast-forward, recusa checkout sujo, preserva o histórico e não inclui binários ou dados comerciais.
O pacote compilado D43 contém os novos executáveis; o arquivo de fontes separado contém código e evidências.
Para buscar as duas bases a partir deste ZIP:

```sh
python3 adaptation/tools/fetch_upstream.py
```

Um ZIP do GitHub não traz o conteúdo dos submódulos. Depois de publicada a atualização, use o clone acima ou execute
`git submodule update --init --recursive` em um clone existente.

## Online e modo trapaça

O upstream já oferece online experimental: lobby, entrada por código/endereço,
espectadores e rollback. A adaptação **reutiliza `net.c` / `net_lobby.c`**, sem
substituí-los pelo antigo protocolo de lobby `DRL1` deste repositório.

As trapaças usam a fila original `gPortUnlockAll`, consumida pela thread do jogo,
que chama `Save_UnlockAll`. Ela desbloqueia personagens, cenários, itens, músicas
e Zenni, mas também apaga os recordes; o efeito fica permanente no próximo save.
A confirmação da interface original é preservada. O adaptador recusa a operação
durante uma sessão online.

Para aplicar a integração na base de PC e compilar usando o instalador original:

```sh
python3 adaptation/tools/apply_upstream.py upstream/Tenkaichi3Decomp
cd upstream/Tenkaichi3Decomp
python3 install.py --check
python3 install.py --iso /caminho/para/sua/SLUS-21678.iso
```

Na versão de PC: abra **Tenkaichi Budokai Online** no menu original. **F1 → Cheats**
abre as trapaças; **Start + Select** também abre as configurações pelo controle.
O patch encaminha as ações existentes para as APIs originais e valida os argumentos.
Os testes de integração não substituem uma partida real entre duas máquinas.

## Estado real no Xbox 360

A D43 apresenta cenário, dois lutadores, HUD original e uma sessão local
experimental limitada a 3.600 passos, cancelável por Back + Start. A sequência
curta foi validada no Xenia: 89 passos, quatro quadros em FIGHT, encerramento seguro.
O fim natural por KO/resultado, 30 FPS estáveis e o jogo completo não foram comprovados.
O pitch dos snapshots e a vida inicial foram corrigidos; outras diferenças visuais e de combate continuam pendentes. `NOT_READY` permanece em 2.

**Online do upstream ainda não funciona no Xbox.** Transporte, interface, rollback,
representação de memória/endian e sincronização precisam ser adaptados/validados.
Um lobby UDP conectado não comprova uma luta online. A variante Xbox da nova
interface retorna indisponível enquanto isso estiver pendente.

O ZIP Linux x64 enviado é um executável para CPU x86-64 e SDL/Vulkan/OpenGL;
**ele não executa no PowerPC/Xenos do Xbox e não pode ser renomeado para XEX**.
Seu conteúdo não foi alterado nem apresentado como uma compilação Xbox.

## Testes e construção nativa

```sh
python3 adaptation/tests/run.py --upstream upstream/Tenkaichi3Decomp
python3 tools/build_d43.py --help
```

O primeiro verifica encaminhamento às APIs oficiais, argumentos, confirmação,
recusa de trapaças online, indisponibilidade explícita no Xbox e aplicação idempotente
do patch. Executado com GCC e G++, ASAN/UBSAN. Não valida GPU ou combate pela rede.
`validation-adaptation/result.json` e `RESULTADO-D43.json` detalham o escopo.
Consulte também `README-D43.md`, `LICENSES.md` e `adaptation/PROVENANCE.json`.

A distribuição não inclui ISO, dados do jogo, saves pessoais, SDK proprietário ou credenciais.
O código do port mantém a AGPL e os avisos/licenças de terceiros. Não há afiliação oficial
com o projeto upstream ou com os titulares do jogo.
