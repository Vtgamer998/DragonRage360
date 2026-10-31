# Diagnóstico 0.33-HW1 — teste gráfico automático

O log recebido passou na interface/ISO/animação, mas não contém abertura do
modelo ou teste GS. Não há falha gráfica diagnosticada nesse log porque esses
caminhos não chegaram a ser registrados.

Esta revisão altera somente o launcher base 0.33 para iniciar um diagnóstico
explicitamente configurado. O teste da interface continua ocorrendo antes.
Não incorpora o player de vídeo WIP nem o perfil Battle_Main. Não há luta.
A legenda antiga DEMO 0.16 era um texto fixo esquecido na interface; agora
mostra 0.33-HW1. O log anterior já identificava corretamente a versão 0.33.

## Primeiro teste: gráficos

1. Use esta pasta completa em separado no Xbox RGH e execute seu default.xex.
   startup_test.txt já contém graphics. Não precisa de ISO para este teste.
2. Após desenhar a interface inicial, o app tenta abrir automaticamente o teste
   gráfico nativo. Não precisa navegar até a aba Controle nem apertar X.
3. Observe a imagem. B volta. BACK abre a confirmação de saída no menu.
   Copie default.log antes de iniciar novamente, pois ele é sobrescrito.

O log deve registrar startup: requested action=..., startup: executing
 diagnostic action=..., depois PASS ou FAIL: native GS MRT/DATE/FBA diagnostic
open e, se avançar, o resultado native PSMT8/MODULATE diagnostic pixel.
Mesmo esse pixel não comprova todos os estados GS ou o combate.
Se travar, envie o log parcial e diga se apareceu alguma imagem do teste.

## Segundo teste: modelo

Depois de guardar o primeiro log, troque o conteúdo de startup_test.txt para
model (ou copie startup_test-model.txt sobre startup_test.txt). Mantenha a sua
ISO USA em isos/ no USB/HD já usado. O launcher procura as imagens e tenta
abrir o modelo da primeira ISO encontrada. Se houver várias, deixe só a ISO
USA nessa pasta para este teste. Não precisa copiar a ISO ao lado do XEX.

O modelo é um visualizador em pose de referência, não uma luta e não uma
animação aplicada ao corpo. Registra body e o resultado de carregamento no log.
B volta; X tenta a expressão seguinte enquanto o visualizador está aberto.
Guarde esse segundo default.log com outro nome antes de abrir novamente.

Remova startup_test.txt para voltar ao comportamento manual anterior.
Tokens aceitos: somente graphics ou model. Outros conteúdos geram mensagem
FAIL no log e não escolhem automaticamente outro teste. Nenhum modo se repete
sozinho depois de voltar com B.

## Estado e validação

Novo XEX recompilado para PowerPC com SDK fixado, verificação estática XEX/ELF
passou (ABI, símbolos, cabeçalhos e hashes). Teste host da interface passou.
Este novo binário ainda NÃO foi executado no Xbox/emulador pelo assistente.
O envio ao teste gráfico usa o mesmo caminho da ação manual já existente.
A inicialização automática só ocorre após a primeira checagem de pixel da UI.

O estado do combate não mudou: último perfil offline de compilação tinha
118 referências (15 com código/103 só dados), boot/dados e integração pendentes.
O XEX deste pacote não contém Battle_Main nem Game_Main. Filmes/menus/rede
continuam fora do objetivo da luta fixa.

Checkpoint antigo de fontes experimentais e seus relatórios foram preservados
como histórico; seus patches partem da 0.33 original, não desta variante HW1.
Não aplique esses patches diretamente por cima desta variante sem rebase.
Nenhum recurso comercial é incluído.
