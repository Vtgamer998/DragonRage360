# Revisão D6-C1 dos dois pacotes enviados

**A D6 enviada não é uma integração funcional de combate. A correção do main.c é válida e foi incorporada. Este pacote recompila o diagnóstico da D5 e acrescenta verificações de build; a luta continua sem iniciar. Nenhuma execução nova em Xbox/Xenia foi realizada.**

## Comparação comprovada

A comparação dos ZIPs com a 0.39-D5 verificada encontrou:

- Um único arquivo existente modificado na D6: src/main.c.
- 136 arquivos acrescentados e nenhum removido.
- default.xex da D6 idêntico ao da D5: SHA-256 ff48957be7fd58bd482addee8ea9106cae7dbfa765de4339dd103ef5c2c9b81a. Ele não contém as integrações anunciadas pelos fontes novos.
- main.c do correcao-D6.zip idêntico, byte a byte, ao main.c da D5.
- 102 headers, btl_obj_decomp.c, btl_char_mgr_decomp.c e port_compat.h idênticos às cópias já presentes no upstream fixado. Copiá-los para a plataforma não implementa novas funcionalidades e pode duplicar definições/shadowing.
- 25 blocos binários, somando 86.048 bytes, sem esquema tipado ou ligação válida comprovada. Não foram incluídos nesta entrega. Seus nomes, tamanhos e hashes estão no relatório JSON; nenhum byte desses recursos é necessário para reproduzir os fontes corrigidos.

O script engine/tools/audit_d6_bundle.py reproduz a comparação a partir dos três ZIPs e do upstream. Ele não executa o código ou os binários enviados.

## Problemas reais nos fontes da D6

| Arquivo | Constatação |
|---|---|
| xb_battle_activate.c | A função CompleteTables **existe**, mas retorna sucesso com TODO e sem concluir tabelas. O texto da correção é impreciso ao tratá-la como ausente. A ativação não prepara os recursos exigidos pelo ciclo original de BtlChar_AllocAll/Setup. |
| xb_tables_complete.c | Publica ponteiros derivados de common2/common4 e libera os buffers ao final, deixando referências sem memória válida. Usa campos inexistentes no upstream fixado: BtlResWork.unk0 e BtlObjState.unk. |
| xb_boot_combat.c | Declara gScenarioReady sem definição no port. Escrever nele não modifica a guarda real gPortDataStripped nem o NOT_READY do cenário. A chamada Battle_Main permanece comentada; o log de desbloqueio não comprova inicialização. |
| xb_facial_hook.c | Chama dr_face_vif_init/update, ausentes nas implementações do port. Encaminhar essas chamadas não implementa blink, expressão, mandíbula ou lip-sync. |
| xb_render_hook.c | Chama dr_gs_backend_submit, ausente no port. Não prepara materiais, mipmaps, skinning ou os consumidores de comandos do combate. |
| src/main.c | Acrescenta chamadas de ativação antes de o diagnóstico carregar/preparar seus recursos. Não condiciona as etapas seguintes ao sucesso das anteriores. |

Os dois primeiros módulos falharam também na compilação isolada PowerPC. Os outros três compilaram isoladamente, mas seus objetos conservaram as referências ausentes mencionadas; isso não constitui vínculo ou execução. Os logs estão em checkpoint-D6-C1/D6-PPC/.

Não foram criadas definições vazias para fazer essas chamadas passarem. Não foi simplesmente acrescentado stdio.h ou declarado um estado fictício: isso não corrigiria a vida útil, os layouts, os dados e a inicialização.

## O que entrou nesta revisão

A árvore de fontes foi reconstruída sobre a D5 verificada, incorporando o main.c restaurado pela correção. As adições defeituosas da D6 ficaram fora dela. Os anexos originais não foram alterados.

O build deixava entrar automaticamente qualquer .c em engine/platform/platform. Agora Makefile e link_battle.py usam a mesma lista explícita de 29 módulos, engine/platform/platform-sources.txt. Antes de invocar o compilador/plugin, a auditoria recusa:

- módulos extras ou ausentes;
- nomes duplicados ou caminhos inválidos no inventário;
- port_compat.h local, que resolveria antes do header do upstream;
- tabelas .bin sem esquema dentro da árvore de fontes da plataforma.

Os testes injetam essas situações e conferem que Make recusa antes de tentar executar o compilador. Também conferem as adições reais da D6 com o novo inventário aplicado numa cópia temporária. A lista exige revisão explícita para novos módulos; **não é um analisador capaz de provar que uma implementação é correta**. Compilação, vínculo, dados e testes de execução continuam necessários.

## Estado do jogo

Não há novo gerenciador de personagens/IA, fluxo facial, renderer de luta ou boot nativo completo nesta revisão. Permanecem os avanços reais da D5: quatro janelas tipadas de tabelas por lutador, poses/volumes, cache de texturas, retarget comum e registro inativo nas listas originais. Battle_Main entra no executável, mas não é chamado pelo diagnóstico. O marcador de prontidão continua 2 e o cenário conserva NOT_READY.

O diagnóstico recompilado se identifica como 0.39-D5 porque seu código de execução foi restaurado para essa base. D6-C1 identifica esta revisão de fontes/verificações, sem anunciar uma fase de combate inexistente.

## Reprodução

```
python3 engine/tools/audit_platform_sources.py .
python3 engine/tools/test_platform_sources.py OUTPUT --d6 CAMINHO_D6_EXTRAIDA
python3 engine/tools/audit_d6_bundle.py ZIP_D5 ZIP_D6 ZIP_CORRECAO UPSTREAM OUTPUT
```

Os comandos de build existentes continuam aplicáveis, usando o upstream e SDK fixados em BUILD.md. A auditoria entra automaticamente no build Make e no vínculo estrito. Para testar no console, startup_test.txt continua typed-data e escreve default.log; ainda não há uma luta para selecionar/iniciar.

Esta revisão não incorpora ISO, programas MIPS, extrações do jogo ou os dumps sem tipo da D6. Consulte o resumo dos testes e o hash do XEX ao final deste relatório.

## Validação concluída

- SDK e toolchain restaurados; testes de recusa de ABI misturada e alinhamento aprovados.
- Compilação final: 401/401 unidades PowerPC, zero indefinidos e zero conflitos de ABI.
- Catálogo regenerado com os programas reais verificados: C idêntico ao anterior. No ELF final, 33 regiões, 2.738 operações e 1.779 bindings de ponteiro conferidos.
- Interface, disco e saves no host: PASS. Carregador tipado com ASan/UBSan, carga transacional e recusa de boot incompleto: PASS.
- Auditoria do inventário e casos negativos de Make paralelo e entrada do vínculo antes do plugin: PASS.
- Exclusões auditadas e isolamento dos 103 símbolos de bibliotecas originais: PASS. O probe de Game_Main original continua com 69 dependências; não foi integrado.
- XEX2/PE, hashes da imagem, ABI e presença da guarda de prontidão: PASS.
- Modelos, poses, volumes, GPU, entrada de luta e partidas não foram exercitados nesta revisão. Os resultados anteriores da D5 são históricos, não novos testes em hardware.

SHA-256 do default.xex recompilado: `38c3301f6b8268c8d826fc78f0f70bf63a518abb899ac747cb57853176f2c3f3`. Tamanho: 13.705.216 bytes.

O XEX anterior da D5 foi preservado em diagnostico-anterior-D5/. O novo binário continua sendo diagnóstico; um hash diferente após a recompilação não implica nova funcionalidade de jogo.
