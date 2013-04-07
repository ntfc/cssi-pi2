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
  return R

#Chinese Remainder Theorem
def crTheorem(f, g, n, m):
  return crt(f, g, n, m)

#degree of polynomial
def polyDegree(f):
  return f.degree()

#tambem já não tenho a certeza que isto seja assim :/
#xor integers
def xor(a, b):
  return a.__xor__(b)

#não tenho a certeza que esteja certo isto
# returns the list of coefficients of an element vi
def pimapping(c, f):
  d = polyDegree(f)
  dif = d-80
  t = xor(c, dif)
  l = c.bits()
  return l
