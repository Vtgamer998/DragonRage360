# DragonRage360 0.34-L1 — vínculo de Battle_Main fechado

## Resultado deste pacote

O alvo de combate passou de 103 símbolos pendentes para ZERO. Compilaram
396/396 unidades PowerPC, sem conflito ABI. Battle_Main agora entra tanto no
ELF estrito quanto no novo default.xex de diagnóstico incluído neste ZIP.

Isso fecha o vínculo desse alvo; NÃO inicializa uma luta. O XEX usa a entrada
do diagnóstico, conserva o teste gráfico automático e não chama Battle_Main.
Os dados do programa ainda estão em forma de skeleton, sem manifesto tipado
completo. Não execute Battle_Main sobre essas tabelas zeradas. Game_Main não
está neste XEX. Nenhum código novo foi executado em Xbox real ou emulador.

## Por que 103 nomes desapareceram juntos

A contagem anterior incluía referências de blocos monolíticos de dados das
bibliotecas antigas do PS2. Quando um dado de jogo mantinha a seção .data,
o linker mantinha junto tabelas de DVD/CRI, runtime e libc do PS2, embora o
port use os adaptadores nativos de arquivo, áudio ADXT/SFX e libc do SDK.

A correção separa dois blocos completos em seções próprias:

| Skeleton público | Separação | Bytes preservados no objeto |
| --- | --- | ---: |
| cod_1C3400.data.s | dados do jogo até D_002C7100; restante como biblioteca PS2 | 163660 |
| cod_1F44C0.rodata.s | constantes/tabelas da biblioteca PS2 em seção própria | 30952 |

Nenhuma função foi substituída por sucesso vazio. Não foram escritos 103
wrappers fictícios e não foi desativada a verificação de indefinidos. Os
callbacks antigos continuam NÃO implementados; deixaram de ser requisitos
do alvo de combate porque suas tabelas não têm consumidores nativos nele.

Todos os bytes, posições relativas dos rótulos e relocações dos dois objetos
foram comparados com os originais. Foram preservados 1543 rótulos e 1871
relocações, normalizando apenas o nome privado de uma tabela de libc.
O linker normal, com --gc-sections, descarta os blocos sem referência.
As tabelas não foram apagadas dos fontes nem dos objetos de entrada.

O build audita todos os objetos compilados antes do link. Qualquer referência
nativa nova para os blocos de biblioteca provoca erro e exige revisão.
Uma mudança no skeleton upstream também é recusada, em vez de adivinhar a
posição de corte. Preservamos o layout contíguo de todo o prefixo do jogo;
não dividimos arrays ou estruturas indiscriminadamente em cada rótulo.

Essa auditoria não prova que um ponteiro bruto, ainda não descrito no futuro
manifesto de dados, nunca apontará para um serviço legado. Esse caso deve ser
resolvido pelo manifesto/boot antes de chamar a simulação. Link não é prova
de execução nem de completude do jogo.

## Correção adicional de libc

O skeleton definia _ctype_, que capturava as referências dos macros nativos
de classificação de caracteres e deixava esses macros usando uma tabela
vazia do PS2. Agora a tabela antiga é preservada como __xb_ps2_ctype_table;
o símbolo _ctype_ vem da libc real do SDK. O teste confere no ELF a tabela
nativa de 257 bytes e as classificações de maiúscula, minúscula e dígito.
Isso é necessário também para o adaptador de nomes/caminhos, não só para
fazer desaparecer um erro do linker.

## Testes feitos

- Vínculo estrito com Battle_Main forçado: PASS, zero indefinidos e zero ABI.
- Preservação exata dos dois objetos de dados: PASS, incluindo relocações.
- Forçar explicitamente D_002D3FF8 e D_002FAA08: o link volta a FALHAR nos
  mesmos 103 símbolos. Confirma que não há stubs ou opção de ignorar erros.
- Introduzir um novo consumidor nativo de tabela legada: a auditoria recusa.
- Alterar o skeleton upstream: a transformação recusa o layout não revisado.
- UI no host: menus, biblioteca vazia, navegação, opções, modal e saída passam.
- Novo XEX: ELF32 PowerPC/ABI XSDK, nenhum símbolo indefinido forte OU fraco,
  PE PowerPCBE, subsistema Xbox, SHA-1 de cabeçalho e cadeia de hashes das
  páginas verificados. Battle_Main e o sampler nativo de efeitos estão no ELF.

