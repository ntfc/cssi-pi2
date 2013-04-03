## constants definition
lam = 80
tau = 1/8
ta2 = 0.27
n = 532
R = PolynomialRing(GF(2));
f = x^532 + x + 1
def constantC(x, y):
  return ((x/y)^x) * ((1-x)/(1-y))^(1-x)

