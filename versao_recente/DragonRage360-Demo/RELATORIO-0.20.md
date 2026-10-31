# 0.20 — shaders VU nativos e grupos de sombra; combate ainda não integrado

**O pedido completo não foi concluído.** Esta entrega amplia o renderizador
ligado ao XEX. Não liga `Battle_Main` à plataforma Xbox, não executa uma luta
original e não conclui todos os estados/efeitos do GS. Não houve execução em
Xbox ou emulador, nem validação de pixels em hardware.

## Implementação nova

1. Três shaders de vértice nativos, derivados dos shaders públicos do motor:
   - `dr_vu0.vert`: skinning de dois ossos, pivôs, projeção, camada de material,
     coordenadas toon pela normal e variantes de camada 0..5 da captura;
   - `dr_vu4.vert`: transformação e cores de geometria de cenário;
   - `dr_vu6.vert`: projeção das coordenadas da textura de sombra.
   A ponte usa os índices, vértices e `Vu0Uniform` originais da captura. Expande
   os índices em uma arena limitada e envia os dados ao shader correto. O
   interpretador CPU do visualizador anterior continua separado.
2. O backend recebe as arenas `gsVuVerts`, `gsVuIdx` e `gsVuUni`. Todos os
   índices, contagens, seletores e valores são verificados antes de desenhar.
   Programa desconhecido, índice fora da arena, profundidade zero na constante
   ou vértice NaN rejeitam o quadro inteiro antes de emitir primitivas.
3. A profundidade de fragmento usa um varying com `(clip.z, clip.w)`. O shader
   calcula a razão interpolada e pode quantizá-la pelo máximo GS; a ponte agora
   traduz os quatro testes Z e escrita de profundidade. O alvo físico continua
   D24S8: isso não reproduz toda a precisão de armazenamento do GS de 32 bits.
4. Targets amostrados recebem clamp de UV por fragmento usando os limites do
   descriptor. Isso permite o caminho VU com target como textura sem exigir
   que o host estime suas coordenadas já transformadas. Region-repeat e outros
   comportamentos de endereço do GS continuam incompletos.
5. O bit 24 da chave de pipeline da sombra passa à implementação de stencil:
   limpar stencil no início do grupo, testar zero e incrementar no depth pass.
   A limpeza desenha com máscaras de escrita de ambos os MRTs zeradas e sem
   escrita Z. Marcadores nativos, mudança de target ou saída do grupo delimitam
   o próximo início, seguindo a regra do backend original. São chamadas e
   estados implementados; o resultado dos pixels ainda precisa de console.
6. Sprites sem textura usados para limpar o quadro deixam de ser rejeitados
   pelo sinalizador de amostragem 2D, que não afeta a cor sem textura. Sprites
   com regras de grade/textura ainda não implementadas continuam rejeitados.

Os ajustes para o compilador Xenos preservam as equações no domínio aceito:
`mat3 * normal` vira `mat4 * vec4(normal, 0)`; clamp com limite variável usa
min/max; a comparação de camada evita um operador não suportado. As camadas
aceitas são inteiras 0..5, e dados não finitos são rejeitados. Os testes incluem
rotações, escalas e translações distintas nos dois ossos.

## Caminho disponível no XEX

Em **Controle**, **X** abre o diagnóstico MRT/DATE/FBA; **Y**, dentro dele,
abre a captura original. A captura agora envia limpeza GS, geometria comum,
modelo com constante de osso animada, cenário, projeção de sombra e duas
primitivas coincidentes do grupo de sombra com DATE/FBA/stencil. **B** retorna.

**Toda a geometria e as constantes desse diagnóstico são sintéticas.** Não é
um personagem animado da ISO nem uma luta. O visualizador separado da ISO
continua com pose de referência e execução CPU do programa VU comercial.
O roteiro acima ainda não foi confirmado no console.

## Verificação

- Os shaders foram compilados para microcódigo Xenos com glslang + xost/xsc.
- `test_vu_shaders.py`: 128 casos de vértice comparados a equações escalares
  independentes de matrizes/iluminação/projeção, mais 30 casos de profundidade
  de fragmento. Inclui pesos, camadas, osso rotacionado/escalado, perspectiva
  e projeção com w negativo. O microcódigo roda no simulador CPU do xost.
- `test_gs_materials.py`: 5760 combinações de material, com 2560 descartes.
- `test_gs_buffers.py`: 216 casos DATE/FBA, 16 de alfa de target, 27 de clamp
  de UV e 2304 seletores de stencil. Esses números são testes de microcódigo
  em CPU, sem rasterização ou GPU.
- Teste da captura completa com ASan/UBSan: três programas VU selecionados,
  upload de constantes, limpeza de quadro, índices, grupo de sombra e quatro
  ciclos de abertura/fechamento. Verifica também que falhas no último comando
  impedem os comandos anteriores de chegarem ao desenho.
- Testes de backend/registro, replay, targets e visualizador: chamadas de GPU
  simuladas, sem pixels. Detecção de leaks desativada nos testes com ASan.
- Build PowerPC BE e verificações estruturais do XEX em `BUILD-RESULT.txt` e
  `VALIDACAO.json`. Fontes e logs atuais em `engine/reports/0.20/`.

Os testes de host não provam a fidelidade de clipping, interpolação, blend,
profundidade, DATE/FBA, stencil, resolves, alias D24S8/RGBA8 ou faixas eDRAM no
Xbox. A escrita de profundidade no shader também pode afetar early-Z e custo
de GPU; não houve medição de CPU/GPU, memória total ou FPS.

## O que impede declarar a solicitação concluída

| Pedido | Estado real |
| --- | --- |
| GsBackend completo ligado à simulação | Backend/captura ligados ao título; `Battle_Main` não ligado. Memória PS2, recursos e plataforma de combate ainda precisam de adaptação |
| Sombras completas | Skinning/projeção e máscara de overlap com stencil implementados; sequência completa com dados originais e pixels ainda não validada |
| Consumidores de buffers | Textura de target, snapshots e clamp disponíveis; outline, tabelas de profundidade/fog e demais passes nativos continuam faltando |
| VU/GPU e iluminação original | Caminho de três shaders e variantes de camada implementado e testado em CPU; ainda sem integração de uma luta/poses reais e sem confirmação da iluminação no hardware |
| Demais estados GS | Faltam regras completas de amostragem 2D/region-repeat/mips PS2, máscaras parciais, formatos/precisões, fog e outros estados |
| Auras e explosões | Sequências originais e seus recursos não integrados; os desenhos sintéticos não são esses efeitos |
| DATE, FBA e stencil em hardware | Pendente; nenhum binário foi executado em Xbox/emulador nesta entrega |

O replay continua limitado a 1024 vértices agregados, incluindo os seis de
cada limpeza de stencil; 256 texturas e orçamento de 128 MiB de superfícies/
snapshots. Esses limites não tornam o backend suficiente para um quadro de
combate completo. Não há suporte novo de IA, áudio, física, rollback ou online.

## Reprodução e licença

Veja `BUILD.md`. A revisão base permanece
`3ca419c7966b11438e2dc3c9a7e1914f102ed545`; shaders e captura são derivados do
upstream AGPL. XSDK fixado em `57b02f1e60667e37d22c3aa613d23d2486514ff8`.
Os shaders-fonte, headers gerados, adaptadores e testes acompanham a entrega.
Não há ISO ou recursos comerciais no pacote. Relatórios 0.19 e anteriores
registram somente suas respectivas versões.
