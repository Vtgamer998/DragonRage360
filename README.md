# DragonRage360

Port **experimental e não oficial** de *Dragon Ball Z: Budokai Tenkaichi 3* (versão USA, PS2) para **Xbox 360**, escrito para rodar de forma **nativa** (não é um emulador).

> **Estado atual (0.44-D10): ainda não existe luta jogável.**
> O executável é um **diagnóstico**: ele lê a sua ISO, converte dados e inicializa parte do motor de combate, mas `Battle_Update`/`Battle_Main` ainda não rodam e não há renderização de luta na GPU.
> Nenhuma versão depois da 0.33 foi confirmada em um Xbox 360 real. Os testes mais recentes foram feitos em CPU i386 (QEMU).

## Aviso legal

- Este é um projeto de fãs, **sem afiliação** com os detentores dos direitos de *Dragon Ball Z* ou do jogo original.
- **Este repositório não contém a ISO, o executável MIPS do jogo, modelos, texturas, áudio nem qualquer dado extraído do jogo.** Você precisa ter a sua própria cópia legal da versão USA.
- Não peça nem compartilhe ISOs ou dados extraídos nas issues.
- Não publique binários (`.xex`) compilados por você a partir de dados do jogo.

## Como funciona

| Parte | Origem |
|---|---|
| Lógica do jogo (luta, IA, objetos) | Código C do projeto de decompilação Tenkaichi3DecompAndroid (de z3xox e colaboradores), compilado para PowerPC |
| Plataforma (GS, VU, áudio, disco, memória) | Escrita neste projeto para o Xbox 360 |
| Conversão de dados (MIPS little-endian → PowerPC big-endian) | Escrita neste projeto, campo a campo |
| Dados do jogo (modelos, animações, cenário, tabelas) | Lidos da **sua** ISO em tempo de execução |
| Launcher/interface | Próprio (MIT) |

Upstream fixado em `3ca419c7966b11438e2dc3c9a7e1914f102ed545`; SDK `xsdk` em `57b02f1e60667e37d22c3aa613d23d2486514ff8` (veja `BUILD.md`).

## O que já foi comprovado

- Leitura da ISO USA, de AFS/BIN e do executável do PS2 (**no Xbox 360 real, versão 0.33**).
- Decodificação de animações e modelos, colisão do cenário, tabelas comuns e IA convertidas (testes em PC).
- `BtlObj_Init/Term`, `BtlChar_AllocAll` e `BtlAiMgr_Init` originais executando com dados reais (testes i386/QEMU).
- 405/405 unidades compilam para PowerPC, sem símbolos indefinidos.

## O que falta

- Executar `Battle_Update`/`Battle_Main` e o ciclo completo de `Battle_Load`/`Battle_Init`.
- Cenário completo (continua `NOT_READY`), HUD, controle, simulação e apresentação.
- Renderização na GPU do Xbox 360 (skinning, materiais, facial, efeitos) e vozes/áudio.
- Menus originais e `Game_Main` (69 dependências sem solução).
- Testes em Xbox 360 real das versões recentes.

Veja `PENDENCIAS.md` e o relatório mais recente (`RELATORIO-0.44-D10.md`).

## Como compilar

Veja [`BUILD.md`](BUILD.md). Resumo:

```sh
make typed-data-diagnostic UPSTREAM=/caminho/upstream \
  XSDK=/caminho/xsdk TOOLCHAIN=/caminho/toolchain/bin/powerpc-xsdk-elf- \
  ISO=/caminho/jogo-USA.iso
```

O resultado fica em `build/typed-diagnostic/default.xex`.

## Como testar no Xbox 360

1. Copie o `default.xex` e o `startup_test.txt` para uma pasta do console.
2. Coloque a sua ISO USA em `isos/` (USB ou HD).
3. Inicie o `default.xex` (por exemplo, pelo Aurora) com `startup_test.txt` em `typed-data`.
4. Saia pelo menu e guarde o `default.log`.

Instruções detalhadas em [`TESTAR-PRIMEIRO.md`](TESTAR-PRIMEIRO.md). Se der erro, abra uma issue com o `default.log` e uma foto da tela.

## Licença

**AGPL-3.0**, com partes sob MIT e outras licenças de terceiros (xsdk, newlib, libgcc, fonte bitmap). Detalhes em [`LICENSE`](LICENSE) e [`LICENSES.md`](LICENSES.md). Como o projeto deriva do upstream em AGPL, o código-fonte precisa acompanhar qualquer distribuição de binários.

## Créditos

- Tenkaichi3DecompAndroid, de z3xox, dfdx047 e colaboradores.
- xsdk, de Mena e colaboradores.
- Dragon Rage UI e demais autores listados em `licenses/`.
