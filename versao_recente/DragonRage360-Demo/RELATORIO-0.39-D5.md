# DragonRage360 0.39-D5 — tabelas e registro original de objetos

**A lista solicitada não está concluída. O default.xex é diagnóstico, sem luta jogável. Esta versão não foi executada no Xbox 360 ou Xenia.** O trabalho desta revisão fecha conversões verificadas e exercita o registro de objetos do motor; não libera Battle_Main nem marca um cenário incompleto como pronto.

## Resultado por pedido

| Pedido | Resultado e limite |
|---|---|
| gBtlObjTbl e listas originais | Implementados em sessão de diagnóstico isolada, com BtlObj_InitTable, AllocId, RebuildTable e FreeId originais. Dois objetos podem ficar nas listas e no mapa enquanto reproduzem animações. Permanecem inativos. Os gerenciadores BtlChar/BtlAi não foram inicializados. |
| Entrada de parâmetros de 256 bytes | Convertida por campos. Corrigida a flag u16 inicial, preservando os bytes de tipo/aura. O trecho CC..FF só é aceito se comprovadamente zero no recurso recebido; variantes desconhecidas são recusadas. |
| Tabelas | Convertidos/ligados os 163 registros alcançáveis pelos leitores BtlAtk, os 13 registros de ki blast e os parâmetros de movimento. Tabelas de super/skill, ações do roster e demais recursos comuns continuam pendentes. |
| Retarget com tabela comum | Implementada cópia própria da tabela real D_002F2420, validação de quantidade/valores e uso pelo algoritmo original. Ausência da tabela recusa a API; não usa zeros como substituto. |
| Facial | Sem implementação nova de blink, expressões, mandíbula, lip-sync ou apresentação facial. Permanecem as referências de geometria/texturas da D4. |
| Renderização | Sem apresentação nova desses lutadores na GPU. Cache RGBA, matrizes/volumes e referências de malha permanecem disponíveis. Materiais, mipmaps, fluxo de comandos do combate e skinning GPU ainda precisam de integração e validação. |
| Cenário, dano, IA, controle e boot | Ainda incompletos. NOT_READY e marcador 2 preservados. Battle_Main está vinculado, mas não é chamado pelo diagnóstico. |

## Conversão e limites dos recursos reais

| Entrada do modelo (uma baseada) | Janela nativa produzida | Tamanho recebido por modelo | Fora da janela |
|---|---:|---:|---:|
| 0x12 — parâmetros | 256 | 256 | 0 |
| 0x13 — ataques | 7.824 | 8.512 | 688 |
| 0x14 — ki blast | 676 | 704 | 28 |
| 0x15 — movimento | 160 | 192 | 32 |

São 8.916 bytes de janelas nativas por lutador. Os 688 bytes depois dos 163 ataques contêm dados não nulos: não foram tipados por suposição. As caudas de ki/movimento também não são expostas como dados nativos. Essas janelas não constituem o conjunto completo de dados de combate.

Os esquemas resultam dos leitores BtlParam/BtlAtkRecord, BtlTechBParam/BtlKiBlastData/BtlMoveData e BtlCapiParam do upstream fixado. Em vez de trocar todas as palavras, o conversor trata u16, palavras de 32 bits e campos de bytes separadamente. Valida tamanho, capacidade, sobreposição e cauda desconhecida antes de escrever. O fonte está em engine/platform/platform/xb_fighter_tables.c.

## Registro e vida útil

A sessão recusa substituir gBtlObjWork já existente. Aloca trabalho próprio, inicializa somente o registro/listas e publica uma cópia do objeto no slot original; o adaptador muda a resolução do proprietário para esse endereço. Poses, matrizes e reprodução continuam usando a instância publicada. Desregistrar copia o estado de volta, devolve o slot à lista livre e reconstrói o mapa. Liberar o proprietário remove seu slot antes de liberar seus buffers. A sessão só termina quando não há objetos registrados e restaura o mapa anterior.

