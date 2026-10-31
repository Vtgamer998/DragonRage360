0.47-D13: novo XEX de diagnóstico. 39 cenários exercitados em CPU, incluindo seis animados; comum 3 com 635/637 recursos preparados (restam dois). Xenia 291f0c9 executou o XEX e falhou em present_open/LUT 0xC00000B5. O ciclo Battle_Load/Battle_Init, Battle_Update/Battle_Main e o desenho GPU continuam pendentes. Leia RELATORIO-0.47-D13.md.

--- Histórico anterior ---

0.46-D12: geometria nativa do cenário 0 e ciclo original de inicialização, física, navegação, culling e ambiente sonoro testados em CPU. Novo XEX de diagnóstico; não desenha o cenário nem executa Battle_Update/Battle_Main. Comum 3 ainda tem 526 recursos indisponíveis. Veja RELATORIO-0.46-D12.md.

--- Histórico anterior ---

0.45-D11: MAPD, caminhos e navegação nativos; 111 recursos de textura do comum 3 (154 imagens), com 526 recursos ainda indisponíveis; ciclo original de áudio/mixer testado em CPU. A batalha completa, HUD/cena e GPU continuam pendentes. Marcador 2/NOT_READY mantidos. Veja RELATORIO-0.45-D11.md.

# 0.44-D10 — novo XEX: objetos, BtlChar e BtlAi

Novo default.xex compilado. O boot original de objetos e o reset/inicialização
de BtlChar/BtlAi estão integrados ao diagnóstico e passaram em CPU i386 com
recursos reais. Ainda não existe luta jogável: Battle_Update/Battle_Main e
apresentação GPU do combate continuam pendentes; sem teste em Xbox/Xenia.

Leia TESTAR-PRIMEIRO.md e RELATORIO-0.44-D10.md. O XEX anterior D8 foi preservado.

---
Documentação histórica da versão anterior:

# Revisão de fontes 0.43-D9 — entrega parcial

**Não há XEX novo nem luta jogável nesta revisão.** O default.xex é o diagnóstico
0.42-D8 original. As alterações em fontes tratam comum 4, leitores temporários
e posicionamento inicial com chão. Leia RELATORIO-0.43-D9-FONTES.md para testes
e pendências. Não execute BtlChar_AllocAll/Battle_Main como se o boot estivesse pronto.

---
Documentação histórica da versão recebida:

# DragonRage360 0.42-D8 — tabelas e facial CPU

**Ainda não há luta jogável nem inicialização de BtlChar/BtlAi.**

O diagnóstico prepara seis janelas comuns, a seção de IA e oito janelas por modelo; executa blink/boca/trilha labial na CPU. Não comprova apresentação facial, áudio ou GPU. Vínculo: 403/403 unidades PowerPC, zero indefinidos; sem execução desta versão no Xbox.

Leia [RELATORIO-0.42-D8.md](RELATORIO-0.42-D8.md). Evidências: checkpoint-D8/. XEX anterior: diagnostico-anterior-D7/. O teste typed-data continua disponível; gPortDataStripped=2 e cenário NOT_READY. Os fontes já incluem o patch.

---
Histórico anterior:

# DragonRage360 0.41-D7 — ciclo real do objeto de modelo

**O registro completo em BtlChar/BtlAi ainda não foi concluído. A luta não inicia.**

O diagnóstico agora chama BtlObj_CreateChara/Setup originais com recursos CPU próprios, materiais e pools de partes/face/sombra. Ambos os modelos passaram no teste Linux/i386 com UBSan/bounds, incluindo animação e liberação dos pools. Isso não é teste do XEX no Xbox ou validação da GPU.

Leia [RELATORIO-0.41-D7.md](RELATORIO-0.41-D7.md). Evidências em checkpoint-modelo-D7/. startup_test.txt=typed-data continua disponível. O marcador de combate 2 e NOT_READY foram preservados. Binário anterior em diagnostico-anterior-D6-C1/; fontes já atualizados.

---
Histórico anterior:

# DragonRage360 — revisão D6-C1

**Pacote corrigido e verificado. A luta ainda não inicia.**

A D6 enviada acrescentava chamadas incompletas e mantinha o XEX da D5. Esta revisão incorpora o main.c correto, deixa essas adições fora do port e usa um inventário explícito de fontes com testes contra sua reintrodução. O executável de diagnóstico foi recompilado sobre a base D5; não há funcionalidade de combate nova.

Leia [REVISAO-D6-C1.md](REVISAO-D6-C1.md). Evidências em checkpoint-D6-C1/. A identidade de execução continua 0.39-D5; D6-C1 identifica a revisão dos fontes/build. startup_test.txt=typed-data permanece disponível. Não reaplique os patches históricos sobre estes fontes.

---
Histórico anterior:

# DragonRage360 0.39-D5 — tabelas e registro original de objetos

**Diagnóstico: a luta ainda não roda. Esta versão não foi executada no Xbox/Xenia.**

- Entrada de parâmetros de 256 bytes convertida por campos; janelas nativas dos ataques, ki blast e movimento ligadas ao objeto.
- Registro inativo em gBtlObjTbl e nas listas originais, com remoção/limpeza e reprodução testadas em i386.
- Retarget com tabela comum real integrado ao diagnóstico.
- Faltam gerenciadores de personagem/IA, tabelas e binders restantes, facial, apresentação GPU dos lutadores e boot do combate. NOT_READY e marcador 2 continuam ativos.

Leia [RELATORIO-0.39-D5.md](RELATORIO-0.39-D5.md). Evidências em checkpoint-dados-D5/; default.xex atualizado, ainda diagnóstico. startup_test.txt=typed-data registra os testes em default.log. Fontes já atualizados; não reaplique o patch. XEX anterior em diagnostico-anterior-D4/.

