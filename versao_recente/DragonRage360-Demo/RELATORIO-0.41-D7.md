# 0.41-D7 — ciclo real do objeto de modelo

**O pedido não está todo concluído.** Esta revisão executa `BtlObj_CreateChara` e `BtlObj_Setup` originais com recursos próprios convertidos, pools e materiais CPU. **Não inicializa nem registra os lutadores em BtlChar/BtlAi e não inicia uma luta.** O XEX continua sendo diagnóstico e não foi executado no Xbox/Xenia.

## O que mudou

Antes, o diagnóstico copiava um objeto parcial para a lista e mantinha active=0. Agora o startup typed-data prepara os dois modelos e chama o caminho real:

```
BtlObj_CreateChara -> BtlObj_Create -> BtlObj_Setup
 -> BtlObjMdl_Create -> BtlObjMdl_Init
 -> partes, flags/preset, transform, corpo, face, sombra, animação
 -> charaWork do id original
```

Os objetos recebem active=1 e o ready próprio do **modelo**, definidos pelas rotinas originais. Isso não equivale à prontidão dos personagens ou da partida. Os primeiros ids 0 e 1 e os dois slots fixos são usados numa sessão nova. O adaptador recusa tentar criar um terceiro objeto de lutador, evitando usar charaWork fora dos dois buffers existentes.

Também existe a API de Setup sobre um objeto já registrado, utilizada no teste. Destruição/desregistro chamam o destrutor real do modelo, devolvem partes/face/sombra aos pools e liberam os recursos próprios na ordem correta. O estado privado anterior é restaurado para que o proprietário possa ser reutilizado/liberado.

## Recursos e materiais CPU

O módulo xb_model_native.c prepara uma representação de CPU para os leitores usados pelo ciclo do modelo:

- Header pmdl e prefixos dos registros do esqueleto/malha: campos numéricos convertidos separadamente, mantendo os payloads VIF canônicos little-endian.
- TexFile/TexEntry dos grupos necessários: metadados numéricos nativos, TEX0 de 64 bits e ponteiros de pixels/CLUT com limites validados pelo decoder. Pacotes de upload e pixels não são trocados como palavras genéricas.
- Referências de textura dos streams: rebasing limitado aos UNPACKs de materiais reconhecidos pelo parser VIF, com validação de capacidade dos endereços GS. As referências entregues pelo GetMesh do modelo ativo usam os streams rebased.
- Materiais: o original BtlObjMdl_InitAlphaSlots atribui os slots/linha de alpha. A inicialização facial original copia o alpha das paletas do material para os conjuntos de olhos/boca. Isso prepara dados de CPU, sem provar apresentação na GPU.
- Tabela de secondary motion nos campos lidos pelo inicializador de chains e tracks LIPS do idioma padrão: contagens, limites e frames validados, com conversão dos escalares lidos pelo motor.

Duas pontes explícitas reconhecem os proprietários nativos em BtlObj_BindTables e BtlObjMdl_BindFiles. Elas ligam as representações preparadas e inicializam os materiais; não retornam sucesso sem construir os dados. Objetos fora desse adaptador conservam o caminho original. Os anchors dos patches exigem o upstream fixado.

O leitor do byte superior de flags do modelo foi substituído pela extração numérica equivalente. O alias do byte físico +0xF só representa esse byte em little-endian; não pode ser preservado literalmente no PowerPC big-endian.

**Continuam ausentes as tabelas comuns/roster e de super/skill necessárias ao personagem completo.** As quatro janelas de parâmetros/ataque/ki/movimento da D5 permanecem. As pontes não classificam as tabelas restantes como convertidas nem fornecem dados fictícios em seu lugar.

## Pools e propriedade

A sessão inicializa os pools originais de 512 partes de 0xE0 e quatro faces de 0x70. As funções originais alocam/devolvem os nós usados pelo modelo. O pool de cinco sombras usa o layout original, com work e dois buffers por sombra, alocação nativa verificada e alinhamento dos buffers a 64 bytes. Falhas de preparação são recusadas antes de publicar o modelo.

