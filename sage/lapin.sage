class Lapin:
  """ Lapin superclass """
  def __init__(self, reducible=False):
    self.reducible = reducible
    if reducible == False:
      self.sec_param = 80
      self.tau = 1/8
      self.tau2 = 0.27
      self.n = 532
      self.protocol = Irreducible()
      (self.key1, self.key2) = self.protocol.genKey()
    elif reducible == True:
      self.security_param = 80
      self.tau = 1/6
      self.tau2 = 0.29
      self.n = 621
      #self.protocol = Reducible()
      #self.key =
    else:
      print "irreducible parameter must be either 0 or 1"
      
  # generate c
  def reader_step1(self):
    # generate binary string with len(c) = self.sec_param
    return bin(getrandbits(self.sec_param))[2:].zfill(self.sec_param)
    
  # generate (r,z)
  def tag_step2(self,c):
    if self.reducible == False:
      r = protocol.genR()
      e = protocol.genE(self.tau)
      pi = protocol.pimapping(c)
      z = r * (self.key1 * pi + self.key2) + e
      return (r,z)
    elif self.reducible == True:
      print "1"
    else:
      print "irreducible parameter must be either 0 or 1"
    
  # verification
  # TODO: create one class for Reader and another for Tag
  def reader_step3(self,c,r,z):
    if self.reducible == False:
      if r.gcd(self.protocol.f) != 1:
        print "reject R*"
        return False
      e2 = (z - r * (self.key1 * protocol.pimapping(c) + self.key2)).mod(self.protocol.f)
      if e2.hamming_weight() > (self.n * self.tau2):
        print "reject wt"
        return False
      print "accept"
      return True
    elif self.reducible == True:
      print "1"
    else:
      print "irreducible parameter must be either 0 or 1"
        
class Irreducible:
  """ Irreducible protocol """
  def __init__(self):
    (self.R, self.f) = self.__initRing()
    
  def __initRing(self):
    F = PolynomialRing(GF(2), 'x')
    x = F.gen()
    f = x^532 + x + 1
    R = F.quotient(f, 'x')
    self.x = F.gen()
    return (R, f)
  
  # return key (s, s_)
  def genKey(self):
    # TODO: return s,s' as sage.rings.polynomial.polynomial_gf2x.Polynomial_GF2X
    return (binToPoly(polyToBin(self.R.random_element(), self.x), self.x), binToPoly(polyToBin(self.R.random_element(), self.x), self.x))
    
  # returns the list of coefficients of the new polynomial v
  # c must be in binary format
  def pimapping(self, c):
    # DUVIDA: retornar list de coefs ou poly?
    v = 0*self.x
    for j in xrange(0, 16):
      # 32 = 5 bits
      cj = c[j*5 : (j*5) + 5]
      # here we use x instead of the (x-1) indicated in the paper
      # because we're in 0-index mode
      i = (16 * j) + binToInt(cj) # TODO: use binary operations here as well
      v += self.x**i
    return v
  
  # generate r
  def genR(self):
    # TODO: macumba para transformat R.random_element() em sage.rings.polynomial.polynomial_gf2x.Polynomial_GF2X
    r = binToPoly(polyToBin(self.R.random_element(), self.x), self.x)
    # while gcd(r, f) != 1, r not in R*
    # this wasnt supposed to be necessary, but sometimes gcd(r,f) = 5 ... :/
    while r.gcd(self.f) != 1:
      # TODO: macumba para transformat R.random_element() em sage.rings.polynomial.polynomial_gf2x.Polynomial_GF2X
      r = binToPoly(polyToBin(self.R.random_element(), self.x), self.x)
    return r

  # generate e
  def genE(self, tau):
    l = []
    e = 0*self.x
    for i in xrange(0, self.f.degree()):
      ci = bernoulli(tau)
      if ci == 1:
        e += self.x**i
    return e
    
    
""" Aux methods """
def bernoulli(tau):
  return int(random() < tau)
# p: poly, var=x, fill=length in binary
def polyToBin(p, var, fill=0):
  # TODO: validate types
  # TODO: var and p must be sage.rings.polynomial.polynomial_gf2x.Polynomial_GF2X
  l = p.list()
  # traverse in reversed order
  return (''.join(str(l[bit]) for bit in xrange(len(l)-1, -1, -1))).zfill(fill)
  
# convert a binary string to a polynomial
# [1,0,1,1,1] = x^4 + x^2 + x + 1
def binToPoly(b, var):
  # TODO: validate types
  # TODO: var must be sage.rings.polynomial.polynomial_gf2x.Polynomial_GF2X
  v = 0*var
  n = len(b)
  for i in xrange(0, n):
    if int(b[i]) == 1:
      v += var**(n - i - 1)
  return v

