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

# convert integer to bitlist
def intToBitlist(n):
  return [int(bit) for bit in Integer(n).binary()]

# convert a bitlist to sage Integer
def bitlistToInt(l):
  out = 0
  for bit in l:
    out = (out << 1) | int(bit)
  return out

# pad list of bits
def padBitList(l, padding, v=0):
  # TODO: padding < len(l)
  l.extend([int(v)] * padding)
  return l

# pad number n. padding must be the number of pads to add
# return as a bitlist
def padNumber(n, padding, v=0):
  l = intToBitlist(n)

  return padBitList(l, padding, v)

# convert a bitlist to a polynomial
def bitlistToPoly(R, l):
  v = 0
  x = R.gen()
  for i in range(0, len(l)):
    if l[i] == 1:
      v += x^i
  return v

# fi must be a list! TODO: validate that
# returns a list of polynomials in CRT TODO: que e um poly em CRT?
"""
















## DUVIDA ##
Como representar os polinomios? Porque depois vamos ter de fazer contas com eles,
e entao e melhor ver a melhor maneira de os guardar (e de fazer padding)
x^3 + x + 1 <=> 1 1 0 1
OU
x^3 + x + 1 <=> 1 0 1 1
Caso seja a primeira opcao, faz-se padding para a esquerda certo?
Caso seja a segunda, e preciso mudar tudo no lapin.sage!
E depois a conversao para binario, o LSB esta na esquerda certo?









"""
def pimapping(R, c, fi):
  v = []
  l = padNumber(c, 80)
  for f in fi:
    d = f.degree()
    pad = d - 80
    vi = bitlistToPoly(R, padBitList(l, pad))
    v.append(vi)
  return v

"""
The protocol
"""
# generate random c. Used by the Reader
def genC(n=80):
  c = Integer(getrandbits(n))
  return c
