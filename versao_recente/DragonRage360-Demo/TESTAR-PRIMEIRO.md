0.52-D18: o default.xex continua sendo diagnóstico, não uma luta. Use sua ISO USA e startup_test.txt com typed-data. Para captura externa de vídeo e áudio SDL em arquivo, siga RELATORIO-0.52-D18.md. O teste em Xbox real ainda é necessário.
0.51-D17: copie default.xex, coloque sua ISO USA ao lado do XEX (ou em isos) e crie startup_test.txt com typed-data. Este diagnóstico prepara dez pacotes de técnicas e executa a cena em CPU; não inicia Battle_Main. A variante xenia-291f0c9/default.xex usa a política de display do emulador. Leia RELATORIO-0.51-D17.md para resultados e limitações.
0.50-D16: copie default.xex, coloque sua ISO USA ao lado do XEX (ou em isos) e crie startup_test.txt com typed-data. O diagnóstico prepara sete pacotes de técnicas e executa a cena em CPU; não inicia Battle_Main. A variante xenia-291f0c9/default.xex usa a política de display do emulador. Leia RELATORIO-0.50-D16.md para resultados e limitações.

--- Histórico anterior ---

# Testar D12 no Xbox 360 RGH

Copie default.xex e startup_test.txt do pacote D12 para a pasta do aplicativo.
Mantenha a ISO USA na pasta isos e inicie pelo Aurora. startup_test.txt seleciona
typed-data. Continua sendo um diagnóstico, sem partida jogável.

Procure em default.log:

- native stage geometry: error=0, com 2.243 malhas e 25 texturas
- common3 native texture subset: error=0 entries=637 textures=111 images=154 unprepared=526
- original CPU manager boot: error=0
- original initialized fighter: dois lados com HP=40000, Ki=40000, AI_level=13
- original audio diagnostic: error=0
- original stage init/reset: error=0

O programa solicita uma voz e inicia o ambiente sonoro pelo inicializador
original do cenário. Confira se há som no console. error=0 não comprova saída
audível: o teste host usou um coletor PCM simulado. Falhas de serviço de cenário
ou áudio são registradas no log.

O inicializador original controla a prontidão do cenário. Não retire bloqueios
manualmente. O marcador do combate continua em 2; Battle_Load/Battle_Init
completos, Battle_Update/Battle_Main, os 526 recursos restantes do comum 3,
HUD/cena e apresentação GPU continuam pendentes. Veja RELATORIO-0.46-D12.md.
O cenário não será desenhado por esse diagnóstico.
