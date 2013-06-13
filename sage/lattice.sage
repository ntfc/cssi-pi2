

def genMatrix(n,m,q):
	matx = random_matrix(ZZ, n, m, x=0, y=q)
	return matx


def matrixMult(a, s, q):
	if(a.ncols() != s.nrows()):
		print 'error dimensions'
		return
	return (a*s).mod(q)


def matrixAdd(a, s):
	if(a.dimensions() != s.dimensions()):
		print 'error dimensions'
		return
	return a+s

#isto vai ser para apagar, nao era assim
def genVectorY(sigma, m):
	dist = RealDistribution('gaussian', sigma)
	randvector = random_vector(ZZ, m)
	sump = sum(map(lambda x : dist.distribution_function(x), randvector))
	randvector = randvector.apply_map(lambda x: dist.get_random_element()/sump)
	return randvector.transpose()


#Knuth-Yao algotithm for distribution


#isto é para fazer sobre q ou m??!!!!!

#somatorio de p(Zm)
def sumPZ(sigma, m):
	dist = RealDistribution('gaussian', sigma)
	#list com todos os valores de m em ZZ
	v = list(range(m))
	#sum da distribuiçao gaussiana sobre a lista ZZm
	sump = sum(map(lambda x : dist.distribution_function(x), v))
	return sump


#gerar lista de probabilidades em binario
def genListProb(sigma, m):
	sump = sumPZ(sigma, m)
	dist = RealDistribution('gaussian', sigma)
	v = list(range(m))
	#vector com a table de probabilidades para valores de ZZm
	probList = map(lambda x : dist.distribution_function(x)/sump, v)
	#convert to binary
	binaryList = map(lambda x : RealNumber(x).str(2), probList)
	return binaryList

#criar tabela YAO
def genYAOTable(sigma, m):
	binaryList = genListProb(sigma, m)
	
	#binaryList[i][x] ; i = indice da lista de probabilidades; x = caracter da probabilidade
	tableYAO = matrix(ZZ, len(binaryList)+1, len(binaryList[0])-2)
	#funçao que vai fazer map na tabela para por tudo a -1
	f(x) = -1
	tableYAO.apply_map(f)

	#ciclo que vai meter os valores na tabela
	#quando encontrar -1 vai para coluna seguinte




	return tableYAO	












	




