# Diagnóstico 0.31

default.xex continua sendo um diagnóstico. Não executa Game_Main nem Battle_Main.
Não houve teste em console ou emulador nesta entrega.

Abra a captura gráfica original GS: sua inicialização agora chama Dma_InitBuffers
e envia o pacote u64 de boot pelo Dma_SendGif limitado. O log existente
`native Port_Gs ... DBuff clear probe rc=0` passa a incluir esse percurso.
Saia, abra novamente e preserve default.log e uma imagem do resultado.
rc=0 verifica o contrato, não confirma a correção dos pixels.

Ao escolher uma ISO USA, o diagnóstico de plataforma imprime também
`raw ISO open/read/seek/search USA ELF probe rc=0`. Ele abre o ELF do PS2,
confere os primeiros bytes e fecha; não executa esse código.
Em seguida continuam os probes ADXF e do registro de recursos.

Para um erro, registre a linha inteira e o contexto anterior/posterior.
Não avance para o boot original: a relocação dos dados e do overlay continua
pendente. Os testes de host foram feitos com transportes NT/GPU simulados;
não validam GPU real, luta completa nem desempenho.
