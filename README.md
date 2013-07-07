Lapin
========

##### SAGE
* load('lapin.sage')
* criar instancia `lapin = Lapin(reducible=True|False)`
* gerar chaves com `lapin.genKey()`
* criar challenge com `c = lapin.reader_step1()`
* criar mac com `(r,z) = lapin.tag_step2(c)`
* verificar mac com `lapin.reader_step3(c, r, z)`
* módulo `poly.py` contém operações sobre polinómios, mas nem todas as operações aritmétricas implementadas em sage são usadas na execução do protocolo.

##### C
* os seguintes defines podem ser usados na compilação (terao de ser manualmente definidos no Makefile):
 - DEBUG: é impressa diversa informação durante a execução do protocolo
 - PERFORMANCE: calcular o nr de clock cycles usados na execução do protocolo
 - URANDOM: (PARCIALMENTE IMPLEMENTADO) usar o gerador mais seguro urandom
* `make` cria o executavel `lapin`
* função main de `protocol.c` tem duas funções de teste: para o polinómio irredutível e para o polinómio redutível (verificação ainda não está completamente implementada)

Lattice Signatures
========
##### SAGE
* load('lattice.sage')
* `LatSign = LatticeSignature(variant=1|2|3)`
* gerar chaves individualmente, pois seria lento se fosse para gerar tudo de uma vez
 - chave privada `S = LatSign.genS()`
 - chave pública `A = LatSign.genA()`
 - chave verificação privada `T = LatSign.calcT(A, S)`
* assinar mensagem `m` com `(z,c) = LatSign.sign(m, A, S)`
* verificar com `LatSign.Vrfy(m, z, c, A, T)`
* NOTA: operações de geração de chaves e assinatura tendem a ser lentas, e por vezes toda a memória do PC é utilizada pelo Sage
