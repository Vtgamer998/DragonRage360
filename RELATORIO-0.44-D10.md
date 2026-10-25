# 0.44-D10 — novo XEX, boot original de objetos e gerenciadores

**Esta revisão contém um XEX novo. O pedido completo ainda não terminou:
continua sendo um diagnóstico, sem partida jogável e sem apresentação GPU
do combate.** Não é o binário D8 renomeado.

O novo `default.xex` mede 13.705.216 bytes e tem SHA-256
`8f6fb694564c001729c90b5045b4a69114dab080c1544463f1a603b8d5a5a6cb`.
O D8 recebido está preservado em `diagnostico-anterior-D8/default.xex`.

## O que passou a funcionar nesta etapa

### BtlObj_Init/Term originais

A sessão nova chama `BtlObj_Init(1)` original: heap do motor, buffers fixos,
listas, pools, sombras, iluminação e alocações dos módulos de desenho. A
textura de iluminação do comum 2 ganha uma representação TexFile nativa,
com ponteiros reconstruídos e payloads canônicos preservados.

Os modelos são copiados para os buffers originais, com limites de capacidade.
A sessão preserva os descritores anteriores e os restaura ao destruir cada
modelo. Assim `BtlObj_Term` original libera os buffers que ele próprio criou,
sem liberar um empréstimo do preparador de modelos. A sessão D7 continua
apenas como API de regressão; o launcher D10 usa o boot original de objetos.

### BtlChar e BtlAi originais, com reset real

`XbBattleRuntime_Open` instala uma visão persistente, própria, dos recursos
comuns usados pelo boot CPU. As seis tabelas e os dados de IA têm campos
convertidos, offsets nativos e tempo de vida controlado. Não publica os
arquivos restantes como se estivessem convertidos.

Depois de preparar os modelos 0/1, o caminho verifica suas posições iniciais
com chão. Só então executa `Progress_Init`, `Save_Init`, `Pad_Init`, a
configuração original da partida, `BtlChar_AllocAll(2)` — incluindo seu
`ResetAll` imediato — e `BtlAiMgr_Init` originais.

A configuração é versus local: lado 0 humano, lado 1 CPU, personagens 0/1,
um membro por lado, nível de CPU 13 e saúde inicial de 100%. O save é uma
instância temporária na RAM; nenhum save existente é importado ou escrito.
Os gerenciadores são encerrados antes dos modelos/objetos e dos recursos.

Foi adaptada a instalação das tabelas dos arquivos do modelo no binder de
BtlChar: o caminho nativo fornece as representações com posse própria; não
interpreta cabeçalhos little-endian como ponteiros PowerPC.

### Correções encontradas no teste integrado

- Resultado da busca de chão explicitamente alinhado a 16 bytes, conforme o
  contrato do leitor original `StgGround_TestTri`. O alinhamento casual de
  uma execução anterior não era suficiente.
- Máscara de personagens do save usa `1ULL` para o bit 63.
- Contadores de entrada usam `1u` ao percorrer os 32 bits, incluindo o bit 31.
- Uma segunda execução do teste de dados valida os programas sem reaplicar
  DATA sobre o heap vivo. Isso evita apagar os ponteiros do heap enquanto o
  inicializador privado ainda o considera pronto.

## Verificação real desta revisão

Os programas USA foram conferidos com os hashes fixados pelo projeto. Foram
usados recursos reais correspondentes: comum 2/4, cenário 0, dois modelos e
suas animações. Esses recursos não acompanham a entrega.

**Linux i386 sob QEMU-i386:** catálogo nativo com seus dados e callbacks
reconstruídos; dois ciclos completos de boot de objetos, criação dos dois
modelos, save/configuração, `BtlChar_AllocAll`, `BtlAiMgr_Init`, reset e
encerramento. Nos dois lados: vida 40.000, ki 40.000, perfil de IA 0, nível
13. Em ambos os ciclos o heap voltou exatamente ao tamanho livre anterior.
Isso verifica o heap do motor nesses ciclos, não ausência geral de leaks.

O mesmo teste executou `BtlInput_Sample` e `BtlInput_Update` originais com
um estado de controle de teste: remapeamento, pressionamento e soltura de
Square/Rush passaram. Não comprova leitura de um controle físico no Xbox.

O teste host mantém os callbacks originais. As cinco importações de áudio
do SDK que ele não executa são armadilhas que abortam se chamadas; nenhuma
foi chamada. Isso não implementa nem valida áudio ou GPU. Essas armadilhas
existem só no teste, não entram no XEX.

