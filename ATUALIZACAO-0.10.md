# Entrega de fontes 0.10

Não há luta jogável. O pedido de concluir todo o port não foi atendido.

## Código acrescentado

- Ponte que lê as estruturas `GsDraw`/`Vtx` originais e resolve pipelines e texturas para uma única superfície diagnóstica.
- Validação da lista inteira no núcleo nativo, impedindo emissão parcial por erro em um comando posterior.
- Testes com o cabeçalho original e chamadas xsdk simuladas: limites, pipelines, handles, vértices, estados incompatíveis e ciclo de batch.

## Verificação desta execução

- Compilação host com `-Wall -Wextra -Werror` e teste de replay: aprovado.
- ASan/UBSan: aprovado, com detecção de vazamentos desativada pela restrição de `/proc` do ambiente.
- Decoder: 27000 leituras e 38912 pixels comparados com o código GS original: aprovado.
- Compilação PowerPC dos fontes novos: não realizada; compilador xsdk indisponível nesta execução.
- GPU, emulador, ISO real e Xbox: não executados.

Os detalhes e os comandos reproduzíveis estão em `engine/graphics/REPLAY.md`.
Os relatórios desta rodada estão em `engine/reports/0.10/`.

## Executável incluído

O `default.xex` permanece byte a byte o diagnóstico 0.9. Seus fontes em `src/`
também foram preservados. As modificações desta entrega estão em `engine/` e
não alteram o comportamento desse XEX. Isso evita apresentar fontes testados
somente em host como um novo jogo executável no console.

## Para chegar à luta

Ainda faltam os targets e callbacks reais de GsBackend, shaders e VU integrados,
modelos e animações reais, arquivos e memória do motor, plataforma e loop de
combate. Modos, áudio/vídeo, saves em jogo, IA, rollback/NAT e estabilidade
continuam pendentes. Não há medição de FPS nem validação no console.