---
Documentação histórica (escopo e binário da raiz substituídos acima):

# DragonRage360 0.38-D4 — poses e volumes nativos

**Ainda é diagnóstico, sem luta jogável e sem execução desta versão no Xbox/Xenia. A lista pedida não está completa.**

- Player original ligado a BobjWork, nós próprios e motions; retarget entre os dois modelos testado.
- Poses e matrizes animadas pelas rotinas originais; 22 partes de corpo por modelo e volumes de ataque ativos.
- 24 texturas RGBA por modelo e referências de malha/geometria facial preparadas. Não há submissão GPU ou animação facial completa.
- Faltam tabelas/binders completos, renderer/facial e estado/registro nos gerenciadores de personagens. O registro de proprietários dos nós não é o gerenciador do combate.

Leia [RELATORIO-0.38-D4.md](RELATORIO-0.38-D4.md) e os testes em `checkpoint-dados-D4/`. O `default.xex` novo mantém a guarda da luta. `startup_test.txt=typed-data` executa o warm-up e registra resultados em `default.log`. Fontes atuais na raiz; não reaplique o patch incremental. Binário anterior em `diagnostico-anterior-D3/`.

---
Documentação histórica (escopo e binário da raiz substituídos acima):

# DragonRage360 0.37-D3 — catálogo nativo e estados parciais

**Novo XEX de diagnóstico; ainda não inicia uma luta e não foi executado no Xbox/Xenia.**

- Catálogo nativo `complete=true`: campo inteiro da água comprovado; 46 bytes de três pools sem consumidor nativo excluídos com auditoria e testes. Isso não significa converter todos os dados da ISO.
- Colisão preparada em arena própria e instalada em `gBtlStage`/`gColMeshBase`, mantendo NOT_READY. Leitores originais conferidos com objetos inteiros e quebrados.
- Dois esqueletos nativos e buffers de animação preparados; `PlayModel/Step` original testado por 200 passos em cada modelo.
- **Faltam os lutadores completos**, seus binders/tabelas/corpos, cenário completo, gerenciadores, entrada e apresentação da luta. A guarda permanece ativa mesmo com catálogo completo.

Leia [RELATORIO-0.37-D3.md](RELATORIO-0.37-D3.md). `startup_test.txt=typed-data` exercita os caminhos novos e escreve no `default.log`; execução no console ainda pendente. Os fontes da raiz já estão atualizados. O patch incremental em `extras/` parte da 0.36-D2; o binário anterior fica em `diagnostico-anterior-D2/`.

---
Documentação histórica (contagens e binário da raiz substituídos acima):

# DragonRage360 0.36-D2 — revisão dos dados e colisão do cenário

Novo `default.xex` recompilado e verificado estaticamente. **Ainda é diagnóstico; não inicia uma luta e não foi executado no Xbox/Xenia.**

- Faixas sem tipo: 168 → 4, com 54 bytes restantes.
- Os 23 trechos suprimidos foram revisados como preenchimento de alinhamento antes dos símbolos.
- Leitor nativo de colisão do cenário 0: 100 zonas e 13.583 ocorrências de triângulos conferidas com dados reais e leitura independente.
- Catálogo: 2.736 operações; 1.779 slots de ponteiro conferidos no vínculo final. `Battle_Main` continua protegido antes de acessar o estado.

O startup `typed-data` agora também registra a colisão no `default.log`. Faltam os recursos vivos e o boot da luta; o leitor não instala o cenário no motor. Leia [RELATORIO-0.36-D2.md](RELATORIO-0.36-D2.md). Fontes atuais na raiz; patch incremental em `extras/`, para quem parte exatamente da 0.35-D1. Binário anterior em `diagnostico-anterior-D1/`.

---
Documentação histórica (contagens e binário da raiz substituídos acima):

# DragonRage360 0.35-D1 — manifesto tipado PARCIAL

O novo default.xex testa a carga parcial das tabelas pela ISO do usuário.
startup_test.txt=typed-data. O manifesto ainda não é completo e a luta não inicia.
O próprio Battle_Main recusa a entrada antes de acessar o work, com marcador 2.
Leia RELATORIO-0.35-D1.md para resultados e limites. Os fontes estão atualizados
na raiz; não aplique o patch novamente. O XEX 0.34-L1 anterior está em
 diagnostico-anterior-L1, com sua configuração de teste gráfico.

O catálogo aplica 2.157 operações em 33 regiões; 1.694 slots de ponteiro foram
conferidos no link final. Faltam 168 faixas e a revisão de 23 trechos suprimidos,
além de conversões e inicialização dos recursos para combate. Não é jogo pronto.

Para repetir o gráfico: copie startup_test-graphics.txt sobre startup_test.txt.
Para regenerar tudo: make typed-data-diagnostic ISO=... XSDK=... TOOLCHAIN=... UPSTREAM=...
O alvo comum make all não inclui o catálogo/combate; confira o relatório.

---
Documentação histórica anterior (escopo e contagens substituídos acima):

# DragonRage360 0.34-L1 — diagnóstico com Battle_Main vinculado

Novo default.xex: vínculo do alvo de combate fechado, sem indefinidos.
Não inicia a luta: dados tipados e boot nativo continuam faltando.
Leia RELATORIO-0.34-L1.md para resultados, limites e reprodução.
Os fontes atuais estão na raiz. Não aplique o patch novamente sobre eles.
O diagnóstico HW1 anterior está em diagnostico-anterior-HW1/default.xex.
A interface nova mostra 0.34-L1; startup_test.txt continua graphics.

Para reproduzir o XEX incluído, use make engine-linked-diagnostic,
conforme o relatório. make all gera só o diagnóstico de componentes.

---
Documentação histórica anterior (escopo e contagens substituídos acima):