UBSan/bounds habilitados no teste de 32 bits. Apenas na biblioteca matemática
newlib de terceiros, verificações de shift/overflow com sinal são excluídas
por seus algoritmos de manipulação de bits; nos módulos do jogo/port elas
permanecem ativas. Não houve execução de código PowerPC nesse teste.

Verificações adicionais:

- Regressão dos modelos reais: animações, matrizes, volumes, face CPU,
  texturas e criação/liberação dos pools: PASS.
- Oráculo endian do comum 2/IA e equivalência do binder original em i386:
  PASS; 11 corrupções de IA recusadas.
- Posições sintéticas: seis consultas, ausência de chão e três corrupções
  MAPD recusadas, ASan/UBSan: PASS; LeakSanitizer desativado.
- Inventário de 33 módulos da plataforma e recusas de fontes inesperados:
  PASS. Interface existente: PASS.
- Toolchain xsdk GCC 15.3.0, ABI própria: verificações do toolchain PASS.
  Motor: 405/405 unidades PowerPC no vínculo final (incluindo catálogo gerado), zero símbolos pendentes, ABI fechada.
- Catálogo final: 33 regiões, 2.738 operações, 1.779 bindings nativos
  conferidos no ELF final. `complete=true` é do catálogo selecionado.
- Seções PS2 antigas continuam isoladas; enraizá-las ainda expõe seus 103
  símbolos não implementados. O probe de Game_Main ainda encontra 69 nomes
  ausentes. Nenhuma função vazia foi acrescentada ao título para esconder isso.
- XEX2/PE, imagem, símbolos, hashes e chamadas diretas do boot CPU no ELF:
  PASS. Evidências em `checkpoint-D10-final/`.

**Não houve execução em Xbox ou Xenia.** Compilar um XEX e testar o boot em
uma CPU i386 não comprova que ele executa corretamente no console.

## Situação dos pedidos

| Parte | Resultado D10 |
|---|---|
| BtlChar / BtlAi | Funções originais de alocação, reset e inicialização executadas em CPU com dados reais e integradas ao caminho do novo XEX. Sem execução PowerPC/console. |
| Tabelas comuns | Instalação persistente das seis tabelas e IA para esse boot; comum 4 nativo; textura de iluminação nativa. Comum 3 e demais recursos de combate não concluídos. |
| Cenário | Colisão e posições com chão; ainda NOT_READY. MAPD completo, modelos, ambiente, caminhos/nav e demais recursos internos não integrados. |
| Configuração / save | Configuração versus e save temporário originais inicializados em RAM. Não há persistência em disco nesta sessão. |
| Boot | BtlObj_Init(1)/Term originais integrados. O ciclo Battle_Load/Battle_Init completo continua pendente. |
| Simulação / controle | Fase isolada de entrada original testada. Battle_Update e Battle_Main NÃO executados. Marcador permanece 2. |
| Facial / GPU / vozes | Sem nova prova de apresentação, mandíbula animada, skinning na GPU, vozes, auras ou efeitos. O visualizador antigo não prova renderização da partida. |

Não libere o marcador nem retire NOT_READY para chamar Battle_Main: a cena,
o HUD, os recursos restantes, o cenário completo e o fluxo de apresentação
continuam faltando. O diagnóstico não os substitui por sucesso vazio.

## Reproduzir

Use o upstream `3ca419c7966b11438e2dc3c9a7e1914f102ed545`, xsdk
`57b02f1e60667e37d22c3aa613d23d2486514ff8` e o toolchain descrito em BUILD.md.
Com a ISO USA fornecida localmente:

```sh
make typed-data-diagnostic UPSTREAM=/caminho/upstream \
  XSDK=/caminho/xsdk TOOLCHAIN=/caminho/toolchain/bin/powerpc-xsdk-elf- \
  ISO=/caminho/jogo-USA.iso
```

O XEX resultante do alvo está em `build/typed-diagnostic/default.xex`.
O `make all` comum produz apenas o diagnóstico de componentes.

O teste `engine/tools/test_native_managers.py` recebe, nessa ordem: upstream,
xsdk, sysroot i386, qemu-i386, diretório do vínculo nativo, C do catálogo,
recursos selecionados, programas originais e diretório de saída. Requer os
arquivos `common-2.bin`, `common-4.bin`, `stage.bin`, `left-model.bin`,
`left-animation.bin`, `right-model.bin`, `right-animation.bin`; não os gera
nem os acompanha. Veja os nomes e contagens em `checkpoint-D10-final/`.
Os caches do teste são invalidados quando fontes, headers ou flags mudam.
