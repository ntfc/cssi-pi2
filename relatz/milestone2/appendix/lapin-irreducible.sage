class Irreducible:
  """ Irreducible protocol """
  def __init__(self):
    (self.R, self.f, self.x) = self.__initRing()
    
  def __initRing(self):
    F = PolynomialRing(GF(2), 'x')
    x = F.gen()
    f = x^532 + x + 1
    R = F.quotient(f, 'x')
    return (R, f, x)

  # returns the list of coefficients of the new polynomial v
  # c must be in binary format
  def pimapping(self, c):
    v = 0*self.x
    for j in xrange(0, 16):
      # 32 = 5 bits
      cj = c[j*5 : (j*5) + 5]
      # here we use x instead of the (x-1) indicated in the paper
      # because we're in 0-index mode
      i = (16 * j) + binToInt(cj) # TODO: use binary operations here as well
      v += self.x**i
    return v
  
  # generate r
  def genR(self):
    # workaround to convert to sage.rings.polynomial.polynomial_gf2x.Polynomial_GF2X
    r = binToPoly(polyToBin(self.R.random_element(), self.x), self.x)
    while r.gcd(self.f) != 1:
      # workaround to convert to sage.rings.polynomial.polynomial_gf2x.Polynomial_GF2X
      r = binToPoly(polyToBin(self.R.random_element(), self.x), self.x)
    return r

  # generate e
  def genE(self, tau):
    l = []
    e = 0*self.x
    for i in xrange(0, self.f.degree()):
      ci = bernoulli(tau)
      if ci == 1:
        e += self.x**i
    return e