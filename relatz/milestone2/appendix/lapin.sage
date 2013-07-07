class Lapin:
  """ Lapin superclass """
  def __init__(self, reducible=False):
    self.reducible = reducible
    if reducible == False:
      self.sec_param = 80
      self.tau = 1/8
      self.tau2 = 0.27
      self.n = 532
      self.protocol = Irreducible()
    else:
      self.sec_param = 80
      self.tau = 1/6
      self.tau2 = 0.29
      self.n = 621
      self.protocol = Reducible()
      
  def genKey(self):
    self.key1 = binToPoly(polyToBin(self.protocol.R.random_element(), self.protocol.x), self.protocol.x)
    self.key2 = binToPoly(polyToBin(self.protocol.R.random_element(), self.protocol.x), self.protocol.x)
    
  # generate c
  def reader_step1(self):
    # generate binary string with len(c) = self.sec_param
    return bin(getrandbits(self.sec_param))[2:].zfill(self.sec_param)
    
  # generate (r,z)
  def tag_step2(self,c):
    if self.reducible == False:
      """
      " Irreducible
      """
      r = protocol.genR()
      e = protocol.genE(self.tau)
      pi = protocol.pimapping(c)
      z = r * (self.key1 * pi + self.key2) + e
      return (r,z)
    elif self.reducible == True:
      """
      " Reducible
      """
      fi = self.protocol.fi
      
      r = reduceToCRT(self.protocol.genR(), fi)
      e = reduceToCRT(self.protocol.genE(self.tau), fi)
      # keys in crt form
      (s1, s2) = (reduceToCRT(self.key1, fi), reduceToCRT(self.key2, fi))
      pi = self.protocol.pimapping(c)
      
      z = addCRT(multCRT(r, addCRT(multCRT(s1, pi, fi), s2, fi), fi), e, fi)
      return(r,z)
    else:
      print "irreducible parameter must be either 0 or 1"
    
  # verification
  def reader_step3(self,c,r,z):
    if self.reducible == False:
      """
      " Irreducible
      """
      if r.gcd(self.protocol.f) != 1:
        print "reject"
        return False
      e2 = (z - r * (self.key1 * protocol.pimapping(c) + self.key2)).mod(self.protocol.f)
      if e2.hamming_weight() > (self.n * self.tau2):
        print "reject"
        return False
      print "accept"
      return True
    elif self.reducible == True:
      """
      " Reducible
      """
      fi = self.protocol.fi
      (r1, z1) = (CRT_list(r, fi), CRT_list(z, fi))
      if r1.gcd(self.protocol.f) != 1:
        print "reject"
        return False
      pi = CRT_list(self.protocol.pimapping(c), fi)

      e2 = (z1 - r1 * (self.key1 * pi + self.key2)).mod(self.protocol.f)

      if e2.hamming_weight() > (self.n * self.tau2):
        print "reject"
        return False
      
      print "accept"
      return True
    else:
      print "irreducible parameter must be either 0 or 1"
      return False