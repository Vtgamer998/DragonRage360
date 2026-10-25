# Relatório e continuação — DragonRage360 0.15

Data: 23 de outubro de 2026.

O projeto continua experimental e não executa uma luta. Esta atualização
acrescentou o carregamento limitado de TexFile, decodificação de texturas
reais e duas prévias separadas no visualizador. Foi gerado um XEX novo para
PowerPC. Nenhuma versão nova foi executada no Xbox ou emulador.

## O que foi feito nesta atualização

- Leitor de TexFile com offsets explícitos little-endian e limites reais.
- Validação do pacote VIF DIRECT e dos blocos GIF de transferência CT32,
  TRXPOS/TRXREG/TRXDIR, IMAGE e TEXFLUSH, antes de usar os pixels.
- Reconstrução da VRAM em CPU e decodificação de CT32, PSMT8 e PSMT4 com
  paletas CT32/CSM1. A cobertura dos dados enviados é conferida antes de
  publicar RGBA; áreas não enviadas não são aceitas como pixels válidos.
- Duas prévias do primeiro TexFile do pacote do modelo, carregadas da ISO e
  enviadas ao núcleo gráfico nativo. A malha conserva suas cores por parte.
- Testes de falha de carregamento das prévias e de alocação de textura,
  incluindo liberação dos recursos já criados e preservação em timeout.

## Evidência e limites

| Verificação | Resultado | O que não comprova |
| --- | --- | --- |
| Corpus TexFile em CPU com ASan/UBSan | 2.148 arquivos, 16.156 texturas decodificadas | Correção visual de todas as imagens ou GPU |
| Formatos do corpus | 4.441 CT32; 1.396 PSMT8; 10.319 PSMT4 | Outros formatos, mipmaps ou CSM2 |
| Comparação independente | 10 variantes, 419.072 pixels RGBA idênticos | Todos os pixels do corpus |
| Limites do leitor | Todos os truncamentos da fixture, capacidade, offsets inválidos e transferências não suportadas rejeitados | Todos os arquivos malformados possíveis |
| Visualizador com GPU simulada | Lotes/fences, duas prévias, falhas e reabertura passaram | Rasterização ou desempenho do Xenos |
| Build PowerPC e link | XEX gerado; leitor TexFile e visualizador presentes; nenhum símbolo pendente | Boot ou comportamento no console |
| Check de alinhamento PPC | dr_texfile.c e o visualizador passaram com -mcheck-fpr-alignment | Execução de instruções no hardware |
| Xbox real / emulador | Não executado | Pixels, estabilidade e FPS continuam desconhecidos |

A comparação usa o decodificador Python do upstream fixado e os pixels
lineares originais para CT32. O corpus veio da mesma ISO USA de 3.020.816.384
bytes, SHA-1 a1c4d21c154da8b1d4b15055907edb0abb055e63. Nenhum recurso
comercial extraído acompanha o pacote. A detecção de leaks estava desativada
nos testes ASan; não foi validada ausência de vazamentos durante gameplay.

Os resultados de corpo/VU da 0.14 permanecem como evidência anterior:
698 modelos, 43.121 nós e 32.454 listas VIF lidas; um corpo processado
continuamente pelo programa 0 da VU1 gerou 4.492 triângulos. Esses números
não representam lutas, animações completas ou execução em Xbox.

## Uso do XEX novo

1. Copie a pasta para o Xbox 360 RGH/JTAG e coloque sua ISO USA em isos/.
2. Abra default.xex, selecione a ISO na Biblioteca e confirme com A.
3. Vá à aba Controle com LB/RB e pressione Y.
4. O caminho novo solicita o corpo em pose de referência e duas prévias
   de textura à direita. B retorna à interface. X mantém o diagnóstico sintético.
5. Consulte default.log caso a abertura, leitura ou geração falhe.

Esse é o comportamento implementado, ainda sem confirmação visual no console.

## Materiais: pendência imediata

As duas prévias são do primeiro TexFile do pacote. A amostra TEX0 da primeira
parte habilitada do corpo indica uma textura de 256×256/PSMT4; as prévias são
64×64/PSMT8. É necessário resolver os recursos e o vínculo de materiais antes
de aplicar texturas à malha. Esta versão não substitui esse vínculo por uma
associação arbitrária. UVs, seleção de materiais, referências de blocos,
camada de iluminação e substituições faciais precisam entrar no renderizador.

## Restante do port

| Área | Trabalho restante |
| --- | --- |
| Gráficos | GsBackend completo/registrado; stencil; alpha-test/DATE/FBA; efeitos com buffers amostrados; materiais reais na malha; mipmaps; auras e explosões; validação de pixels |
| Geometria e animações | Matrizes animadas, hierarquia no renderizador, binders restantes e captura/reconstrução da arena no rollback |
| Dados e plataforma | Colisão/física de cenário, tabelas, demais contêineres, adaptador de arquivos do motor; memória/endereço PS2, heap, áudio, temporização e controles do motor |
| Combate e conteúdo | Loop original, IA/física em execução, seleção de personagem/cenário e modos originais |
| Áudio/vídeo/saves | Decodificação e sincronização; leitura/gravação durante gameplay |
| Online | Input de luta, determinismo/rollback, NAT, matchmaking pela internet e antitrapaça |
| Validação | Boot do XEX, pixels, partidas e transições reais, medições de CPU/GPU/memória e estabilidade |

O lobby LAN e a exportação RAW continuam separados do jogo. Não há promessa
ou medição de 60 FPS. Os testes de leitores não completam esses subsistemas.

## Reprodução e artefatos

Fontes atuais: src/ e engine/graphics/dr_texfile.*. Os módulos duplicados
permanecem idênticos. Fontes, testes, licenças e relatórios acompanham o XEX.

```sh
python3 engine/tools/test_texfile.py /caminho/SLUS_21678.ISO /caminho/upstream /tmp/texfile-test --sanitize
python3 engine/tools/test_body_demo.py /caminho/xsdk /tmp/viewer-test --sanitize
make XSDK=/caminho/xsdk TOOLCHAIN=/caminho/xsdk-gcc/bin/powerpc-xsdk-elf-
python3 tests/check_xex.py
```

Upstream: 3ca419c7966b11438e2dc3c9a7e1914f102ed545.
xsdk: 57b02f1e60667e37d22c3aa613d23d2486514ff8.
O compilador utilizado continua o estágio C/bootstrap com os patches/specs
do xsdk e newlib instalada, documentado no relatório da 0.14. Não se trata
de um build com o estágio final C/POSIX nem de validação de hardware.

Relatórios atuais e mapa de símbolos: engine/reports/0.15/.
VALIDACAO.json contém o hash e as verificações estáticas do XEX atual.