O BtlObjWork desta sessão tem alocação nativa alinhada e vida útil própria. **Não é o boot completo BtlObj_Init/Term da partida.** Seu encerramento usa o adaptador, depois de conferir que todos os objetos e pools foram devolvidos. Essa distinção precisa ser preservada na integração posterior do boot.

## Evidência executada nesta revisão

Foram recuperados os recursos do perfil fixo e conferidos tamanho e FNV64 contra o manifesto anteriormente verificado. Nenhum desses bytes é distribuído no pacote.

Em Linux/i386 via QEMU-i386, com UBSan/bounds, os dois modelos reais passaram por:

- Setup original e CreateChara original, sem substituir essas funções por stubs;
- dois modelos simultâneos nos pools: 138 partes, duas faces e duas sombras ocupadas;
- reprodução, poses e matrizes pelas rotinas originais após Setup;
- retarget de um modelo enquanto o outro é liberado, sem perder a cópia da animação;
- destruição, desregistro e encerramento com contagens de pools restauradas;
- 564 blocos de materiais VIF por modelo conferidos, incluindo os campos de endereço GS rebased;
- TexFile inválido recusado antes de consumir pools ou ativar o modelo.

As texturas/geometrias do cache anterior também passaram: 24 imagens RGBA por modelo e cinco/seis conjuntos faciais. O teste x86-64 com ASan/UBSan cobre construção, caches e vida útil; os ciclos que exigem layouts de 32 bits são recusados nessa arquitetura, não apresentados como testados por ela.

QEMU-i386 executou um teste Linux de 32 bits. **Não executou PowerPC, XEX, Xbox ou GPU.** A inicialização facial não comprova blink/lip-sync apresentados, auras, efeitos ou skinning na GPU.

## O que ainda impede o pedido completo

BtlChar_AllocAll não é apenas uma alocação: lê seis tabelas comuns, chama ResetAll e inicializa cada personagem a partir da configuração da partida, membros, saves, movimento e ações. BtlAiMgr_Init faz bind da seção de IA e lê os parâmetros/kit de cada lutador. Chamar esses caminhos agora consumiria tabelas e estado ainda não preparados.

Portanto, gBtlChars/gBtlAi não são publicados como inicializados nesta revisão. Falta converter e instalar essas tabelas/estado, executar os resets reais e integrar os gerenciadores. Depois ainda será necessário completar o cenário e conectar simulação, controle e apresentação.

Battle_Main permanece vinculado, mas bloqueado com gPortDataStripped=2; o cenário conserva NOT_READY. Nenhum desses marcadores foi alterado pela ativação do **modelo**.

## Uso

O novo default.xex se identifica como 0.41-D7. startup_test.txt continua typed-data. O log acrescenta:

```
fighter real BtlObj_CreateChara/Setup: side=... id=... error=...; native CPU resources/pools/materials; BtlChar/BtlAi NOT initialized
```

error=0 nesse ponto valida somente esse passo quando observado no console. O diagnóstico não oferece uma luta para iniciar. Os testes host, vínculo e hashes estão em checkpoint-modelo-D7/. O binário anterior fica em diagnostico-anterior-D6-C1/.

Fontes atuais já incluem as alterações. O patch incremental em extras parte exclusivamente da revisão D6-C1. Use uma pasta nova; não reaplique patches sobre os fontes atualizados. ISO, programas MIPS e recursos extraídos não acompanham a entrega.

## Vínculo e integridade

402/402 unidades PowerPC com catálogo; zero indefinidos e zero conflitos de ABI. Catálogo regenerado: C idêntico, 33 regiões, 2.738 operações e 1.779 bindings conferidos no ELF final. Checks de isolamento das bibliotecas originais, exclusões auditadas, guarda de prontidão, XEX2/PE e hashes da imagem: PASS. O Game_Main original ainda tem 69 dependências no probe. Nenhuma execução do XEX nesta revisão.

SHA-256 do novo default.xex: `84615c3a972b6e16009ecdf07022285c8717a52fb15b7e2a88108f467bb3c54d`.

As verificações de inventário continuam ativas, agora com 30 módulos explicitamente selecionados. Os fontes dos três adaptadores novos/modificados foram comparados com os hashes registrados na compilação final.
