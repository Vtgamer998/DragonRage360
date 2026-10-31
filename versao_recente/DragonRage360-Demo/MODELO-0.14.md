# Corpo real e programa original da VU1

A 0.14 acrescenta um visualizador de desenvolvimento para um modelo real da
ISO USA SLUS-21678. Não é uma luta jogável nem um GsBackend completo.

O caminho novo lê ISO → AFS → BIN → pmdl, valida a hierarquia e os intervalos
das listas do corpo, localiza o programa 0 da VU1 no ELF MIPS original da ISO,
executa seus UNPACK/MSCNT continuamente no interpretador de referência e
converte a primeira camada dos pacotes GIF em triângulos comuns. Não executa
o processador MIPS do jogo. O programa e os modelos não estão embutidos.

As matrizes fixas recompõem uma pose de referência usando os dois pivôs.
Isso exercita o cálculo de vértices e pesos do microprograma real, mas não
integra poses animadas, matrizes mundiais da hierarquia, binders ou combate.
Cada parte recebe uma cor de diagnóstico e uma textura branca de um pixel;
as texturas, o segundo passe de iluminação e as expressões faciais do jogo
não são desenhados. A câmera e o ajuste à tela são próprios deste diagnóstico.

## Uso no console

1. Coloque sua ISO USA em `isos/`, ao lado do XEX, ou em `usb0:/isos/`.
2. Na Biblioteca, selecione a ISO e confirme com A. Feche a mensagem com A.
3. Use LB/RB para ir a Controle. Y solicita o modelo; B volta à interface.
4. X continua abrindo o diagnóstico sintético de paleta/alpha/aditivo.

Sem ISO selecionada e validada, Y procura e usa a primeira ISO encontrada.
O arquivo escolhido é lido sem alterações e fechado depois da geração da
malha. O diagnóstico usa o primeiro recurso de modelo `pzs3us1/01423.bin`;
não há seleção de lutador ou cenário. Os logs distinguem falhas de leitura,
geração e desenho. Nenhuma execução em hardware foi confirmada.

## Evidência de CPU

- 698 modelos: 43.121 nós e 32.454 listas VIF do corpo validadas.
- 590 pacotes de skins ausentes: placeholders de 64 bytes, separados dos modelos.
- Um modelo completo processado continuamente pelo programa real: 69 nós,
  53 listas habilitadas, 564 MSCNT, 564 pacotes GIF e 4.492 triângulos.
- As 127 instruções do programa original correspondem ao `prog0.vsm`
  montado do upstream fixado. O pacote VIF de 1.024 bytes tem SHA-256
  `b57f2d1348cc92e16c70fbce563d157741b440bbe612fca918ad0f52750ad47d`.
- ASan/UBSan: corpus real, hierarquia, todos os truncamentos da fixture,
  capacidade de saída, mapeamento ELF e rejeição de segmentos sobrepostos.
- Chamadas GPU simuladas: divisão em lotes de até 1.023 vértices, fence antes
  de reutilizar a arena e preservação dos recursos em timeout.

Relatórios: `engine/reports/0.14/`. Não são capturas nem testes de pixels da GPU.
O teste de corpo valida todos os leitores; a execução contínua da VU foi
testada em um modelo, não em todos os personagens ou animações.

Para reproduzir no host, com GCC e Python:

```sh
python3 engine/tools/test_body_pipeline.py /caminho/SLUS_21678.ISO /tmp/body-test --sanitize
```

A ISO usada nos testes tem 3.020.816.384 bytes e SHA-1
`a1c4d21c154da8b1d4b15055907edb0abb055e63`. O teste depende desse corpus;
não demonstra compatibilidade com outras regiões ou mods.

## Limites do desenho

O núcleo nativo usa cor, profundidade reversa, shaders MODULATE e uma malha
em cache. O visualizador espera um fence entre lotes para respeitar a arena
de 1.024 vértices; é um caminho de diagnóstico sem meta de desempenho medida.
Targets de cor/profundidade estão ligados como módulo nativo, mas não têm
registro ou contrato completo de GsBackend. A ponte GsDraw continua separada.

Não foram validados os pixels dos shaders, restauração dos targets, shaders de
profundidade ou o modelo no Xbox. Stencil, alpha-test/DATE/FBA, efeitos com
buffers amostrados, mipmaps, texturas reais, auras e explosões continuam fora
desse caminho. O interpretador de VU usa ponto flutuante de referência, sem
promessa de equivalência bit a bit com todas as particularidades do PS2.

O loop original de combate, colisão e física de cenário, tabelas, adaptador de
arquivos do motor, memória/endereço PS2, heap, temporização, áudio e controle
do motor continuam pendentes. O mesmo vale para menus/modos originais, vídeo,
saves durante o jogo, IA em execução, rollback, NAT e partidas online.

Os fontes correspondentes, shaders e testes estão incluídos. A distribuição
continua sob as licenças indicadas em LICENSES.md; não inclui ISO nem recursos
extraídos do jogo.
