# parameters for the reducible case
m = 5
n = 621
tau = 1/6
tau2 = 0.29
lam = 80

# irreducible pentanomials
f = x^127 + x^8 + x^7 + x^3 + 1
g = x^126 + x^9 + x^6 + x^5 + 1
p = x^125 + x^9 + x^7 + x^4 + 1
l = x^122 + x^7 + x^4 + x^3 + 1
k = x^121 + x^8 + x^5 + x + 1 

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