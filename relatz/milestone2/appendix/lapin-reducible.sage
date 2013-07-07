class Reducible:
  """ Reducible protocol """
  def __init__(self):
    (self.R, self.f, self.fi, self.x) = self.__initRing()
    
  def __initRing(self):
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
    return (R, f, [f1,f2,f3,f4,f5], x)
  
  # c must be a binary
  # returns a list of polynomials in CRT
  def pimapping(self, c):
    v = [] # list of v_i
    for p in self.fi:
      toPad = p.degree() - 80
      # pad to the left
      poly = (''.join(['0' * toPad, c]))
      v.append(binToPoly(poly, self.x))
    return v
    
  # generate r
  def genR(self):
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