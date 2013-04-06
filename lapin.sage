def constantC(x, y):
  return ((x/y)^x) * ((1-x)/(1-y))^(1-x)

def Ber(tau=1/8):
  # aquele range(1) pode ser optimizado...
  return sum (random() < tau for _ in range(1))

lam = 80
tau = 1/8
tau2 = 0.27
n = 532

def initRing(f=x^532+x+1):
  # crete finite field F_2[a]
  F = PolynomialRing(GF(2), 'x')
  x = F.gen()
  # create the ring
  R = F.quotient(f, 'x')
  x = F.gen()
  return R

def genkey(R):
  s = R.random_element()
  s_ = R.random_element()
  return (s, s_)

# generate challenge
def genchallenge(n=80):
  return Integer(getrandbits(n))

def pimapping(R, c):
  l = c.bits()
  coefs = []
  for x in range(0, 16):
    ci = l[x*5:(x*5)+5]
    #print "{0} = {1}".format(ci, bitlistToInt(ci))
    # here we use x instead of the (x-1) indicated in the paper
    i = 16 * x + bitlistToInt(ci)
    print "i = {0}".format(i)
    coefs.append(i)
  return coefs

def createPoly(R, l):
  f = 0
  for i in range(0, len(l)):
    f += x^l[i]
  return f

def bitlistToInt(l):
  out = 0
  for bit in l:
    out = (out << 1) | bit
  return out

# TODO: convert polys to integers
