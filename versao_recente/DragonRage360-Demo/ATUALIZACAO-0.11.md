# Atualização de fontes 0.11

**O port completo ainda não está concluído. Não há luta jogável ou novo XEX.**

Foram implementados targets nativos com persistência de cor/profundidade, troca
A→B→A e uma ponte que aceita listas GsDraw com vários IDs. O núcleo usa shaders
novos de restauração, compilados pelas ferramentas abertas do SDK.
Os testes CPU com chamadas de GPU simuladas e ASan/UBSan passaram.
Isso não valida rasterização, pixels ou desempenho no Xbox.

O caminho não restaura stencil e não inclui aux/DATE/FBA, efeitos GS com buffers
amostrados, readback/cópia para o contrato completo do GsBackend, registro do
backend, VU/skinning real, recursos da ISO ou loop de combate. Modos e online
continuam pendentes. O default.xex é o diagnóstico 0.9 anterior, byte a byte.

Documentação atual: engine/graphics/TARGETS.md (ciclo, testes e limites).
Relatórios novos: engine/reports/0.11/.
Os fontes em src/ correspondem ao diagnóstico 0.9; as mudanças novas estão
somente em engine/ e não foram compiladas para PowerPC nesta execução.


## Reproduzir a compilação dos shaders

```sh
make -C /caminho/xsdk build/host/libxsc.so -j8
python3 /caminho/xsdk/tools/build_shaders.py --glslang /caminho/glslangValidator --output engine/graphics/dr_restore_shaders.h engine/graphics/shaders/dr_restore.vert engine/graphics/shaders/dr_restore.frag
```

Os testes e o contrato de uso estão em engine/graphics/TARGETS.md.
O teste de ASan/UBSan deixou leak scan desativado pela restrição de /proc.
Ainda é necessário compilar os novos módulos com a ABI xsdk, criar um
executável de teste correspondente e validar as imagens produzidas no Xbox.
