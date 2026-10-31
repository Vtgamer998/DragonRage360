# Relatório do port — versão experimental 0.16

23 de outubro de 2026.

O projeto ainda não é um jogo executável com luta. A atualização 0.16 liga
materiais reais à malha do modelo de teste e acrescenta seleção manual de
cinco substituições de geometria facial. Foi compilado um XEX novo para
PowerPC. Nenhuma versão nova foi executada em Xbox real ou emulador.

## Implementação desta atualização

O vínculo de materiais segue BtlObj_BindTables: a textura do corpo vem da
entrada canônica 11 do BIN, equivalente ao índice 0xC usado pela tabela
original. A entrada 0, usada nas prévias da 0.15, continua como prévia separada.

O programa 0 da VU1 agora publica STQ e TEX0 da primeira camada por triângulo.
O leitor encontra uma entrada única do TexFile pelos offsets relativos de
pixels/paleta e pelo formato. As 13 texturas usadas no modelo de teste são
decodificadas da ISO e enviadas ao núcleo nativo. A malha usa cores neutras
para modular essas texturas, substituindo as cores por parte da 0.15.
Apenas MODULATE com TCC está aceito nesse caminho; modos não suportados
são rejeitados. Isso não completa a emulação de estado do GS.

Os triângulos são agrupados por material sem ultrapassar a arena de 1.024
vértices. O viewer usa até 14 materiais, mais duas prévias, dentro do limite
de 16 texturas do núcleo. O modelo e cada forma facial testada usam 13.
O desenho continua um caminho de diagnóstico com espera entre lotes, sem
meta de desempenho medida.

Para o rosto, a lista VIF do nó 0x30 pode ser substituída pelas entradas
canônicas 3..10, correspondentes à tabela mouthA original. O modelo de teste
possui formas 0..4; os slots 5..7 estão vazios. A seleção pula os vazios.
É substituição manual de geometria em pose fixa. Não há reprodução de fala,
curvas de expressão, troca dinâmica dos olhos ou animação facial completa.
A câmera do diagnóstico foi orientada para a frente do modelo.

## Resultados comprovados

| Teste | Resultado | Limite |
| --- | --- | --- |
| Corpo com materiais | 4.492 triângulos, 13 referências TEX0 encontradas e decodificadas; STQ finito | Um modelo, pose de referência |
| Geometria facial | Cinco substituições processadas pelo programa VU e com materiais resolvidos | Seleção manual; não é lip-sync |
| Slots faciais vazios | 5, 6 e 7 rejeitados sem publicar saída parcial | Não significa que todos os personagens tenham cinco formas |
| Teste integrado com ISO real | Pose padrão + cinco formas abriram, emitiram chamadas de desenho e fecharam os recursos | GPU simulada; sem rasterização |
| Lifetime com mocks | Troca A/B/A de materiais entre lotes, fences, falhas de carregamento/alocação e timeout passaram | Não executa Xenos |
| Leitor e modelo | Varredura anterior de 698 corpos/43.121 nós/32.454 listas VIF repetida; passou | Leitura e referência, sem combate |
| Matcher TexFile | Seleção por TBP/CBP, flags de desenho, ausência e ambiguidade testadas; bounds e truncamentos passaram | Subconjunto de formatos documentado |
| Build PowerPC | XEX2 gerado e link sem símbolos pendentes; módulos novos presentes | Não é teste de boot |
| Alinhamento PPC | Três módulos alterados compilados com -mcheck-fpr-alignment | Compilação, não execução |

Contagens da geometria facial no modelo de teste:

| Seleção | Triângulos | MSCNT |
| --- | ---: | ---: |
| Padrão do pmdl | 4492 | 564 |
| Forma 0 | 4562 | 566 |
| Forma 1 | 4591 | 569 |
| Forma 2 | 4576 | 570 |
| Forma 3 | 4576 | 570 |
| Forma 4 | 4497 | 561 |

Os testes de CPU e de chamadas simuladas foram executados com ASan/UBSan;
a detecção de leaks estava desativada. Não há validação de vazamentos em
partidas, de pixels de shaders/targets no console ou de desempenho de jogo.
Uma inspeção local por rasterização de software confirmou a associação
visual de materiais, mas não é uma imagem produzida pelo Xenos.

