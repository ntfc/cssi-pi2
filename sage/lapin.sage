# TODO: either use only Polys or use only bins!

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
    else:
      self.sec_param = 80
      self.tau = 1/6
      self.tau2 = 0.29
      self.n = 621
      self.protocol = Reducible()
      
  # TODO: throw something when trying to use protocol without generating keys
  def genKey(self):
    self.key1 = binToPoly(polyToBin(self.protocol.R.random_element(), self.protocol.x), self.protocol.x)
    self.key2 = binToPoly(polyToBin(self.protocol.R.random_element(), self.protocol.x), self.protocol.x)
    
  # generate c
  def reader_step1(self):
    # generate binary string with len(c) = self.sec_param
    return bin(getrandbits(self.sec_param))[2:].zfill(self.sec_param)
    
  # generate (r,z)
  def tag_step2(self,c):
    if self.reducible == False:
      """
      " Irreducible
      """
      r = protocol.genR()
      e = protocol.genE(self.tau)
      pi = protocol.pimapping(c)
      z = r * (self.key1 * pi + self.key2) + e
      return (r,z)
    elif self.reducible == True:
      """
      " Reducible
      """
      fi = self.protocol.fi
      
      r = reduceToCRT(self.protocol.genR(), fi)
      e = reduceToCRT(self.protocol.genE(self.tau), fi)
      # keys in crt form
      (s1, s2) = (reduceToCRT(self.key1, fi), reduceToCRT(self.key2, fi))
      pi = self.protocol.pimapping(c)
      
      # z = add( mult (r, add( mult( s, pi), s')), e) == r * (s * pi) + e
      z = addCRT(multCRT(r, addCRT(multCRT(s1, pi, fi), s2, fi), fi), e, fi)
      return(r,z)
    else:
      print "irreducible parameter must be either 0 or 1"
    
  # verification
  # TODO: create one class for Reader and another for Tag => with that, we dont need to pass all these params
  def reader_step3(self,c,r,z):
    if self.reducible == False:
      """
      " Irreducible
      """
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
      """
      " Reducible
      """
      fi = self.protocol.fi
      # TODO: r and z must be in CRT form
      (r1, z1) = (CRT_list(r, fi), CRT_list(z, fi))
      if r1.gcd(self.protocol.f) != 1:
        print "reject R*"
        return False
      pi = CRT_list(self.protocol.pimapping(c), fi)

      e2 = (z1 - r1 * (self.key1 * pi + self.key2)).mod(self.protocol.f)

      if e2.hamming_weight() > (self.n * self.tau2):
        print "reject wt"
        return False
      
      print "accept"
      return True
    else:
      print "irreducible parameter must be either 0 or 1"
      return False
        
class Irreducible:
  """ Irreducible protocol """
  def __init__(self):
    (self.R, self.f, self.x) = self.__initRing()
    
  def __initRing(self):
    F = PolynomialRing(GF(2), 'x')
    x = F.gen()
    f = x^532 + x + 1
    R = F.quotient(f, 'x')
    return (R, f, x)

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
  
class Reducible:
  """ Reducible protocol """
  def __init__(self):
    (self.R, self.f, self.fi, self.x) = self.__initRing()
    
  def __initRing(self):
    F = PolynomialRing(GF(2), 'x')
    x = F.gen()
    # irreducible pentanomials
    f1 = x^127 + x^8 + x^7 + x^3 + 1
    f2 = x^126 + x^9 + x^6 + x^5 + 1
    f3 = x^125 + x^9 + x^7 + x^4 + 1
    f4 = x^122 + x^7 + x^4 + x^3 + 1
    f5 = x^121 + x^8 + x^5 + x + 1
    # calculate polynomial f
    f = f1 * f2 * f3 * f4 * f5
    # create the ring
    R = F.quotient_ring(f, 'x')
    return (R, f, [f1,f2,f3,f4,f5], x)
  
  # c must be a binary
  # returns a list of polynomials in CRT
  def pimapping(self, c):
    v = [] # list of v_i
    for p in self.fi:
      toPad = p.degree() - 80
      # pad to the left
      poly = (''.join(['0' * toPad, c]))
      v.append(binToPoly(poly, self.x))
    return v
    
  # generate r
  def genR(self):
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

# converts a binary string to int
def binToInt(i):
  """out = 0
  for bit in i:
    out = (out << 1) | int(bit)
  return out"""
  return Integer(i, 2)

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

# reduces a polynomial to its CRT form
# a: polynomial to reduce
# fi: list of modulus
def reduceToCRT(a, fi):
  return map(lambda f : a.mod(f), fi)

# f1 and f2 must be in CRT form
# result in CRT form
def multCRT(p1, p2, fi):
  p1zip = zip(p1, fi)
  p2zip = zip(p2, fi)

  # returns a list with all multiplications done modulo each fi
  # CRT_list(return value, fi) == (p1*p2).mod(fi)
  return map(lambda ((x1, y1), (x2, y2)) : (x1 * x2).mod(y1), zip(p1zip, p2zip))

# p1 and p2 must be in CRT form
# result in CRT form
def addCRT(p1, p2, fi):
  pAddZip = zip(p1,p2)

  return map(lambda (x,y,z) : (x+y).mod(z), zip(p1, p2, fi))
