n = 512
q = 2**27
d = 1
k = 80
m = 8786
sigma = 31495
T = RealDistribution('gaussian', sigma)

def genY():
  return vector([T.get_random_element().integer_part() for _ in range(m)])
  
def vector_distribution_function(y):
  if len(y) != m:
    print "ERROR: len(y) must be m!"
    return
  return math.pow(1/(sigma * sqrt(2 * math.pi)), m)# * math.exp(- ((y.norm(2))**2 / (2 * sigma**2)))
  
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
#def genVectorY123(sigma, m):
# dist = RealDistribution('gaussian', sigma)
# randvector = random_vector(ZZ, m)
# sump = sum(map(lambda x : dist.distribution_function(x), randvector))
# randvector = randvector.apply_map(lambda x: dist.get_random_element()/sump)
# return randvector.transpose()



#Probabilidades e amostragem

#Todo: ver p^m??? Ver se a funçao de probabilidades bate certo

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
