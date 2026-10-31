# Renderização — atualização experimental 0.17

23 de outubro de 2026.

**A lista solicitada não está concluída. Não existe GsBackend completo e
registrado, nem combate executável.** Este pacote acrescenta implementação
real de materiais, testes de alfa e blends ao núcleo nativo, mais upload de
mipmaps explícitos. O XEX foi recompilado, mas não executado em Xbox ou emulador.

## Alterações implementadas

O shader nativo agora calcula os quatro modos TEX0.TFX: MODULATE, DECAL,
HIGHLIGHT e HIGHLIGHT2, com TEX0.TCC ligado ou desligado. As equações seguem
`port/src/gs/shaders/gs.frag`, na revisão upstream fixada. A entrada de cor
nativa continua usando 1.0 = 128; a conversão para a escala do shader original
é explícita. O caminho antigo do visualizador mantém MODULATE/TCC.

TEST.ATST aceita desligado e os oito comparadores: NEVER, ALWAYS, LESS,
LEQUAL, EQUAL, GEQUAL, GREATER e NOTEQUAL. A comparação arredonda o alfa para
unidades inteiras do GS antes de aplicar AREF. Isso implementa o descarte do
shader; não significa que todo o caminho TEST/AFAIL do motor esteja integrado.

A ponte que recebe o cabeçalho GsDraw original traduz TFX/TCC/ATST, AREF,
máscaras RGBA e a chave de blend do motor. O núcleo configura fatores,
soma/subtração/subtração inversa e constantes pela API pública do xsdk. O bit
9 da chave escala RGB pelo alfa no shader antes do blend com fator ONE.
O replay continua separado do executável e do gs_draw.c. As capacidades
novas do núcleo/shader estão compiladas no XEX; a ponte foi compilada para
PowerPC e testada no host, sem fazer parte desse link.

A API `dr_xenos_texture_create_mips` valida a cadeia completa antes de alocar,
aceita níveis RGBA explícitos com dimensões reduzidas até 1x1 e envia cada
nível ao upload tiled do xsdk. O caminho de uma textura sem mips usa a mesma
API com um nível. Não há geração automática, extração dos mips originais da
ISO ou integração de TEX1/MIPTBP e dos samplers do GS. O visualizador continua
enviando somente o nível base.

A compilação direta do shader original encontrou instruções/recursos não
aceitos pelo compilador aberto nesta configuração. A variante de materiais
foi adaptada e compilou. Isso não dispensa adaptar os demais shaders, os
attachments e o replay; não é apenas uma pendência de teste no console.

## Verificações executadas

| Verificação | Resultado | Limite da evidência |
| --- | --- | --- |
| Microcódigo nativo do shader | 5.760 casos passaram; 2.560 descartados conforme a referência | Simulador de CPU xost; sem rasterização, filtragem ou GPU |
| Materiais | Quatro TFX, dois TCC, ATST desligado/0..7, AREF 0/127/128/255 e escala pelo alfa | Entradas sintéticas controladas; não são frames de uma luta |
| Blends | 108 combinações de fatores/operações passaram | Traços de chamadas, não mistura de pixels na GPU |
| Mipmaps | Cadeia explícita de três níveis aceita; cadeia longa, bytes incorretos e ponteiro ausente rejeitados antes de alocar | API de upload com mocks; sem textura mipada da ISO |
| Ponte GsDraw | ABI original, materiais, alfa, máscaras e preflight passaram com ASan/UBSan | Ponte fora do motor e do XEX |
| Visualizador existente | Testes de lotes, fences, falhas, fechamento e reabertura passaram com ASan/UBSan | GPU simulada |
| PowerPC | Build sem warnings, link sem símbolos pendentes; dois módulos alterados passaram em -mcheck-fpr-alignment | Não executa instruções PPC |
| XEX | ELF32 BE/ABI xsdk, PE Xbox e cadeia completa de hashes passaram | Não comprova boot ou pixels |

A detecção de leaks dos testes sanitizados estava desativada. Não há medição
de CPU/GPU ou comprovação de desempenho. O shader mais amplo também precisa
de medição real. Nenhuma imagem é apresentada como captura do Xbox.

## Estado exato do pedido

| Item pedido | Estado ao entregar |
| --- | --- |
| GsBackend completo e registrado | Pendente: posse de recursos, callbacks, registro no gs_draw.c e replay completo do motor |
| Stencil | Pendente, inclusive preservação/restauração entre targets |
| Alpha-test | ATST implementado no núcleo/shader e traduzido na ponte; TEST/AFAIL completo no motor ainda não integrado |
| DATE/FBA | Pendentes; requerem a camada de alfa auxiliar e cópias corretas dos attachments |
| Efeitos com buffers amostrados | Pendentes; alvo como textura e cópias/sincronização ainda sem implementação GS completa |
| Materiais e modos do GS | TFX/TCC e blends ampliados; demais estados, samplers, profundidade quantizada e modos de efeitos continuam parciais |
| Mipmaps | Upload nativo explícito implementado; recursos e estado mip do GS ainda pendentes |
| Iluminação original | Pendente: camada toon, normais/matrizes animadas e programas VU integrados |
| Auras e explosões | Pendentes: passes, dados e ligação ao motor |
| Hardware | Não executado em Xbox real ou emulador |

Não é possível apresentar este pacote como cumprimento integral do pedido.
Os recursos anteriores de corpo/facial continuam como diagnóstico em pose
fixa, sem loop de luta. Os resultados extensos da ISO nas versões anteriores
não foram transformados em alegação de validação dos novos estados na GPU.

## Reprodução

```sh
python3 engine/tools/test_gs_materials.py /caminho/xsdk /caminho/glslangValidator /tmp/shader
ASAN_OPTIONS=detect_leaks=0 python3 engine/tools/test_native_materials.py /caminho/xsdk /tmp/native --sanitize
ASAN_OPTIONS=detect_leaks=0 python3 engine/tools/test_gs_replay.py /caminho/upstream /caminho/xsdk /tmp/replay --sanitize
ASAN_OPTIONS=detect_leaks=0 python3 engine/tools/test_body_demo.py /caminho/xsdk /tmp/viewer --sanitize
make XSDK=/caminho/xsdk TOOLCHAIN=/caminho/xsdk-gcc/bin/powerpc-xsdk-elf-
python3 tests/check_xex.py
```

Para gerar os containers nativos novamente, use xsdk/tools/build_shaders.py
com engine/graphics/shaders/dr_gs.vert e dr_gs.frag; atualize tanto o header
em engine/graphics quanto sua cópia em src/. O teste de microcódigo requer
xenos-toolchain/xdis e libxsc.so construído no xsdk.

Relatórios desta execução: engine/reports/0.17/.
Upstream: 3ca419c7966b11438e2dc3c9a7e1914f102ed545.
xsdk: 57b02f1e60667e37d22c3aa613d23d2486514ff8.
Toolchain C/bootstrap GCC 15.3.0 com patches/specs xsdk e newlib instalada;
o estágio final C/POSIX não foi usado. Nenhuma ferramenta Microsoft.

XEX SHA-256: d74af7a183d3570bf019aeb6af984a7287662d1f5ace053f57fba5d899b2a85b

A distribuição contém fontes correspondentes, testes e licenças. Não contém
ISO nem recursos comerciais extraídos. A ISO permanece fornecida pelo usuário.
