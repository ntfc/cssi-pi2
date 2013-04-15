

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
  if nbits != 0:
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
  #x = R.gen() # type(v) == sage.rings.polynomial.polynomial_quotient_ring_element.PolynomialQuotientRing_field_with_category.element_class
  x = R.polynomial_ring().gen() # type(v) == sage.rings.polynomial.polynomial_gf2x.Polynomial_GF2X
  v = 0*x
  n = len(l)
  for i in range(0, n):
    if l[i] == 1:
      v += x**(n - i - 1)
  return v

# convert a poly to bitlist
# given x^4 + x^2 + x + 1 = 10111
# returns a list with len = R.degree()
def polyToBitlist(f):
  l = f.list()
  l.reverse()
  return l

def polyToBitlistPadded(f, size):
  l = polyToBitlist(f)
  pad = size - len(l)
  # reverse
  l.reverse()
  # Pad list, at the end
  l.extend([0]*pad)
  # revere again
  l.reverse()
  return l

# f: modulus polynomial
# p: polynomial to convert
# convert to an array of W-bit words
def polyToWbitList(f, p, W=32): # default = 32bits
  m = f.degree()
  if m <= p.degree():
    print "Degree of polynomial p must be at most m-1"
    return
  t = ceil(m/W)
  s = (W * t) - m
  l = polyToBitlistPadded(p, t * W)
  A = [] # length = t. W bits in each position
  print "t = {0} and s = {1}". format(t, s)
  for i in range(0, t):
    w = len(l) - (i * W)
    ai = l[w-W : w]
    A.append(ai)
  return (s,t,A)

# modulo f, A in Wbitlist format
# returns a list with f.degree() bits
def wBitlistToBitlist(f, A, W=32):
  m = f.degree()
  t = ceil(m/W)
  s = (W * t) - m
  a = []
  a.extend(A[t-1][0:s])

  for i in range(t-2, -1, -1):
    a.extend(A[i])
  return a


# fis must be a list!
# c must be a bitlist, with length = 80
# returns a list of polynomials in CRT
def pimapping(R, c, fis):
  #validate if fis is a list
  if type(fis) is not list:
    print"Error: fi must be a list!"
    return
  v = [] # list of v_i
  for fi in fis:
    newList = list(c)
    toPad = fi.degree() - 80
    newList.extend([0] * toPad) #after this, len(newList) = fi.degree()
    v.append(bitlistToPoly(R, newList))

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

# perform bitwise xor on two lists of the same length
def bitwiseXor(l1, l2):
  if len(l1) != len(l2):
    print "Error: l1 and l2 must be of equal length"
    return
  r = []
  for i in range(0, len(l1)):
    # cast to Integer is important
    r.append(Integer(l1[i]).__xor__(Integer(l2[i])))
  return r

# modulo f
# polynomial a and b, returns c = a + b
def addition(f, a, b):
  W = 32
  (s, t, A) = polyToWbitList(f, a)
  B = polyToWbitList(f, b)[2]
  C = []
  for i in range(0, t):
    C.append(bitwiseXor(A[i], B[i]))
  return C

# right-to-left comb method
def multiplication(f, a, b):
  W = 32
  s, t, A = polyToWbitList(f, a)
  B = polyToWbitList(f, b)[2]
  x = f.variables()[0]
  c = 0*x # polynomial = 0

  """ Qual destes e mesmo o C? """
  C = [0] * (f.degree()-1)
  #C = polyToWbitList(f, c)
  """  """

  for k in range(0, W):
    for j in range(0, t):
      if A[j][k] == 1:
        """
        TODO: Como e que se adiciona B a C{j}??
        TODO: B <- B*z e para fazer shift como em FC?
        http://www.engr.uconn.edu/~zshi/publications/shi08software.pdf
        """
        #Cj = C[0:(len(C) - j)]
        # add Cj = Cj + B mod f

  return C

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
  #x = R.gen()
  x = R.polynomial_ring().gen()
  e = x*0
  for i in range(0, n):
    ci = Ber()
    if ci == 1:
      e += x^i
  return e

# reduces a polynomial to its CRT form
# a: polynomial to reduce
# fi: list of modulus
def reduceToCRT(a, fi):
  c = []
  for f in fi:
    c.append(a.mod(f))
  return c

def calcZ(R, fi, s, s_, c):
  pi = pimapping(R, c, fi)
  # TODO: gerar apenas o e a partir do anel R. Derivar r da chave s
  r = genR(R)
  e = genE(R)
  # TODO: optimizar esta parte
  rCRT = reduceToCRT(r, fi)
  eCRT = reduceToCRT(e, fi)
  s_CRT = reduceToCRT(s_, fi)
  sCRT = reduceToCRT(s, fi)
  z = [] # c will be in CRT form
  for i in range(0, len(fi)):
    # TODO: usar o metodo de multiplicacao do paper
    zi = rCRT[i].mod(fi[i]) * ((sCRT[i].mod(fi[i])*pi[i]) + s_CRT[i].mod(fi[i])) + eCRT[i].mod(fi[i])
    z.append(zi.mod(fi[i]))
  return z

def calcE_(R, s, s_, pi, r, z):
  return (z - r * (s * pi + s_)).mod(R.modulus())

# tag step1. generate e, r and z in CRT form
def step2(R, s, s_, c):
  r = genR(R)
  e = genE(R)
  # z = r * (s*pi + s') + e
  piCRT = CRT_list(pimapping(R, c, fi), fi)
  z = r * (s*piCRT + s_) + e
  return z.mod(R.modulus())


def verify(R, fi, s, s_, c, r, zCRT):
  if r.gcd(R.modulus()) != 1:
    print "reject R*"
    return
  # create mapping and do the CRT
  pi = CRT_list(pimapping(R, c, fi), fi)
  # compute z
  z = CRT_list(zCRT, fi)
  e_ = calcE_(R, s, s_, pi, r, z)
  if e_.hamming_weight() > (n * tau2):
    print "reject wt"
    return
  print "accept"
