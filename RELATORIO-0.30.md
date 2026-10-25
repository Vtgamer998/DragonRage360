# 0.30 — chamadas gráficas do motor e auditoria da inicialização

O trabalho desta etapa adapta quatro chamadas reais usadas pelo motor:
Port_GsMarker, sceGsSyncPath, sceGsSetDefStoreImage e sceGsExecStoreImage.
Não há luta executável nem validação em Xbox ou emulador.

## Resultado do vínculo PowerPC

| Raízes do diagnóstico | Unidades compiladas | ABI float | Nomes pendentes |
| --- | ---: | --- | ---: |
| Battle_Main | 383/383 | Sem conflitos | 133 (antes 137) |
| Battle_Main + Game_Main | Mesmos objetos reutilizados | Sem conflitos | 228 |

As quatro chamadas acima deixaram a lista de símbolos indefinidos. A auditoria
achou 30 nomes com referência em código e 103 somente em dados no primeiro
vínculo. Ao incluir Game_Main, são 125 com referência em código e os mesmos
103 somente em dados. Isso inclui seções posteriormente descartadas pelo GC;
não autoriza remover nenhuma tabela. A inicialização acrescenta 95 dependências,
inclusive criação de áudio ADXT, DMA, leitura de arquivos, MPEG e frontend.
A contagem de 137 descrevia apenas o primeiro recorte, não o jogo inteiro.

A aritmética do motor foi rebaixada em 294 objetos; a inspeção de instruções
não encontrou aritmética float nativa nesses objetos. Isso não prova
rollback determinístico nem comportamento correto do combate em execução.
O vínculo do combate continua falhando e não produz XEX. A repetição com
Game_Main reutilizou os mesmos objetos compilados; não houve segunda compilação.

Foi gerado um novo default.xex **de diagnóstico 0.30**. A validação estática
confirmou as quatro entradas no executável, formato/ABI/hashes e ausência de
símbolos fortes indefinidos no diagnóstico. Battle_Main e Game_Main não estão
nele. Isso não significa que os símbolos pendentes do motor foram ignorados:
o launcher não os chama. Evidências completas em engine/reports/0.30.

## Implementação

Port_GsMarker enfileira o ID do efeito no registro virtual 0x7F pelo produtor
de pacotes canônicos. Isso preserva a posição do efeito na lista de desenho.
O teste compara os bytes com o gs_marker.c original compilado separadamente,
além das comparações dos produtores DMA que já existiam: 1.841 casos.
O diagnóstico usa a entrada real em vez de escrever esse registro diretamente.

sceGsSyncPath(0,0) submete a captura e espera o fence real de dr_xenos_sync.
Outros modos/tempos são recusados explicitamente; não há retorno vazio de
sucesso. Erros do backend e de espera são propagados. O limite da espera
interna do backend continua sendo 1.000 ms.

O descritor de StoreImage ocupa os 0x70 bytes usados pelo jogo. Seu estado é
independente por descritor e acessado com memcpy, aceitando arrays desalinhados.
O perfil implementado é CT24, largura de framebuffer 512, altura 448 e SBW=8,
como em ScrXfade_Capture/StoreHalf do jogo USA. O endereço do bloco seleciona
o framebuffer e a linha inicial; x/y/w/h delimitam o recorte.

A execução obtém RGBA do target exato do GS, espera a GPU e copia RGB para
o destino. Não substitui um target ausente pelo frame atual. Validação,
alocação e leitura completa ocorrem antes da escrita no destino: argumentos
inválidos, framebuffer ausente ou timeout deixam seus pixels intactos.
O chamador continua responsável por reservar w*h*3 bytes no destino.
Não existe cache entre as duas metades; desempenho ainda precisa ser medido.

## Verificações nesta etapa

O teste de captura compila gs_core.c, gs_draw.c, gs_sony.c, o produtor DMA,
o backend e o diagnóstico reais. Somente os transportes GPU são simulados.
Foram verificados dois descritores simultâneos desalinhados, as metades
superior/inferior, o segundo framebuffer, recortes RGB com canários de memória,
formato/tamanho inválidos, target ausente e timeout do fence. Também passaram
as regressões existentes de captura, VU, textura, stencil e consumidores.
ASan/UBSan foram usados com detect_leaks=0; não há medição de vazamentos.

O teste separado de ambiente Sony verifica serialização dos DBuff e transporte
GIF. Nesse teste, as rotinas de leitura não chamadas são eliminadas por GC;
sua execução é coberta pelo teste completo de captura, não por funções falsas.

audit_engine_refs.py inspeciona relocações dos objetos ELF32 big-endian usados
no vínculo. Separa referências em código das que aparecem apenas em seções de
dados, omitindo debug. Inclui seções posteriormente descartadas pelo linker;
não demonstra uso ou desuso em execução e não remove callbacks de tabelas.
Foi conferido com um objeto PowerPC contendo referências reais em texto,
dados e debug antes da auditoria do motor.

link_battle.py aceita --root repetível para auditar também Game_Main.
Battle_Main permanece uma raiz obrigatória do diagnóstico de vínculo.
Isso não chama nenhuma dessas funções nem substitui a inicialização PS2.

## Limites que continuam impedindo a luta

As tabelas de dados da compilação continuam sem todos os valores originais.
O carregador tipado ainda precisa do manifesto completo e das relocações.
Game_Main ainda inicializa serviços PS2 e precisa de adaptação nativa.
O transporte dos pacotes produzidos pelo combate, IA, colisões, animações e
apresentação ainda não formam um ciclo executável no Xbox.

Os chamadores originais de StoreImage não verificam todos os retornos;
o tratamento de erro no fluxo do jogo ainda precisa ser integrado. Disponibilizar
as entradas SDK não conclui esse fluxo. O diagnóstico verifica os retornos.

Áudio ADXT, RPC/IOP, outros serviços Port_* e callbacks de tabelas continuam
dependências reais. Não foram criadas funções vazias para satisfazer o linker.
DATE/FBA/stencil, iluminação, sombras e efeitos não receberam validação de
pixels em hardware. Saves, modos e online jogável continuam pendentes.

## Reproduzir

Use o upstream 3ca419c7966b11438e2dc3c9a7e1914f102ed545 e o xsdk
57b02f1e60667e37d22c3aa613d23d2486514ff8, com seu toolchain powerpc-xsdk-elf.

```sh
python3 engine/tools/test_dma_producer.py UPSTREAM OUT --sanitize
python3 engine/tools/test_capture.py XSDK OUT --sanitize
python3 engine/tools/test_gs_sony_native.py OUT --sanitize
python3 engine/tools/link_battle.py UPSTREAM CC XSDK OUT --float-mode native-ps2 --platform
python3 engine/tools/audit_engine_refs.py OUT AUDIT_OUT
python3 engine/tools/link_battle.py UPSTREAM CC XSDK BOOT_OUT --float-mode native-ps2 --platform --root Game_Main
```

O plugin de aritmética exige headers de desenvolvimento GCC e GMP/MPFR/MPC.
Não remova os atributos de ABI nem desative a verificação do linker.
ISO e recursos extraídos continuam externos ao pacote distribuído.
