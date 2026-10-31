> Relatorio historico do sampler: a descricao de default.xex/HW1 e as contagens foram substituidas por RELATORIO-0.34-L1.md. O sampler nao mudou nesta etapa.

# Sampler nativo e RPC de efeitos — 24/10/2026

Os dois símbolos receberam implementações com operações reais nos fontes
experimentais: func_002967C0 e sceSifCallRpc. Não foi criado um retorno de
sucesso vazio. A luta ainda não funciona e nenhum áudio novo foi ouvido no
Xbox. O default.xex incluído continua sendo o diagnóstico gráfico 0.33-HW1.

## Implementação

- func_002967C0: transferência síncrona IOP -> memória SPU de 2 MiB; consulta
  de conclusão só confirma uma transferência válida. A origem deve pertencer
  a uma alocação registrada por xb_iop; origem/tamanho/destino são conferidos.
- sceSifCallRpc: cliente nativo registrado no ID de áudio 0x2000004; INIT,
  TERM, RESET, SET/FREE_BANK, mono, pausa, volume, tick, parada de banco e filas
  de até 32 PLAY/STOP/STOP_HANDLE. Tom entre -1200 e +1200 cents. RPC síncrono,
  que corresponde às chamadas do jogo. Resposta de erro gravada em reply[0],
  inclusive quando a fila é recusada; entradas são copiadas antes da resposta
  para permitir buffers sobrepostos. Callbacks/modo assíncrono são recusados.
- Sampler: 48 vozes VAG ADPCM, filtros 0..4, interpolação linear, pan, loops
  pelos flags VAG, parada com fade de 8 ms e ganho compatível com o port PC.
  Registros de banco são copiados e validados; não ficam apontando para IOP
  liberada. Palavras RPC usam o endian nativo; cabeçalhos de banco usam LE.
- Carregamento adiado: os metadados podem chegar antes das amostras, como em
  Snd_LoadBank(..., 0). PLAY é recusado até uma transferência cobrir o banco.
  Sobrescrever a região interrompe vozes antigas e atualiza seus limites.
- ADXT e efeitos passam pelo mesmo mixer e cliente XSDK, antes do clipping
  final. A inicialização é compartilhada: Game_Main chama Snd_Init antes de
  File_Init/ADXT_Init; essa segunda inicialização preserva o sampler aberto.
  O mesmo gancho de vblank alimenta a saída, inclusive sem música tocando.

Os comandos sem semântica implementada 0x8/0xB/0xC/0xE retornam erro explícito;
não há chamadores de jogo deles além dos wrappers não usados no upstream
fixado. FIGHTERS registra os IDs; não implementa vozes alternativas online.
Erros têm códigos -710 (argumentos/estado), -711 (limites), -712 (formato),
-713 (operação não suportada) e -714 (memória). Falhas ADPCM aparecem também
no TICK até RESET e no log; não são transformadas em reprodução bem-sucedida.

## Validação feita

O teste host lê a ISO real pelo xb_disc/dr_disc e adaptador de arquivos, com
somente o transporte NT e a saída física de áudio simulados. O banco é
pzs3us1/03194.bin (AFS1 índice 3194); as amostras comerciais não acompanham
este pacote.

ASan/UBSan passaram. Foram comparados 6144 frames estéreo de PCM em três
valores de tom (-1200, 0, +1200), com tolerância inferior a 1/32768, usando
o decode_block extraído do upstream fixado como referência. Também passaram
pausa/resume, volume, pan, mono, stop/fade, fila inválida sem execução parcial,
limites IOP/SPU, vida útil das cópias, transferência adiada/substituição,
erro persistente de ADPCM inválido, ordem Snd_Init -> ADXT_Init e mistura
ADX real + efeitos por um único cliente SDK. A comparação cobre uma amostra
desse banco, não todos os efeitos ou todos os bancos do jogo.

O teste ADXT anterior passou novamente. A compilação estrita compila 396/396
unidades PowerPC, zero conflitos ABI. O vínculo com Battle_Main forçado ainda
falha em 103 nomes de dados, sem nomes classificados com referência em código
nessa auditoria. Nenhuma tabela foi apagada nem erro do linker ignorado.
A classificação inclui seções depois descartadas por GC; não prova alcance
em execução. Os 103 nomes não são 103 recursos da ISO.

## Limites que continuam

Isto é um sampler nativo baseado no port público, não emulação completa da
SPU2: envelope/release exato, interpolação do hardware e reverb não estão
reproduzidos. O balanço efeitos/música ainda precisa de comparação auditiva
no console. Não foram medidas 48 vozes, latência, consumo CPU ou underruns.
Não houve execução PowerPC/Xbox/emulador deste código nem validação da saída
XSDK de áudio real. Os testes host não substituem essa etapa.

Continuam faltando o manifesto tipado dos dados do programa, o boot nativo,
a conversão/integração de todos os recursos usados pela luta e os controles,
animações, IA, colisão e renderização no combate. Os 103 símbolos de tabelas
ainda impedem o link. Battle_Main e Game_Main não estão no XEX distribuído.

## Fontes e reprodução

Os fontes novos e os testes estão no patch cumulativo
extras/fixed-battle-min-INCOMPLETO.patch, relativo à base 0.33 original.
Substitui o patch cumulativo anterior. Não aplicar diretamente sobre a
variante HW1 sem rebase: o launcher da raiz e a variante experimental são
árvores diferentes. checkpoint-audio/fontes contém também cópias dos arquivos
de áudio alterados para inspeção, sem constituir um projeto separado.

Após aplicar o patch à base 0.33, com o upstream fixado e SDK:

```
python3 engine/tools/test_sfx.py UPSTREAM XSDK ISO OUT --sanitize
python3 engine/tools/test_adxt.py UPSTREAM XSDK ISO OUT_ADXT --sanitize
python3 engine/tools/link_battle.py UPSTREAM CC XSDK OUT_LINK --float-mode native-ps2 --platform --fixed-battle
python3 engine/tools/audit_engine_refs.py OUT_LINK AUDIT_OUT
```

O comando de vínculo deve falhar nos 103 símbolos; não produz um XEX de luta.
Consulte os JSON/logs em checkpoint-audio e checkpoint-luta-minima para os
resultados. Os relatórios históricos permanecem no pacote, mas este relatório
substitui suas contagens e a afirmação anterior de sampler/RPC ausentes.
