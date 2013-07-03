
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

def genY(sgima, n):




def hashFunction(ay, u):
	return
	


def verify(n, sigma, m, u, z, c, a, t):
	c1 = hashFunction(matrixMult((a, z)-matrixMult(t,c), u))
	return ((norm(z) <= (n*sigma*sqrt(m))) and (c == c1))








#Probabilidades e amostragem


def probFunction(v, m, x):
	return	

#Todo: ver p^m??? Ver se a funçao de probabilidades bate certo
#Isto esta mal. Come me a ram toda para o valor de q

def genListProb(sigma, q):
	sump = sumPZ(sigma, q)
	dist = RealDistribution('gaussian', sigma)
	v = vector(ZZ, range(q))
	#vector com a table de probabilidades para valores de ZZm
	probList = map(lambda x : dist.distribution_function(x)/sump, v)
	return probList

#somatorio de p(Zm)
def sumPZ(sigma, q):
	dist = RealDistribution('gaussian', sigma)
	#list com todos os valores de m em ZZ
	vec = vector(ZZ, range(q))
	#sum da distribuiçao gaussiana sobre a lista ZZm
	sump = sum(map(lambda x : dist.distribution_function(x), vec))
	return sump

def genVectorY(sigma, q, m):
	listProb = genListProb(sigma, q)
	dist = GeneralDiscreteDistribution(listProb)
	vectorY = vector(ZZ, m)
	vectorY = map(lambda x: dist.get_random_element(), vectorY)
	return vectorY


