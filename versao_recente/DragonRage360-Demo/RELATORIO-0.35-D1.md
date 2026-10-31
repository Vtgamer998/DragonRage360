# DragonRage360 0.35-D1 — manifesto tipado parcial e carga verificável

O pedido ainda NÃO está inteiramente concluído: o manifesto não cobre todos
os dados, e os recursos dos personagens/cenário não estão todos convertidos
para as estruturas do combate. O programa continua sem iniciar uma luta.

Esta versão implementa o gerador de catálogo, a carga transacional de campos
conhecidos e as religações de ponteiros nativos. O novo default.xex contém
Battle_Main, o catálogo compilado e um teste explícito de carga parcial.
Não chama Battle_Main. O marcador de dados incompletos continua igual a 2.

## O que foi implementado

### Layouts e endianness

O gerador lê o DWARF dos próprios objetos PowerPC, produzido pelo compilador
XSDK. Usa os tamanhos, offsets de membros, arrays e tipos de ponteiro dessa
ABI, sem usar sizeof do computador host ou assumir que tudo são palavras de
32 bits. Há um witness compilado apenas para obter tipos de declarações que
o otimizador eliminou do debug: cinco classes de tarefas documentadas e os
coeficientes de seno. Esse witness não é ligado ao jogo.

São separados campos de 1/2/4/8 bytes, ponteiros de dados e funções, padding
e regiões opacas. Structs mistas geram operações diferentes por campo. Unions
cujas alternativas exigem layouts diferentes, bitfields e tipos incompletos
são recusados. Arrays de bytes globais genéricos NÃO são considerados uma
descrição suficiente para converter uma estrutura escondida neles.

O bloco VIF/VU é explicitamente mantido como bytes canônicos little-endian,
conforme sua declaração u8[] e os consumidores que decodificam LE. Não é
tratado como uma matriz de números para o CPU PowerPC.

Zeros confirmados na fonte podem ser inicializados sem adivinhar um tipo:
o formato all-zero é endian-invariante. Essas operações são distintas de
campos tipados e exigem bytes zero na validação; não classificam um blob
não zero como padding para conseguir completar a cobertura.

### Ponteiros MIPS -> nativos

Cada slot de ponteiro descrito pelo catálogo valida primeiro o valor original
da ISO. O valor que entra no destino é o símbolo nativo resolvido pelo linker,
nunca o endereço MIPS lido do disco. Os offsets de dados confirmados também
são relocados em relação ao símbolo nativo correspondente. Ponteiros sem
mapeamento e offsets para o meio de uma função MIPS não são aceitos.

O catálogo C gerado usa referências simbólicas, não uma lista de endereços
absolutos de um ELF anterior. Na verificação do ELF FINAL foram conferidos
todos os destinos e todas as 2.157 operações. Das 1.694 religações de ponteiro,
1.473 mudaram de endereço em relação ao vínculo de referência e continuaram
apontando para os símbolos corretos após o novo link.

### Bloqueio no próprio combate

A entrada nativa de Battle_Main chama XbTypedData_RequireBattleData antes de
Battle_GetWork e de qualquer inicialização da luta. Enquanto o marcador for
não zero, retorna -721 e registra a recusa, sem acessar o work. A carga parcial
não altera esse marcador. A ordem da checagem foi conferida no disassembly do
ELF PowerPC final; o teste host verifica a recusa após aplicar dados parciais.
Isso ainda não é execução desse caminho em hardware.

### Carregador transacional

XbTypedData_Load valida fontes ELF, cobertura real das seções, tamanhos,
destinos, sobreposição/ordem das operações, widths, valores MIPS esperados,
zeros obrigatórios e o checksum das operações antes de escrever qualquer
campo. Copia os descritores antes de chamar o leitor, e recusa reentrada.
Não carrega ponteiros nativos de um manifesto externo: os destinos e bindings
são metadados compilados pertencentes ao build.

Há modo de validação sem escrita. Um catálogo parcial é recusado por padrão;
aplicá-lo requer explicitamente XBTD_ALLOW_PARTIAL para diagnóstico. Essa
operação não marca os dados como completos e não libera o boot do combate.

