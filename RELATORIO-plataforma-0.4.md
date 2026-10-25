# Plataforma 0.4 integrada — combate ainda bloqueado

O pacote enviado foi incorporado e seus 12 módulos C compilaram para PowerPC
com o SDK real. Há um novo `default.xex` de diagnóstico com o gancho de
recursos instalado antes das leituras. **Battle_Main continua fora do XEX.**

| Verificação | Resultado |
| --- | --- |
| Tentativa de vínculo original | 380/380 unidades compiladas; vínculo falhou |
| Objetos rejeitados por ABI float | 221 antes; **0 agora** |
| Nomes de símbolos pendentes | 187 antes; **163 agora** |
| Objetos com aritmética adaptada inspecionados | 293; nenhuma instrução aritmética FP nativa encontrada |
| Comparações do adaptador numérico no host | 1.300.187 passaram contra os helpers originais |
| Leitura, conversores e BPE no host | ASan/UBSan passaram; LeakSanitizer desativado por limitação de /proc |
| Registro de IDs sem conversor | 701 IDs distintos, duas leituras cada, uma linha por ID |
| Empacotamento do XEX | ELF/PE PowerPC, símbolos, SHA-1 e cadeia de páginas passaram |
| Xbox, Xenia, pixels, combate e desempenho | Não executados / não medidos |

## O que mudou na ABI

`engine/combat/ps2_float_pass.cc` é um passe de compilação explícito que
mantém assinaturas C e chamadas hard-float do Xbox. Antes das otimizações,
encaminha operações float para os helpers de padrões de bits do PS2.
As operações inteiras dos helpers continuam inteiras. Os nomes `dr_ps2_*`
evitam substituir funções internas do libgcc que têm outro contrato.

`math_bridge.c` fornece entradas com tipos corretos para libm e operações
double IEEE, seguindo a divisão de responsabilidades do upstream. A opção
`legacy-soft` continua disponível para reproduzir o impedimento anterior.
Não foram removidos atributos ELF nem desativadas verificações do linker.
O teste de controle confirma que misturar soft/hard ainda falha, enquanto
objetos com a mesma ABI ligam. Os testes numéricos foram executados no host;
não comprovam identidade de bits em PowerPC, determinismo da luta ou rollback.
Transferências por registradores FP, casos especiais e o motor inteiro ainda
precisam de validação no destino. As próprias referências upstream não são
uma validação do hardware PS2.

## O que mudou na plataforma e nos recursos

- XInput usa `INPUT_STATE` e `INPUT_VIBRATION` oficiais do SDK; a espera usa
  `KeDelayExecutionThread`. A propriedade dos blocos grandes é registrada
  separadamente dos seus endereços de jogo.
- `XbRes_Install()` é chamado no início do título, por `XbPlatform_Install()`.
  `ADXF_ReadNw` entrega partição, índice, deslocamento, tamanho real e bytes
  válidos ao gancho. O preenchimento do último setor não vira tamanho do recurso.
- Leituras parciais ficam registradas e não são convertidas. Erros de um
  conversor com retorno verificado causam `STAT_ERROR` na leitura.
- O caminho big-endian de `src/sys/bpe.c` está em `engine/combat/bpe.c`:
  usa cabeçalho LE, o descompressor limitado do demo e `XbRes_AfterBpe` somente
  depois da expansão. Buffers sem limites conhecidos são recusados. Há
  ganchos de descarte no heap para não manter metadados de memória liberada.
- Os adaptadores de `dr_anim_prepare` e `dr_skeleton_prepare` aceitam apenas
  arquivos inteiros de formato confirmado. O arquivo opcional
  `resource-converters.txt` registra faixas explicitamente. Nenhuma faixa
  vem habilitada por palpite. Recursos embutidos exigem conversor do contêiner.
- O registro de arquivos sem conversor cresce além dos 512 IDs do pacote
  recebido. `SwapPlan` rejeita estouros no tamanho do plano.