def intToBin(i, fill=0):
  return ''.join(bin(i)[2:]).zfill(fill)

# converts a binary to int
def binToInt(i):
  out = 0
  for bit in i:
    out = (out << 1) | int(bit)
  return out

# perform bitwise xor on two bins
# lists can have different length
def bitwiseXor(a, b):
  lenA = len(a)
  lenB = len(b)
  if len(a) >= len(b):
    diff = lenA - lenB
    c = a[:diff]
    c += ''.join(str(e) for e in map(lambda x,y : int(x).__xor__(int(y)), a[diff:], b))
  else:
    diff = lenB - lenA
    c = b[:diff]
    c += ''.join(str(e) for e in map(lambda x,y : int(x).__xor__(int(y)), a, b[diff:]))
  return c
  
"""# parameters for the irreducible case
lam = 80
tau = 1/8
tau2 = 0.27
n = 532

# constant function defined in the paper
def constantC(x, y):
  return ((x/y)^x) * ((1-x)/(1-y))^(1-x)

# bernoulli distribution. Returns a random bit
def Ber(tau=1/8):
  return int(random() < tau)


def initRing():
  # crete finite field F_2[a]
  F = PolynomialRing(GF(2), 'x')
  x = F.gen()
  # create the ring
  f = x^532 + x + 1
  R = F.quotient(f, 'x')
  x = F.gen()
  return R

def genkey(R):
  s = R.random_element()
  s_ = R.random_element()
  return (s, s_)


# returns the list of coefficients of the new polynomial v
def pimapping(R, c):
  l = c.bits()
  coefs = []
  for x in range(0, 16):
    ci = l[x*5:(x*5)+5]
    #print "{0} = {1}".format(ci, bitlistToInt(ci))
    # here we use x instead of the (x-1) indicated in the paper
    i = 16 * x + bitlistToInt(ci)
    #print "i = {0}".format(i)
    coefs.append(i)
  return coefs

# create the v in R polynomial
# l is a list of integers
def createPolyFromListInt(R, l):
  v = 0
  x = R.gen()
  for i in range(0, len(l)):
    v += x^l[i]
  return v

# convert integer to bitlist
def intToBitlist(n):
  return [int(bit) for bit in Integer(n).binary()]

# convert a bitlist to sage Integer
def bitlistToInt(l):
  out = 0
  for bit in l:
    out = (out << 1) | int(bit)
  return out

# pad n with size bits with value bit
# return as a bitlist
def padNumber(n, size, bit=0):
  l = intToBitlist(n)
  l.reverse()
  l.extend([int(bit)] * size)
  l.reverse()
  return l

# TODO: adicionar um wt para inteiros
# hamming weight of a polynomial
def wt(e):
  return e.list().count(1)

""
" The Protocol
""

# generate random c. Used by the Reader
def genC(n=80):
  c = Integer(getrandbits(n))
  return c

# generate r
def genR(R):
  #return R.multiplicative_generator()
  f = bitlistToInt(R.modulus().list())
  r = R.random_element()
  print "r = {0}".format(bitlistToInt(r.list()))
  print "gcd(r, f) = {0}".format(gcd(bitlistToInt(r.list()), f))
  # while gcd(r, f) != 1, r not in R*
  # this wasnt supposed to be necessary, but sometimes gcd(r,f) = 5 ... :/
  while gcd(bitlistToInt(r.list()), f) != 1:
    r = R.random_element()
  return r


# generate e
def genE(R):
  l = []
  e = 0
  x = R.polynomial_ring().gen()
  for i in range(0, n):
    ci = Ber()
    if ci == 1:
      e += x^i
  return e

# executed by the tag
def calcZ(c, s, s_, R):
  r = genR(R)
  e = genE(R)
  pi = createPolyFromListInt(R, pimapping(R,c))
  z = r * (s * pi + s_) + e
  return (r, z)

# executed by the tag
def calcE_(c, s, s_, r, z, R):
  pi = createPolyFromListInt(R, pimapping(R,c))
  return z - r * (s * pi + s_)

# test if accepts or rejects
def accept(s, s_, c, r, z, R):
  #if r not in R:
  if gcd(bitlistToInt(r.list()), bitlistToInt(R.modulus().list())) != 1:
    print "reject R*"
    return
  e_ = calcE_(c, s, s_, r, z, R)
  if wt(e_) > (n * tau2):
    print "reject wt"
    return
  print "accept"
"""
