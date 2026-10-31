# 0.47-D13 — cenários animados e comum 3

Há um XEX D13 novo neste pacote. O pedido completo continua pendente: não há
luta jogável, ciclo completo Battle_Load/Battle_Init nem Battle_Update/Battle_Main
executado. O marcador de dados permanece em 2. Não se retirou NOT_READY para
forçar o combate; a prontidão de cada cenário vem do inicializador original.

## Implementação e execução comprovadas

| Área | Resultado e limite |
| --- | --- |
| Cenários | 39 pacotes reais preparados, com MAPD, colisão, posições, geometria, materiais e recursos de navegação disponíveis no arquivo |
| Ciclo original do cenário | Os 39 passaram por Init, 12 Updates, culling em CPU e Term, com encerramento de recursos |
| Animação de objetos | Seis cenários (4, 5, 12, 17, 18 e 27), 16 modelos de animação e 31 definições de objetos animados; contadores avançaram |
| Matrizes | Vu1Node_Animate original executado em três instantes por modelo, com matrizes finitas; sem envio à GPU |
| Temporizadores reais | 19 entradas, com 57 viradas observadas nos ciclos de teste |
| Comum 3 | 635 de 637 entradas preparadas. As duas indisponíveis são 0x23D e 0x23E, sem consumidor/formato identificado |
| Consumidores comuns originais | 24 conjuntos, 50 pacotes V000/197 camadas, 309 chaves de partículas, 2.000 passos de sprites e dois inicializadores de malha |
| Texturas do comum 3 | 116 TexFiles, com 162 imagens e 42 entradas somente de paleta; 204 descritores não significam 204 imagens |
| Rejeição de dados corrompidos | Sete casos do comum 3, oito de animação do cenário, sete de parâmetros/caminhos/navegação e cinco de geometria |
| Gerenciadores e áudio | Ciclos anteriores de objetos, BtlChar/BtlAi, entrada e áudio passaram novamente; áudio usa dispositivo PCM simulado no host |

A execução acima foi i386/QEMU com UBSan/bounds. Não demonstra execução desses
ciclos no PowerPC do Xbox. O heap do jogo foi restaurado após os ciclos.

O MAPD pode conter mais objetos que a colisão: os objetos extras são de animação
e precisam do bit original 0x40000000. A navegação ausente nos quatro cenários
sem objetos permanece ausente: StgNav_Bind retorna -2, sem fabricar um grafo.

Os nós/trilhas e tabelas de animação têm campos numéricos preparados em ordem
nativa. Os comandos VIF mantêm bytes little-endian, com alteração de flags
limitada ao bloco validado. O comprimento da trilha é u16 em +0xC; trilhas de
nós filhos podem ter esse campo zero. A leitura original de pos[3] de uma chave
foi substituída por cópia dos bits do campo frame adjacente, evitando acesso
fora do array sem alterar os bits lidos.

Algumas texturas reais dos cenários 13 e 34 têm uploads GS que cobrem somente
parte do bloco. A preparação valida a estrutura e os limites dos pacotes;
o decodificador RGBA continua recusando cobertura incompleta. Isso não é prova
de pixels nem uma imagem completada artificialmente.

A animação de textura avançada no caminho de DESENHO continua sem validação.
O acesso ao índice de material foi corrigido para usar o byte numérico baixo,
compatível com PowerPC big-endian. Essa correção ligada não prova seu desenho.

## Compilação e catálogo

405/405 unidades do vínculo nativo compilaram. Zero símbolos indefinidos e
zero conflitos de ABI. Catálogo final: 33 regiões, 2.738 operações e 1.779
ponteiros verificados após o vínculo; marcador 2 confirmado no ELF final.
As verificações negativas de raízes legadas continuam falhando como esperado.
Battle_Main está ligado; Game_Main não está ligado e nenhum deles é iniciado.

XEX D13: 13.770.752 bytes.
SHA-256: 14faf41335d3e68c941903437fd948715a3ca77e12132a9b685289364b590ee6.
O D12 anterior foi preservado em diagnostico-anterior-D12.

## Teste real no Xenia solicitado

Release oficial: https://github.com/xenia-canary/xenia-canary/releases/tag/291f0c9
Commit: 291f0c9be398a3e0a0e29f378cbd564fa28c55a9.
AppImage Linux SHA-256:
9b1b17f8c602420a8387da93a8d7ce1436fb5d9129ffd51e1009ff1b21a18eaa.

O XEX D13 acima foi carregado e a thread principal executou no emulador.
Os testes sintéticos BPE, BIN/esqueleto, endereço GS e VU passaram. Vulkan criou
o dispositivo llvmpipe/Mesa e a swapchain 1280x720; é renderização em software.
A abertura da apresentação falhou com "the LUT autofill didn't finish" e
"FAIL: present_open 0xC00000B5". O guest registrou saída 1. O processo do
emulador permaneceu aberto e foi encerrado pelo limite de 40 segundos do teste.
Nenhum quadro do cenário/combate ou áudio do jogo foi comprovado no Xenia.

O SDK não foi alterado para ignorar a falha. O tratamento de registros do
Xenia nessa revisão grava o valor do registro de autofill, sem implementar sua
conclusão: src/xenia/gpu/graphics_system.cc, ReadRegister/WriteRegister, linhas
360–406. Isso explica o timeout observado ao esperar o bit de conclusão; não
foi feito um teste com emulador modificado. Os logs reais estão no checkpoint.

## Bloqueio do boot e pendências

Uma sondagem separada de BtlScene_Init(0) alcança BtlCam_Init e falha em
BtlScene_TestCharPackBit ao consultar o pacote do personagem. O caminho de
modelo do diagnóstico ainda não prepara os pacotes de efeitos usados pelo
boot original. O erro não foi convertido em retorno falso nem contornado.
O log e o trecho final da execução QEMU estão em checkpoint-D13-final.

Restam: preparar/instalar todos os recursos de personagens e internos do
cenário para o loader original; concluir Battle_Load/Battle_Init; executar
Battle_Update/Battle_Main; concluir HUD/cena e resolver a compatibilidade de
apresentação do SDK com o Xenia; validar desenho, animação de textura, skinning,
pixels, mandíbula, lábios, vozes, auras e efeitos. Os dois recursos comuns ainda
sem formato também permanecem indisponíveis. Não há teste em Xbox físico.

Os testes usam dados reais externos. Este pacote não inclui ISO, programas
originais nem os pacotes de recursos do jogo.