O patch integrado de BPE substitui o patch inicial do anexo: não aplique os
dois juntos. Os patches integrados ficam em `engine/platform/patches/`.
O builder usa as cópias adaptadas de bpe.c e heap.c, sem alterar o checkout
externo do upstream.

## O que está no XEX e o que só compilou

O XEX liga a leitura ADXF/ISO, registro/mapa/adaptadores de recursos e o
diagnóstico anterior de renderização. Na opção de verificar a ISO, também
lê o primeiro setor do primeiro arquivo de cada AFS pelo caminho ADXF e
registra o resumo no `default.log`. Isso **não é um inventário completo**.

Os módulos de memória, controles e temporização foram compilados, mas sua
execução dentro do combate não foi validada. O Bpe_Decode original adaptado
participa da tentativa de vínculo do motor, não do launcher: seu gancho
pós-expansão ainda não foi executado no console. O launcher não reserva os
endereços fixos PS2 nem inicia o heap do motor apenas para mostrar o menu.

O pacote recebido contém respostas de compatibilidade para chamadas de
kernel/CD/IOP, algumas constantes ou sem operação. Elas não equivalem à
emulação desses serviços. Não foram gerados stubs novos para satisfazer
símbolos restantes; a queda da contagem não prova implementação funcional
de todos esses serviços. Áudio ADXT e RPC, por exemplo, continuam pendentes.

## Impedimentos restantes

Os 163 nomes incluem ADXT, produtores DMA não adaptados, GsVu1, cópia de
buffers GS, RPC, extensões de interface/rede, globais e referências de
bibliotecas PS2 em tabelas. A lista exata e a diferença de 24 nomes ficam
em `engine/reports/platform-0.4/`.

As estruturas de tabelas compiladas continuam sem valores do jogo. Não
foi gerado nem validado um `.dat` completo a partir de um ELF de combate;
o carregador BT3X e a ferramenta de strip ainda precisam dessa validação.
Não foram convertidos todos os modelos, texturas, colisões e cenários.

O backend GS nativo experimental anterior continua sendo o caminho gráfico
escolhido. Ainda precisa dos produtores reais/VU e da simulação conectados.
Não há teste de DATE/FBA/stencil no hardware, luta jogável, IA integrada ou
online de combate. A coleta completa dos IDs só será possível quando o
motor puder inicializar e ler os recursos reais.

## Uso e reprodução

No Xbox 360 com suporte a homebrew, extraia a pasta do projeto, coloque sua
ISO em `isos/` e abra o `default.xex` pelo Aurora. Verifique a ISO pelo menu
e recolha `default.log`. Nenhuma execução no console foi feita nesta entrega.

Para configurar conversores, leia `resource-converters.EXEMPLO.txt`. Não
classifique um modelo inteiro como animação ou esqueleto para eliminar logs.

Compile o título com o Makefile e o SDK fixado no BUILD.md. Para o motor:

```sh
make battle-link UPSTREAM=/caminho/upstream XSDK=/caminho/xsdk \
  TOOLCHAIN=/caminho/xsdk-gcc/bin/powerpc-xsdk-elf-
```

O alvo ainda retorna falha nas 163 dependências. Requer compilador C++ do
host, cabeçalhos de plugins do mesmo GCC e cabeçalhos de GMP/MPFR/MPC.
Se estiverem em caminho separado, passe `PLUGIN_ARGS` com um ou mais
`--plugin-include /caminho`. Ele não substitui o XEX por um ELF incompleto.

```sh
python3 engine/tools/test_platform.py /caminho/xsdk build/platform-tests --sanitize
python3 engine/tools/test_float_abi.py /caminho/upstream build/float-tests
python3 tests/check_xex.py
```

O teste float usa os cabeçalhos de plugin correspondentes ao compilador do
host. Veja `--cc`, `--host-cxx` e `--plugin-include` quando necessário.
Evidências: `engine/reports/platform-0.4/` e `VALIDACAO.json`.
O pacote não contém ISO nem recursos extraídos do jogo.