## Cobertura atual dos dados iniciais

| Item | Resultado |
| --- | ---: |
| Regiões de dados do programa | 33 |
| Operações compiladas | 2157 |
| Bytes com campos tipados/religações | 28321 |
| Bytes adicionais com zero comprovadamente invariante | 1362 |
| Bytes inicializáveis por este catálogo | 29683 |
| Bytes .space/.long considerados nessa superfície | 35434 |
| Faixas não zero ainda sem tipo seguro | 168 |
| Bytes dessas faixas ainda não cobertos | 5751 |
| Trechos .zero do skeleton cuja fonte não é zero | 23 (62 bytes) |
| Religações de ponteiro explicitamente confirmadas | 1694 |

Esses números descrevem somente a superfície de dados iniciais analisada.
Não são uma porcentagem de conclusão do jogo. As 168 faixas incluem tabelas
de UI/pausa, estruturas/arrays opacos e constantes sem declaração suficiente.
Os 23 trechos suprimidos precisam de revisão semântica: não basta copiar de
volta bytes que o port de PC deliberadamente zerava.

manifesto-tipado/typed-manifest.json lista cada faixa com símbolo, offset e
endereço de origem, e inclui a evidência de tipos por unidade. O campo
complete permanece false. Não houve tentativa de ocultar essas lacunas.

O gerador exige os programas USA com fingerprints já verificados. Não tenta
aplicar esse esquema a outra edição ou a um programa modificado por suposição.
Os hashes e as descrições dos arquivos acompanham o pacote; seus bytes não.

## Dados dos dois lutadores e do cenário

Foi selecionado um perfil explícito de teste: personagens numéricos 0 e 1,
roupa 0, variante padrão, cenário 0, tela única e vozes padrão. Não foram
atribuídos nomes de personagens/cenário sem validar os textos correspondentes.
Os IDs vêm de battle_work.h e battle_load.c do upstream fixado. IDs de arquivo
do motor são baseados em 1; o índice da AFS usado pelo leitor é ID - 1.

Foram lidos 18 arquivos reais: common 2/3/4, boot 5, recurso de progressão 6,
os dados de membro 8 e 11, banco comum 330, banco de cenário 334, cenário 369,
modelos 1424/1434, animações 1432/1442, auxiliares 1433/1443 e vozes 3034/3035.
O manifesto de recursos registra tamanhos, fingerprints e contagens; não
inclui esses arquivos comerciais.

- Ambos os modelos têm 69 partes lidas pelo parser de modelo em estruturas
  CPU nativas: hierarquia, pais, pivôs, rest pose, flags e spans VIF validados.
  Isso é uma VIEW nativa validada, não um BobjMdl inteiramente rebased e
  instalado no estado de combate.
- Os dois arquivos de membro são TexFile; o decoder limitado existente
  validou e decodificou a primeira imagem de cada um para RGBA no teste host.
  Não foram tratados incorretamente como um array genérico de stats.
- Os dois contêineres de animação têm 440 entradas. Nos primeiros 414 slots
  de movimento de cada um, converteram-se 718 animações não vazias para BE.
  Outros 110 slots descomprimem para zero bytes: agora retornam DR_LOAD_EMPTY,
  com zero bytes escritos, sem fabricar uma animação. Nenhuma animação não
  vazia desses 828 slots foi recusada nesse teste.
- As 26 entradas restantes por contêiner e os movimentos embutidos no modelo
  não foram apresentados como conversão completa. Binders/estado do player
  não foram integrados por este teste.
- O contêiner do cenário tem 25 entradas, todas com limites verificados.
  Colisão, física, hierarquia completa do cenário e demais subestruturas
  continuam sem conversão tipada completa. Ler e validar o contêiner não
  equivale a preparar o cenário para simulação.
- Bancos de som e auxiliares tiveram seus spans/contêineres verificados; isso
  não substitui a adaptação de todos os leitores internos usados pelo motor.

