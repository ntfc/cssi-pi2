

"""
NAO USAR O f.list()! Usar antes o polyToBitlist()
"""


# parameters for the reducible case
m = 5
n = 621
tau = 1/6
tau2 = 0.29
lam = 80

# bernoulli distribution. Returns a random bit
def Ber(tau=1/6):
  return int(random() < tau)

def initRing():
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
  #return the ring and the 5 polys
  return (R,[f1,f2,f3,f4,f5])

#Chinese Remainder Theorem
def crTheorem(f, g, n, m):
  return crt(f, g, n, m)

"""
All these auxiliary methods need the Ring
"""
# convert integer to bitlist
# example: if R.degree() is 100 and n has 9 bits, this returns a list with length
# 100, and the first 91 (100 - 81) bits are set to 0
def intToBitlist(R, n):
  nbits = Integer(n).nbits()
  deg = R.degree()
  # add the "unnecessary" bits to the left
  l = [0] * (deg - nbits)
  l.extend([int(bit) for bit in Integer(n).binary()])
  return l

# convert a bitlist to sage Integer
def bitlistToInt(l):
  out = 0
  for bit in l:
    out = (out << 1) | int(bit)
  return out

# convert a bitlist to a polynomial
# [1,0,1,1,1] = x^4 + x^2 + x + 1
def bitlistToPoly(R, l):
  v = 0
  #x = R.gen() # type(v) == sage.rings.polynomial.polynomial_quotient_ring_element.PolynomialQuotientRing_field_with_category.element_class
  x = R.polynomial_ring().gen() # type(v) == sage.rings.polynomial.polynomial_gf2x.Polynomial_GF2X
  n = len(l)
  for i in range(0, n):
    if l[i] == 1:
      v += x^(n - i - 1)
  return v

# convert a poly to bitlist
# given x^4 + x^2 + x + 1 = 10111
# returns a list with len = R.degree()
def polyToBitlist(f):
  l = f.list()
  l.reverse()
  return l

# fi must be a list! TODO: validate that
# c must be a bitlist
# returns a list of polynomials in CRT
def pimapping(R, c, fi):
  v = [] # list of v_i
  for f in fi:
    l = list(c) # save original value of c for each iteration
    d = f.degree()
    pad = d - lam
    l.extend([0] * pad)
    # new ring Fi[x]/fi
    Fi = R.polynomial_ring()
    Ri = Fi.quotient_ring(f,'x')
    vi = bitlistToPoly(Ri, l)
    v.append(vi)
  return v

def genkey(R):
  #s = R.random_element()
  #s_ = R.random_element()
  s = random_element(R)
  s_ = random_element(R)
  return (s, s_)

# generate a random element in R
# cast the result to sage.rings.polynomial.polynomial_gf2x.Polynomial_GF2X
def random_element(R):
  # generate random element in R, then convert it to bitlist, then return bitlist
  # as a polynomial type sage.rings.polynomial.polynomial_gf2x.Polynomial_GF2X
  return bitlistToPoly(R, polyToBitlist(R.random_element()))

"""
The protocol
"""
# generate random c. Used by the Reader
def genC(n=lam):
  #c = Integer(getrandbits(n))
  # c is a bitlist
  c = [int(bit) for bit in (Integer(getrandbits(n))).binary()]
  # pad c, if necessary
  if len(c) < n:
    # list of padding bits
    paddedC = [0] * (n - len(c))
    paddedC.extend(c)
    c = paddedC
    # c is now a list with 80bits
  return c

# generate r \in R^*
def genR(R):
  # DUVIDA: da sempre um elemento de R^* ??????
  #return R.random_element()
  return random_element(R)

# generate e
def genE(R):
  l = []
  e = 0
  #x = R.gen()
  x = R.polynomial_ring().gen()
  for i in range(0, R.degree()):
    ci = Ber()
    if ci == 1:
      e += x^i
  return e

def calcZ(R, fi, s, s_, c):
  pi = pimapping(R, c)
  r = genR(R)
  e = genE(R)
  z = []
  for f in fi:
    # do the CRT
    r * (s *
