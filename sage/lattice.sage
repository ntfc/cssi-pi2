import hashlib

n = 512
q = 2**27
d = 1
k = 80
m = 8786
sigma = 31495
kp = 28

#TODO: ver cena da rejeiçao no genY tambem



def genS():
	return random_matrix(ZZ, m, k, x=-d, y=d)

def genMatrix():
  matx = random_matrix(ZZ, n, m, x=-int(((q-1)/2)), y=int((q-1)/2))
  return matx


def matrixMult(a, s):
  if(a.ncols() != s.nrows()):
    print 'error dimensions'
    return
  return (a*s).mod(q)

def matrixVectorMult(a, v):
	if(a.ncols() != len(v)):
		print 'error dimensions'
		return
	return (a*v.column()).mod(q)


def matrixAdd(a, s):
  if(a.dimensions() != s.dimensions()):
    print 'error dimensions'
    return
  return a+s


def genY():
	limite = int(sigma*log(n))
	T = RealDistribution('uniform', [-limite, limite])
	i = 0
	vec =vector(ZZ, m)
	while i < m:
		x = ZZ.random_element(-limite, limite, 'uniform')
		if int(random < T.distribution_function(x)):
			vec[i] = x
			i+=1
	return vec


#este lixo é deterministico por isso não dá para passar na condicao final
def hashFunction(v, u):
	normv = kp +1
	j = 0
	while normv > kp:
		s = ''
		#vector to string
		for i in v:
			s = s + str(i)
		s = s + u


		#resultado hash com 160bits
		sh = hashlib.sha1()
		sh.update(s)
		h = bin(int(sh.hexdigest(),16))[2:]
		h.zfill(160)
		
		c1 = h[ : len(h) // 2].zfill(80)
		c2 = h[len(h) // 2 : ].zfill(80)
		v1 = vector([int(c1[i]) for i in range(len(c1))])
		v2 = vector([int(c2[i]) for i in range(len(c2))])	
		vf = vector(v1-v2)
		vf1 = vf
		#norm v1
		#http://rorasa.wordpress.com/2012/05/13/l0-norm-l1-norm-l2-norm-l-infinity-norm/
		normv = sum(map(lambda x: abs(x), vf1))
		j+=1
		print 'tentativa hash '+j.str()
		print 'k '+kp.str()+'normv '+normv.str()
	return vf

#sem a cena da rejeicao
def hashFunction1(v, u):
	s = ''
	#vector to string
	for i in v:
		s = s + str(i)
	s = s + u
	#resultado hash com 160bits
	sh = hashlib.sha1()
	sh.update(s)
	h = bin(int(sh.hexdigest(),16))[2:]
	h.zfill(160)
	
	c1 = h[ : len(h) // 2].zfill(80)
	c2 = h[len(h) // 2 : ].zfill(80)
	v1 = vector([int(c1[i]) for i in range(len(c1))])
	v2 = vector([int(c2[i]) for i in range(len(c2))])	
	vf = vector(v1-v2)
	vf1 = vf
	#norm v1
	#http://rorasa.wordpress.com/2012/05/13/l0-norm-l1-norm-l2-norm-l-infinity-norm/
	normv = sum(map(lambda x: abs(x), vf1))
	print 'k '+kp.str()+'normv '+normv.str()
	return vf



#denominador e comum portanto pode ser cortado
def rejectionSampling(sc, z):
	pz = math.pow(1/(sigma * sqrt(2 * math.pi)), m) * math.exp(- ((norm(z))**2) / (2 * sigma**2))

	pvz = math.pow(1/(sigma * sqrt(2 * math.pi)), m) * math.exp(- ((norm(z - sc))**2) / (2 * sigma**2))

	return pz/(m*pvz)


def verify(u, z, c, a, t):
	c1 = hashFunction1(matrixMult(a, z)-matrixVectorMult(t,c), u)
	return ((norm(z) <= (n*sigma*sqrt(m))) and (c == c1))


def sign(message):
	rejectS = 0
	i = 0
	#fazer prints tentativas rejection sampling
	while rejectS < 1:
		a = genMatrix()
		s = genS()
		#print'chegou aqui'
		t = matrixMult(a, s)
		y = genY()
		#print'chegou aqui1'
		c = hashFunction(matrixVectorMult(a, y), message)
		
		#print'chegou aqui2'
		#print 's '+str(s.dimensions())
		#print 'c '+str(len(c))
		#print 'y '+str(len(y))
		#print 'y '+str((y.column()).dimensions())
		#print 'Sc '+ str(matrixVectorMult(s, c).dimensions())
		z = matrixAdd(matrixVectorMult(s, c), y.column())
		#print'chegou aqui3'
		rejectS = rejectionSampling(matrixVectorMult(s, c), z)
		i+=1
		print 'tentativa sign '+i.str()
	return (z, c)

def sign1(message):
	rejectS = 0

	#fazer prints tentativas rejection sampling

	a = genMatrix()
	s = genS()
	#print'chegou aqui'
	t = matrixMult(a, s)
	y = genY()
	#print'chegou aqui1'
	c = hashFunction1(matrixVectorMult(a, y), message)
		
	#print'chegou aqui2'
	#print 's '+str(s.dimensions())
	#print 'c '+str(len(c))
	#print 'y '+str(len(y))
	#print 'y '+str((y.column()).dimensions())
	#print 'Sc '+ str(matrixVectorMult(s, c).dimensions())
	z = matrixAdd(matrixVectorMult(s, c), y.column())
	#print'chegou aqui3'
	#rejectS = rejectionSampling(matrixVectorMult(s, c), z)

	return (z, c, a, t, messagemv)





















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
