# Luta fixa — hardware e dependências

## Confirmado no Xbox pelo usuário

Os logs recebidos confirmam o diagnóstico 0.33: abertura de display/texto,
pixel da interface FFFF8A1F, A/B, navegação (default4.log), leitura da ISO
USA de 3.020.816.384 bytes, sondagem nativa open/read/seek/search do ELF e
leitura/conversão de animação pzs3us1/01431.bin, entrada 0: 5.384 bytes.
Saída normal com status 0. Não demonstram luta, modelo desenhado ou o teste
GS MRT/DATE/FBA. O ISO=0 do primeiro log significa que a ação de validação
não foi registrada nessa execução; a leitura da animação passou mesmo assim.

Ausência de demo.cfg e de usb0:/isos não bloqueou esses testes. A ISO está
em usb1:. resource-converters.txt não existe: o registro global tem zero
faixas. As sondagens de primeiro setor de três arquivos não instalaram
conversores, enquanto a animação usa outro caminho explícito e passou.
Não extrapolar uma animação convertida para todos os dados do jogo.

## Continuação de código: perfil offline de compilação

engine/combat/offline_profile.c contém oito consultas de configuração usadas
pelo port: cenário 0, uma tela/visão do lado 0, sem sessão de rede e sem
opções negociadas remotas. Port_TestBgm=-1 significa manter a escolha do
chamador, NÃO desligar áudio. Não há transporte/lobby nesse módulo.

link_battle.py --fixed-battle inclui esse perfil somente quando solicitado.
Exige --platform e Battle_Main como única raiz; recusa Game_Main. O perfil
padrão do script e o launcher não recebem automaticamente essas definições.
Isso não cria os dois lutadores nem um boot; apenas fixa opções do build.

Compilação PPC: 392/392 unidades, zero conflitos de ABI. Vínculo estrito:
118 referências pendentes, sendo 15 com referências em código e 103 apenas
em dados na auditoria de seções alocadas, incluindo seções descartadas por GC.
Nenhuma tabela removida, nenhuma verificação do linker desabilitada.

Comparação de escopos:
- Tentativa anterior combinando Game_Main/Battle_Main: 195 nesta árvore WIP.
- Battle_Main sem perfil novo: 126.
- Battle_Main com perfil offline: 118.
A passagem 195 para 126 é mudança de raiz, não 69 funções implementadas.
As oito consultas de configuração explicam a mudança de 126 para 118.
Os antigos 133 e 207 pertencem a revisões/escopos anteriores.

## O que falta para a luta fixa

1. Eliminar/adaptar dependências não essenciais pelos seus chamadores no
   perfil offline, preservando HUD e texto que a luta realmente usa.
2. Resolver as tabelas de dados/callbacks com implementações reais ou substitutos
   nativos comprovadamente equivalentes. Os 103 nomes não podem ser ignorados.
3. Completar manifesto tipado dos dados do programa principal: endianness,
   ponteiros nativos, layout e inicialização, sem copiar instruções MIPS.
4. Boot nativo: heap/arena, arquivo, controles, GS, objetos e configuração dos
   dois lutadores e cenário. Ligar carregamento, animação/skinning, colisões,
   IA, pacotes de desenho e apresentação ao loop de Battle_Main.
5. Executar esse caminho no Xbox, guardar logs por etapa e validar imagem,
   controles e estabilidade. O teste atual ainda não percorre esse caminho.

Os 15 nomes com referências em código são:
- DrNet_BattleFinished
- MenuText_RegisterGlyphs
- PortA11y_FontText
- PortQuit_Draw
- PortTbo_TextId
- PortTbo_TextInfo
- Port_ClashLog
- Port_ClashSquare
- Port_HudSwap
- Port_LobbyCode
- Port_LobbyInBackground
- Port_RumbleTest
- UiText_TranslateString
- func_002967C0
- sceSifCallRpc

Não são todos funções de IA/colisão: há extensões de interface/rede e RPC.
A disponibilidade de símbolos de IA não prova que seus dados e chamadas
estejam corretos em execução. O vínculo continua falhando e não produz XEX
de combate. Não há luta jogável neste checkpoint.

## Próximo teste possível no XEX já instalado

Na Biblioteca, valide a ISO com A. Vá à aba Controle com RB. X abre o teste
gráfico nativo; B volta. Nessa aba, Y abre o modelo da ISO em pose de referência.
Dentro do teste gráfico, Y tenta a captura GS/VU. Guarde o default.log depois
de cada tentativa, antes de iniciar novamente o aplicativo. Se travar, anote
qual ação provocou. Esses testes são diagnósticos; não iniciam uma luta.

## Conteúdo deste pacote

O default.xex continua exatamente o diagnóstico 0.33 testado pelo usuário.
O arquivo battle_resource.txt foi corrigido para 01431.bin 0 1.
Fontes experimentais estão no patch cumulativo extras/fixed-battle-INCOMPLETO.patch
(da base 0.33 à árvore atual). Use esse patch no lugar do antigo patch de vídeo,
não aplique os dois. Vídeo continua pausado; suas alterações anteriores foram
preservadas, sem adicionar funcionalidades de vídeo nesta continuação.
Nenhuma imagem ISO ou recurso comercial é incluído.

Reprodução do perfil (após aplicar o patch numa cópia dos fontes e compilar
os objetos nativos conforme Makefile):
python3 engine/tools/link_battle.py UPSTREAM CC XSDK OUT --float-mode native-ps2 --platform --fixed-battle
python3 engine/tools/audit_engine_refs.py OUT AUDIT_OUT
O primeiro comando deve terminar com falha enquanto houver referências.