Os testes anteriores de SFX/ADXT com ISO real e ASan/UBSan acompanham o pacote;
o sampler não mudou nesta etapa. Não houve teste auditivo novo no console,
validação de pixels de combate, partidas ou medição de desempenho.

## Tentativa além do alvo de combate

Também foi feito um vínculo forçando o Game_Main original. Esse caminho
continua falhando em 69 nomes, listados em checkpoint-simbolos/tests/
native-data-tests.json e game-main-probe.txt. São outra superfície: serviços
de menus/tradução/fontes, personalização de músicas/cenários do PC, lobby/
rede/configurações e a biblioteca de memory card usada pelo fluxo original.
Não foram escondidos com funções vazias. O perfil de combate fixo não chama
esse fluxo de título/menus; ele precisa de um boot nativo mínimo próprio.

O construtor do diagnóstico também não força XbOverlay_PrepareBuild, pois
preparar o catálogo de menus original retém Progress_Main e esses serviços
incompletos. O código permanece nos fontes; o novo XEX não promete abrir os
menus originais. A função de restore retida recusa uso sem preparação.

## O que ainda impede a luta

1. Manifesto tipado completo dos dados usados pelo programa: unidades endian,
   estruturas mistas, ponteiros nativos e dados dos dois lutadores/cenário.
2. Boot nativo que inicialize heap, estado do jogo e recursos de uma luta fixa
   antes de chamar Battle_Main. Ter a função no XEX não prepara seu estado.
3. Integração dos produtores de pacotes, controles, animação, IA, colisão,
   VU/skinning e renderização no loop real; validação com recursos da ISO.
4. Testar o diagnóstico novo e depois o combate em hardware, corrigir falhas
   reais e medir áudio/GPU/CPU. Online, modos e fluxo completo vêm depois.

## Instalação e identificação do binário

Extraia para uma pasta nova no Xbox e use o default.xex dessa pasta. Mantenha
sua ISO na pasta de ISOs já configurada ou ajuste o caminho conforme antes.
O pacote não inclui ISO nem recursos comerciais extraídos. O teste de recurso
continua usando pzs3us1/01431.bin 0 1; startup_test.txt continua graphics.

A interface deve mostrar 0.34-L1. O default.log novo começa com:

```
DragonRage360 0.34-L1: native diagnostic; Battle_Main linked at ...
engine: data_stripped=2; combat boot BLOCKED: typed main-data manifest and battle initialization missing
```

Não é uma mensagem de falha de link: informa que o diagnóstico não iniciou
uma luta com dados ainda ausentes. O diagnóstico HW1 anterior foi mantido em
diagnostico-anterior-HW1/default.xex como opção de retorno.

## Fontes e reprodução

Esta entrega reúne os fontes atuais na raiz do projeto. Não é preciso aplicar
o patch sobre eles. extras/fixed-battle-min-INCOMPLETO.patch é uma alternativa
cumulativa para reconstruir a mesma árvore a partir da base 0.33 original;
não aplique esse patch outra vez na árvore já atualizada deste ZIP.

Upstream: 3ca419c7966b11438e2dc3c9a7e1914f102ed545.
SDK: 57b02f1e60667e37d22c3aa613d23d2486514ff8.
Use o compilador XSDK com a mesma ABI e os cabeçalhos de plugin GCC disponíveis.

```
make engine-linked-diagnostic XSDK=/caminho/xsdk TOOLCHAIN=/caminho/powerpc-xsdk-elf- UPSTREAM=/caminho/upstream
```

Se os cabeçalhos de plugin estiverem fora do caminho padrão, passe
PLUGIN_ARGS="--plugin-include /caminho/include --plugin-include /caminho/include-arquitetura".
O binário desse alvo fica em build/engine-diagnostic/default.xex.
O alvo comum make all gera apenas o diagnóstico de componentes, sem o motor;
use o alvo acima para reproduzir o XEX de vínculo incluído.

Os relatórios atuais ficam em checkpoint-simbolos e VALIDACAO.json. Os antigos
relatórios/checkpoints foram mantidos como histórico, mas suas contagens e
afirmações de Battle_Main fora do XEX estão substituídas por este relatório.
O código base mantém a licença AGPL e os avisos dos componentes utilizados.
