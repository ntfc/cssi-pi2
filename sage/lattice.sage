import hashlib
import hmac

#n = 512
#q = 2**27
#d = 1
#k = 80
#m = 8786
#sigma = 31495
#T = RealDistribution('gaussian', sigma)


def genS(d, m, k):
	return random_matrix(ZZ, m, k, x=-d, y=d)

def genMatrix(n,m,q):
  matx = random_matrix(ZZ, n, m, x=-int(((q-1)/2)), y=int((q-1)/2))
  return matx


def matrixMult(a, s, q):
  if(a.ncols() != s.nrows()):
    print 'error dimensions'
    return
  return (a*s).mod(q)

def matrixVectorMult(a, v, q):
	if(a.ncols() != len(v)):
		print 'error dimensions'
		return
	return (a*v.column()).mod(q)


def matrixAdd(a, s):
  if(a.dimensions() != s.dimensions()):
    print 'error dimensions'
    return
  return a+s

def genY(sigma, n, m):
	limite = int(sigma*log(n))
	return vector([ZZ.random_element(-limite, limite, 'uniform') for _ in range(m)])


#este lixo é deterministico por isso não dá para passar na condicao final
#arranjar outra solucao
def hashFunction(v, u, k):
	normv = k +1
	j = 0
	while normv > k:
		s = ''
		#vector to string
		for i in v:
			s = s + str(i)	
		hmc = hmac.new(s, u, digestmod=hashlib.sha1)
		#resultado hash com 160bits
		h = bin(int(hmc.hexdigest(),16))[2:]
		c1 = h[ : len(h) // 2]
		c2 = h[len(h) // 2 : ]
		v1 = vector([int(c1[i]) for i in range(len(c1))])
		v2 = vector([int(c2[i]) for i in range(len(c2))])	
		vf = v1-v2
		vf1 = vf
		#norm v1
		#http://rorasa.wordpress.com/2012/05/13/l0-norm-l1-norm-l2-norm-l-infinity-norm/
		normv = sum(map(lambda x: abs(x), vf1))
		j+=1
		print 'tentativa hash '+j.str()
		print 'k '+k.str()+'normv '+normv.str()
	return vf


#denominador e comum portanto pode ser cortado
def rejectionSampling(sc, z, m):
	pz = math.pow(1/(sigma * sqrt(2 * math.pi)), m) * math.exp(- ((norm(z))**2) / (2 * sigma**2))

	pvz = math.pow(1/(sigma * sqrt(2 * math.pi)), m) * math.exp(- ((norm(z - v))**2) / (2 * sigma**2))

	return pz/(m*pvz)


def verify(n, sigma, m, u, z, c, a, t):
	c1 = hashFunction(matrixVectorMult(a, z)-matrixVectorMult(t,c), u)
	return ((norm(z) <= (n*sigma*sqrt(m))) and (c == c1))


def sign(message, sigma, n, m, q, k, kp, d):
	rejectS = 0
	i = 0
	#fazer prints tentativas rejection sampling
	while rejectS < 1:
		a = genMatrix(n, m, q)
		#falta definir o S
		s = genS(d, m, k)
		t = matrixMult(a, s, q)
		y = genY(sigma, n, m)
		c = hashFunction(matrixVectorMult(a, y, q), message, kp)
		
		print'chegou aqui'
		z = matrixAdd(matrixVectorMult(s, c, q), y)
		print'chegou aqui2'
		rejectS = rejectionSampling(matrixMult(s, c, q), z, m)
		i+=1
		print 'tentativa sign '+i.str()
	return (z, c)








######################################
#teddy fez isto
#analisar depois
#def genY():
#  return vector([T.get_random_element().integer_part() for _ in range(m)])
  
#def vector_distribution_function(y):
#  if len(y) != m:
#    print "ERROR: len(y) must be m!"
#    return
#  return math.pow(1/(sigma * sqrt(2 * math.pi)), m)# * math.exp(- ((y.norm(2))**2 / (2 * sigma**2)))
	
######################################
#Probabilidades e amostragem
#tudo mal isto

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
