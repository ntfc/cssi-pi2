# parameters for the irreducible case
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

"""
" The Protocol
"""

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