O teste libera os buffers após inspecioná-los. Não deixa um BattleWork pronto,
não inicializa o combate, não instala automaticamente conversores genéricos
para todos esses IDs no adaptador de arquivos do motor.

## Testes e compilação

- ASan/UBSan passaram para o carregador tipado com os programas reais: hashes
  de todos os buffers resultantes, campos mistos, ponteiros, bytes opacos
  intocados, recusa do boot incompleto, dry-run, checksum errado, binding MIPS
  errado, sobreposição, limites de fonte e reentrada.
- Passaram os testes de leitura dos 18 recursos, ambos os modelos, imagens
  de membro, slots de animação não vazios e classificação dos slots vazios.
- O carregador BT3X anterior passou novamente em seus 546 checks.
- A UI host passou. A separação das bibliotecas antigas continua auditada:
  forçar suas tabelas ainda faz os 103 indefinidos reaparecerem.
- Compilaram 397/397 unidades PowerPC com catálogo; vínculo sem indefinidos
  e sem conflito ABI. O fluxo completo de Game_Main ainda tem 69 dependências
  em outra tentativa de vínculo; não entra neste XEX.
- O novo XEX passou nos checks ELF/ABI/PE Xbox, símbolos, SHA-1 de cabeçalho
  e páginas. A forma do catálogo final e todas as religações foram verificadas.

NENHUM desses testes é execução do jogo em Xbox/emulador. Não houve medição
de áudio, pixels de combate, CPU/GPU, FPS nem uma luta rodando.

## Teste no console desta versão

O default.xex é novo, identificado como 0.35-D1. O startup_test.txt incluído
contém typed-data. Depois da interface, o diagnóstico procura a ISO nas
pastas já usadas, escolhe a primeira na ordenação e tenta carregar APENAS os
campos confirmados do catálogo parcial. Use uma pasta nova para não confundir
esse binário com versões antigas. A ISO continua sendo um arquivo do usuário.

O log esperado em caso de sucesso inclui:

```
typed-data: 2157 ops validated; complete=0; commit=1; combat NOT started
PASS: typed main-data partial preview ... regions=33 ops=2157 complete=0 data_stripped=2; combat BLOCKED
```

O modal também informa que a carga é parcial. Isso comprovará a carga dos
campos conhecidos no console somente quando o log real voltar; ainda não
foi observado. Um erro de validação impede a escrita pelo carregador.

Para voltar ao teste gráfico, copie startup_test-graphics.txt sobre
startup_test.txt. O XEX anterior está preservado em diagnostico-anterior-L1.
O teste de animação avulsa continua configurado para pzs3us1/01431.bin 0 1.

## Reprodução e fontes

Todos os fontes atuais estão na raiz. engine/schema/typed_catalog.generated.c
é o catálogo compilado desta versão, sem valores escalares do jogo. O patch
cumulativo em extras é uma alternativa para a base 0.33 original, e não deve
ser aplicado outra vez sobre os fontes já atualizados deste pacote.

Para regenerar a partir de uma ISO USA própria:

```
make typed-data-diagnostic ISO=/caminho/jogo.iso XSDK=/caminho/xsdk TOOLCHAIN=/caminho/powerpc-xsdk-elf- UPSTREAM=/caminho/upstream
```

Use PLUGIN_ARGS com os --plugin-include necessários ao seu GCC, como no
relatório anterior. O resultado fica em build/typed-diagnostic/default.xex.
O alvo make all continua produzindo apenas o diagnóstico de componentes.
Não distribua build/typed-input, que contém programas extraídos da ISO.

Upstream: 3ca419c7966b11438e2dc3c9a7e1914f102ed545.
SDK: 57b02f1e60667e37d22c3aa613d23d2486514ff8.

Antes de permitir Battle_Main, ainda é preciso fechar as faixas do manifesto,
revisar os trechos suprimidos, terminar as conversões dos recursos, instalar
os bindings corretos e executar a inicialização nativa do combate. Esta
versão avança a carga e sua validação, sem declarar essa preparação concluída.
