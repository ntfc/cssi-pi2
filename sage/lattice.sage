import hashlib
import base64

class LatticeSignature:
  def __init__(self):
    self.n = 512
    self.q = 2**27
    self.d = 1
    self.k = 80
    self.eta = 1.1
    self.m = 8786
    self.kappa = 28
    self.sigma = 31495
    self.M = 2.72
  
  def genS(self):
    return random_matrix(ZZ, self.m, self.k, x=-self.d, y=self.d)
    
  def genA(self):
    bound = int(((self.q-1)/2))
    
    #matx = random_matrix(ZZ, self.n, self.m, x=-bound, y=bound)
    #return matx
    A = random_matrix(ZZ, self.n, self.m - self.n, x=-bound, y=bound)
    I = matrix.identity(self.n)
    # return [A | I] in Hermite Normal Form
    return block_matrix(ZZ, [[A, I]], subdivide=False)
    
  def calcT(self, A, S):
    return (A*S).mod(self.q)
    
  # gaussian rejection for integers
  def __p(self, c, x):
    return exp((float(-((x-c)**2) / (2*self.sigma**2))))

  def genY(self):
    limite = Integer(self.sigma * log(self.n,2))
    T = RealDistribution('gaussian', self.sigma)
    i = 0
    vec = vector(ZZ, self.m)
    while i < self.m:
      x = ZZ.random_element(-limite, limite, 'uniform')
      if random() < self.__p(0, x):
        vec[i] = x
        i+=1
    return vec
    
  """
  TODO: mu must be the message digest
  """
  def genC(self, A, y, mu):
    if type(mu) != str:
      print "Message must be a string!"
      return
    # TODO: unicode troubles..
    mu = base64.b64encode(mu)
    s1 = (A*y).Mod(self.q)
    s = ''.join(map(lambda x : str(x), s1.list()))
    s += mu
    
    #resultado hash com 160bits
    h = self.H(s)
    
    return h

  def calcZ(self, S, c, y):
    return (S*c) + y
    
  def H(self, m):
    H = hashlib.sha1()
    H.update(m)
    
    h = bin(Integer(H.hexdigest(),16))[2:].zfill(160)
    
    c1 = h[ : 80]
    c2 = h[80 : ]

    v1 = vector([int(ci) for ci in c1])
    v2 = vector([int(ci) for ci in c2])
    
    v = vector(v1 - v2)
    norm1 = v.norm(p=1)
    print "||v||_1 = {0} (must be <= {1}".format(v.norm(p=1), self.kappa)
    return v
  
"""def matrixMult(a, s):
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
"""

#denominador e comum portanto pode ser cortado
def rejectionSampling(sc, z):
  pz = math.pow(1/(sigma * sqrt(2 * math.pi)), m) * math.exp(- ((norm(z))**2) / (2 * sigma**2))

  pvz = math.pow(1/(sigma * sqrt(2 * math.pi)), m) * math.exp(- ((norm(z - sc))**2) / (2 * sigma**2))

  return pz/(m*pvz)

#esta a receber tudo para ser mais facil. Depois mudar isto
def verify((z, c, a, t, message)):
  c1 = hashFunction1(matrixMult(a, z)-matrixVectorMult(t,c), message)
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


#Assinatura sem rejections
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

  return (z, c, a, t, message)