Isto não chama BtlObj_Setup/CreateChara nem simula suas dependências como inicializadas. active=0 impede apresentar esses registros como personagens prontos. Não há registro em BtlChar/BtlAi. O trabalho de face, pools, recursos/slots, chains, materiais, estado do personagem e boot dos gerenciadores ainda precisa ser integrado antes de ativar esses objetos.

## Testes realizados

- Os dois recursos reais foram conferidos contra leituras escalares independentes: 35 campos dos parâmetros, 1.304 dos ataques, 143 do ki blast e 40 do movimento por modelo. A saída big-endian foi comparada byte a byte. Campos de bytes permanecem inalterados; tamanhos insuficientes, sobreposição e cauda de parâmetros desconhecida são recusados sem escrita parcial. Isso testa bytes BE emitidos no host, não execução BE em hardware.
- Linux/i386 via QEMU-i386 com UBSan/bounds: dois objetos nas listas/mapa originais, remoção de um deles, liberação do proprietário e reutilização da lista livre. A referência nativa de ataques no objeto publicado é conferida no offset 0x920. Reprodução, poses, matrizes, volumes e retarget passam enquanto o objeto está registrado.
- O retarget de tabela comum usou os 213 floats extraídos do ELF USA verificado pelo hash original. As constantes reais ficam apenas nos inputs intermediários de teste e não são distribuídas no pacote. A API recusa tabela ausente ou de tamanho errado.
- Linux/x86-64 com ASan/UBSan: construção, dados próprios, conversões, reprodução, caches e descarte. O registro dependente dos layouts de 32 bits não é executado nesse teste.
- O catálogo nativo continua com 33 regiões, 2.738 operações e 1.779 slots de ponteiro verificados no ELF final. complete=true refere-se às regiões selecionadas, não ao boot ou aos recursos completos da luta.
- Compilação PowerPC: 400/400 unidades na base, 401/401 com catálogo; zero símbolos indefinidos e zero conflitos de ABI. XEX2/PE, hashes da imagem, ABI e presença dos caminhos novos conferidos estaticamente. Guarda de Battle_Main mantida em 2.
- Testes de isolamento dos 103 nomes legados e das exclusões auditadas de pools originais aprovados. Game_Main original ainda tem 69 dependências no probe; o objetivo imediato continua um boot mínimo, não habilitar menus/filmes.

QEMU-i386 executa o teste Linux/i386; não é execução do Xbox, PowerPC ou XEX. Não há evidência nova de pixels, skinning GPU, dano, IA, controles, áudio de combate, estabilidade de partida ou desempenho.

## Uso e reprodução

startup_test.txt=typed-data executa o diagnóstico: catálogo, colisão ainda NOT_READY, modelos, registro inativo nas listas, player/poses/volumes, texturas e retarget normal/comum. default.log identifica side, id, erros e as limitações. Nenhuma dessas etapas chama Battle_Main.

Fontes atuais na raiz; evidências em checkpoint-dados-D5/. O XEX D4 foi preservado em diagnostico-anterior-D4/. Patch incremental em extras/, para a cópia exata da 0.38-D4; já está aplicado aos fontes da raiz. Não reaplique sobre a D5.

Para testar os recursos no host:

```
python3 engine/tools/test_fighter_tables.py LEFT_MODEL RIGHT_MODEL OUTPUT
python3 engine/tools/test_native_fighter.py UPSTREAM MODEL MOTIONS OUTPUT --program SLUS_216.78 --i386-root MULTILIB_ROOT --qemu QEMU_I386 --other-model OTHER_MODEL --other-motions OTHER_MOTIONS
```

A ferramenta exige o ELF USA já identificado; não baixa recursos. ISO, executável MIPS e recursos extraídos continuam fora do pacote. Upstream 3ca419c7966b11438e2dc3c9a7e1914f102ed545, SDK 57b02f1e60667e37d22c3aa613d23d2486514ff8 e GCC 15.3.0/ABI xsdk mantidos. O alvo make typed-data-diagnostic reproduz o diagnóstico usando o SDK/toolchain, upstream e dados próprios indicados.

SHA-256 do default.xex: `ff48957be7fd58bd482addee8ea9106cae7dbfa765de4339dd103ef5c2c9b81a`.
