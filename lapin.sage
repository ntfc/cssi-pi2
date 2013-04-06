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

# TODO: convert polys to integers
