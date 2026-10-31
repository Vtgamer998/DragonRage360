# DragonRage360 0.48-D14 — evidência e limites

## Executado

- Dados reais de ISO USA externa, extraídos somente para teste local: 39 cenários, 2 personagens, comuns 2/3/4, áudio e fluxos ambientais. A ISO e seus recursos não acompanham este pacote.
- QEMU i386 com UBSan/bounds: 39 cenários convertidos e exercitados por 12 quadros, seis pacotes de objetos animados, 31 definições e 19 temporizadores; 57 voltas dos temporizadores. Cena original BtlCam/BtlScene Init, três pares Update/PostUpdate e Term em dois ciclos. Quatro conjuntos originais de emissões por personagem; fita tipo 17 e 24 extremos de câmera originais; nove recursos corrompidos rejeitados sem publicação. Resultado `PASS` em `checkpoint-D14-final/host-result.json`.
- Common3: 635 de 637 entradas preparadas; 116 TexFiles, 162 imagens desenháveis, 42 entradas só de paleta, 24 conjuntos de emissões e 50 pacotes V000 exercitados. As duas entradas 0x23D/0x23E permanecem desconhecidas e ausentes.
- Personagens: os gerenciadores de efeitos reais (terceiro arquivo, membro 6) são ligados ao campo original BObj + 0x58 antes da criação da cena. Por lado: quatro conjuntos, 16 texturas e nove pacotes de sprites; duas câmeras à esquerda e uma à direita. Os cinco membros de técnicas de cada terceiro arquivo ainda não são convertidos.
- PowerPC: compilador C do xsdk original passou a prova de convenção `-mcall-xsdk` e alinhamento FPR; ligação estrita nativa do Battle_Main com todas as fontes e sem símbolos não resolvidos. Os XEX console e Xenia foram produzidos em variantes separadas. A variante Xenia preserva a LUT já inicializada pelo kernel do emulador; não solicita a operação de autofill que expirava no D13. O modo de console conserva o comportamento do SDK.
- Xenia Canary 291f0c9 sob Vulkan llvmpipe: o D13 reproduziu `present_open` timeout; o D14 exibiu 1280×720 e leu o pixel `FFFF8A1F` da interface. Com a ISO de teste, carregou dados tipados, inicializou gerenciadores BtlChar/BtlAi, áudio de diagnóstico, cenário e BtlScene_Init com erro 0. Registrou ao menos 60 pares originais BtlScene_Update/PostUpdate no CPU após BtlStage_Update; os logs brutos estão em `checkpoint-D14-final/`.

## Ainda não demonstrado

Battle_Load/Battle_Init completos, Battle_Update/Battle_Main e luta jogável não foram chamados; o marcador NOT_READY permanece em 2. Os cinco pacotes de técnicas por personagem, dois IDs desconhecidos do common3 e os recursos internos necessários para o desenho completo do cenário e do combate ainda faltam. Não houve BtlScene_Draw da luta, skinning/pixels dos personagens, mandíbula, lábios, auras ou efeitos de combate na GPU. O áudio foi solicitado com quatro bancos e uma voz. Uma execução usou `--apu=nop`; com `--apu=any`, o Xenia registrou `SDL_OpenAudioDevice() failed` neste ambiente. Não há prova de saída audível. O teste de pixel é da interface, não da luta.

A ISO USA usada para teste e os recursos do jogo não são distribuídos. O XEX do console é `default.xex` na raiz; a variante para o Xenia 291f0c9 fica em `xenia-291f0c9/default.xex`. Para repetir o teste tipado, coloque sua própria ISO USA ao lado do XEX e um arquivo `startup_test.txt` com `typed-data`.