Os números da 0.15 (16.156 decodificações e 419.072 pixels de referência)
são evidência daquela versão do decoder, que foi mantido. Esta atualização
acrescenta o vínculo dos materiais e as substituições, não repete uma
alegação de verificação visual de todas as texturas do jogo.

## Uso no console RGH/JTAG

1. Coloque sua ISO USA SLUS-21678 em isos/, ao lado de default.xex.
2. Abra o XEX, selecione a ISO na Biblioteca e confirme com A.
3. Na aba Controle, Y solicita o visualizador.
4. No visualizador, X alterna entre padrão e as cinco geometrias disponíveis;
   B volta. A mudança reconstrói o diagnóstico e pode levar tempo.
5. default.log registra leitura, erros, número de materiais e seleção facial.

Esse é o comportamento implementado, sem confirmação de funcionamento no
console. Só o primeiro modelo de diagnóstico é selecionado. Não há seleção
de personagem/cenário ou gameplay. Os recursos são lidos da ISO, que não é
modificada. A imagem e recursos extraídos não acompanham o pacote.

## Estado da lista solicitada

| Área | Estado atual e trabalho restante |
| --- | --- |
| Renderização | Materiais da camada base ligados ao visualizador. Ainda faltam GsBackend completo/registrado, stencil, alpha-test/DATE/FBA, efeitos com buffers amostrados, mipmaps, segunda camada de iluminação, auras e explosões |
| Modelo e VU | Programa 0 com dados reais e matrizes fixas. Faltam matrizes animadas, hierarquia aplicada às poses e os demais programas VU |
| Facial | Substituição manual de cinco geometrias no modelo de teste. Faltam animação, fala, olhos e validação dos demais personagens |
| Dados | Ainda faltam colisão/física de cenário, tabelas, outros contêineres e adaptador de arquivos do motor |
| Animações | Conversores/patch PlayModel separados; binders restantes e captura/reconstrução da arena no rollback pendentes |
| Plataforma | Launcher tem ISO/controle/rede/tempo. Memória/endereço PS2, heap, áudio, temporização e controles dentro do motor continuam pendentes |
| Combate | Sem loop original integrado, IA, colisões e animações em execução conjunta |
| Conteúdo | Menus e modos originais, áudio, vídeo e saves durante o jogo continuam pendentes |
| Online | Lobby LAN separado. Sem input de luta, rollback determinístico, NAT, matchmaking pela internet ou antitrapaça |
| Desempenho/estabilidade | Sem partidas completas, medições CPU/GPU de combate, testes de pixels ou validação no Xbox. Não há evidência de 60 FPS |

Não é possível declarar toda essa lista concluída com os testes atuais.
O próximo trabalho de motor é ligar a avaliação de poses e matrizes ao
carregamento e ao renderizador; o loop de combate depende dessa integração
e das camadas de plataforma/dados, não apenas de receber um log do console.

## Reprodução

```sh
python3 engine/tools/test_materials.py /caminho/SLUS_21678.ISO /tmp/materials --sanitize --viewer /caminho/xsdk
python3 engine/tools/test_body_demo.py /caminho/xsdk /tmp/viewer --sanitize
python3 engine/tools/test_body_pipeline.py /caminho/SLUS_21678.ISO /tmp/body --sanitize
make XSDK=/caminho/xsdk TOOLCHAIN=/caminho/xsdk-gcc/bin/powerpc-xsdk-elf-
python3 tests/check_xex.py
```

O teste materials aceita opcionalmente --private-dump para inspeção local.
Qualquer dump contém recursos do usuário e deve ficar fora de distribuições.
Os fontes atuais em src/ correspondem aos módulos em engine/. Relatórios,
mapa de símbolos e configuração do compilador: engine/reports/0.16/.

Upstream fixado: 3ca419c7966b11438e2dc3c9a7e1914f102ed545.
xsdk fixado: 57b02f1e60667e37d22c3aa613d23d2486514ff8.
O build usa o estágio C/bootstrap com patches/specs xsdk e newlib instalada;
a configuração completa está no relatório. O estágio final C/POSIX não foi
usado para esta entrega, e não há código C++ no título.

XEX SHA-256: ff5a209b8bd9ee19676e6fae1b9150448c7e77e27325b35f68e35be89ff7d48d
