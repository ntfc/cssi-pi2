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
    #return randomMatrix(self.m, self.k, self.d)
    
  def genA(self):
    bound = int(((self.q-1)/2))
    
    #matx = random_matrix(ZZ, self.n, self.m, x=-bound, y=bound)
    #return matx
    #A = random_matrix(ZZ, self.n, self.m - self.n, x=-bound, y=bound)
    # is is faster to generate random numbers between 0 and q, right?
    ncols = self.m - self.n
    nrows = self.n
    A = randomMatrix(nrows, ncols, self.q)
    I = matrix.identity(self.n)
    # return [A | I] in Hermite Normal Form
    return block_matrix(ZZ, [[A, I]], subdivide=False)
    
  def calcT(self, A, S):
    return (A*S).mod(self.q)

  def genY(self):
    limite = Integer(self.sigma * log(self.n,2))
    T = RealDistribution('gaussian', self.sigma)
    i = 0
    vec = vector(ZZ, self.m)
    while i < self.m:
      x = ZZ.random_element(-limite, limite, 'uniform')
      #x = ZZ.random_element(limite*2, distribution='uniform')
      #x -= limite
      if random() < self.__p(0, x):
        vec[i] = x
        i+=1
    return vec
    
  def genC(self, A, y, mu):
    if type(mu) != str:
      print "Message must be a string!"
      return

    s1 = (A*y).Mod(self.q)
    s2 = self.hash_mu(mu)
    s = ''.join(map(lambda x : str(x), s1.list()))
    s += s2
    #resultado hash com 160bits
    #h = self.H(s)
    h = self.H2(s)
    print "||v||_1 = {0} (should be <= {1})".format(h.norm(p=1), self.kappa)
    return h

  def calcZ(self, S, c, y):
    return (S*c) + y
    
  def hash_mu(self, mu):
    sha1 = hashlib.sha1()
    mu = base64.b64encode(mu)
    sha1.update(mu)
    return bin(Integer(sha1.hexdigest(),16))[2:].zfill(160)
    
  def H(self, m):
    sha1 = hashlib.sha1()
    sha1.update(m)
    
    h = bin(Integer(sha1.hexdigest(),16))[2:].zfill(160)
    
    c1 = h[ : 80]
    c2 = h[80 : ]

    v1 = vector([int(ci) for ci in c1])
    v2 = vector([int(ci) for ci in c2])
    
    v = vector(v1 - v2)
    return v
  
  def H2(self, s):
    sha1 = hashlib.sha1()
    sha1.update(s)
    h = bin(Integer(sha1.hexdigest(),16))[2:].zfill(160)
    
    h2 = vector(ZZ, 0)
    
    for i in xrange(0, 16): # divide h in 10 bit strings
      start = i * 4
      bits = h[start : start + 4]
      pos = int(bits[1:], 2)
      vi = vector(ZZ, 10) # 10 bits
      if bits[0] == '0':
        #print "put -1 in {0}".format(pos)
        vi[pos] = -1
      if bits[0] == '1':
        #print "put 1 in {0}".format(pos)
        vi[pos] = 1
      h2 = vector(list(h2) + list(vi)) # ugly!
    
    return h2[80 : ]
    
  def Sign(self, mu, A, S):
    if type(mu) != str:
      print "Message must be a string!"
      return
    i = 1
    while True:
      print "Attempt nr {0}..".format(i)
      print "Generating y..."
      y = self.genY()
      print "Calculating Ay mod q..."
      s1 = (A*y).Mod(self.q)
      s2 = self.hash_mu(mu)
      print "Calculating H(Ay, mu)..."
      s = ''.join(map(lambda x : str(x), s1.list()))
      s += s2
      print "Calculating s...."
      c = self.H2(s)
      print "Calculating Sc + y...."
      z = S*c + y
      print "Rejection sampling..."
      if self.rejectionSampling() == True:
        print "Rejection sampling ok after {0} tries. Returning (z,c)..".format(i)
        return (z, c)
      else:
        print "Rejection sampling failed. Trying again..."
      i += 1
    
  def Vrfy(self, mu, z, c, A, T):
    if type(mu) != str:
      print "Message must be a string!"
      return
    z_norm = float(z.norm(p=2))
    #mu = base64.b64encode(mu)
    print "Calculating Az - Tc mod q...."
    s1 = (A*z - T*c).Mod(self.q)
    s2 = self.hash_mu(mu)
    print "Converting Az - Tc to str..."
    s = ''.join(map(lambda x : str(x), s1.list()))
    #sha1 = hashlib.sha1()
    #sha1.update(mu)
    #mu = bin(Integer(sha1.hexdigest(),16))[2:].zfill(160)
    s += s2
    #resultado hash com 160bits
    #h = self.H(s)
    print "Calculating H(Az - Tc, mu)...."
    h = self.H2(s)
    z_to_compare = float(self.eta*self.sigma*float(sqrt(self.m)))
    return z_norm < z_to_compare and c == h
  
  #denominador e comum portanto pode ser cortado
  def rejectionSampling(self, S, z, c):
    print "Calculating S*c..."
    Sc = S*c
    
    """pz = math.pow(1/(self.sigma * sqrt(2 * math.pi)), self.m) * math.exp(- ((z.norm(p=2))**2) / (2 * self.sigma**2))
    
    print pz
    return
    pvz = math.pow(1/(self.sigma * sqrt(2 * math.pi)), self.m) * math.exp(- ((z - Sc).norm(p=2)**2) / (2 * self.sigma**2))

    return float(pz/(self.m*pvz))"""
    return int(random() < float(1 / self.M))
  
  # gaussian rejection for integers
  def __p(self, c, x):
    return exp((float(-((x-c)**2) / (2*self.sigma**2))))
  
  # return D^m_sigma(x)
  #def D(self, v, x):
    # TODO: acabar isto
    
def randomMatrix(nrows, ncols, bound):
  A = matrix(nrows, ncols)
  for row in xrange(0, nrows):
    new_row = []
    for col in xrange(0, ncols):
      aij = ZZ.random_element(bound)
      new_row.append(aij - bound)
    A[row] = new_row
  return A
  

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
  pz = math.pow(1/(sigma * sqrt(2 * math.pi)), m) * math.exp(- ((z.norm(p=2))**2) / (2 * sigma**2))

  pvz = math.pow(1/(sigma * sqrt(2 * math.pi)), m) * math.exp(- ((z - sc).norm(p=2)**2) / (2 * sigma**2))

  return pz/(m*pvz)



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
