# parameters for the reducible case
m = 5
n = 621
tau = 1/6
tau2 = 0.29
lam = 80


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

#degree of polynomial
def polyDegree(f):
  return f.degree()

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

# create the v \in R polynomial
# l must be a bit list
# [1,0,0,1,1] = x^4 + x + 1
def createPolyFromBitlist(R, l):
  v = 0
  x = R.gen()
  for i in range(len(l)-1,-1,-1): # reverse order
    if l[i] == 1:
      v += x^i
  return v

# fi must be a list! TODO: validate that
# returns a list of polynomials in CRT TODO: que e um poly em CRT?
def pimapping(c, R, fi):
  v = []
  for f in fi:
    d = f.degree()
    pad = d - 80
    # c must have exactly 80 bits
    newC = padNumber(c, 80-c.nbits())
    newC.extend([0] * pad)
    v.append(createPolyFromBitlist(R,newC))
  return v

"""
The protocol
"""
# generate random c. Used by the Reader
def genC(n=80):
  c = Integer(getrandbits(n))
  return c
