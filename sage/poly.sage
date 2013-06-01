class BinaryPolynomial:
  def __init__(self, f, W=8):
    self.var = PolynomialRing(GF(2),'x').gen()
    if type(f) != type(self.var):
      #print "Polynomial f must be of type {0}".format(type(x))
      # convert expression f to polynomial
      f = expressionToPoly(f, self.var)

    self.f = f
    self.W = W
    self.m = self.f.degree()
    self.t = ceil(self.m / self.W)
    self.s = (self.W * self.t) - self.m


  # convert polynomial to its representation in binary, with W-bit words
  def polyToBin(self, a):
    if type(a) != type(self.var):
      # convert a to poly type
      a = expressionToPoly(a, self.var)
    if a.degree() >= self.m:
      print "degree a >= degree m"
      return
    aList = a.list()
    # reverse list
    aList.reverse()
    # convert list to str
    A = ''.join(str(ch) for ch in aList)
    # return str A with t W-bit words
    return A .zfill(self.W * self.t)

  # convert an array of W-bit words to a polynomial
  def binToPoly(self, A):
    a = self.var*0
    # traverse A from left to right
    for i in xrange(0, len(A)):      
      if int(A[i]) == 1:
        # the symmetric position of i
        pos = len(A) - 1 - i
        a += self.var**pos
    return a

  # get the i-th W-bit word from an array
  def getWord(self, A, i):
    if i >= (len(A) // self.W) or i < 0:
      print "Error: cant found {0}-th word. Allowed: 0 .. {1}".format(i, len(A)-1)
      return
    start = len(A) - (self.W * i)
    end = start - self.W
    return A[end : start]

  # get the k-th bit of the j-th W-bit word of polynomial a
  def getBit(self, a, j, k):
    # TODO: validate j and k and a
    # j-th word of polynomial begins at position len(a) - (W*j) - 1
    wordStart = len(a) - (self.W * j) - 1
    # k-th bit is at position wordStart - k
    bitPosition = wordStart - k
    return a[bitPosition]    

  # Given an array in the form C = (C[n],..,C[1],C[0]),
  # this returns the truncated array (C[n],..,C[j+1],C[j])
  def truncate(self, A, j):
    # TODO: confirmar se este t e mesmo calculado assim..
    t = len(A) / self.W
    C = []
    for i in xrange(t - 1, j - 1, -1):
      C.extend(self.getWord(A, i))
    return C

  # a and b must be in binary form
  # C is returned in binary form
  def polyAddition(self, a, b):
    if type(a) != str or type(b) != str:
      print "Polynomials must be in binary form!"
      return
    if len(a) != len(b):
      print "Polynomials must be of same length"
      return
    return bitwiseXor(a, b)

  # DEPRECATED
  # perform C{j} = C{j} + B, but return the new C
  def addToTruncated(self, C, B, j):
    Cj = self.truncate(C, j)
    D = bitwiseXor(Cj, B)
    diff = len(C) - len(D)
    # merge the C{j} + B array with the old part of C
    D.extend(C[len(C) - diff:])
    return D

  # return the multiplication between a and b, not modulo!!
  def polyMult(self, a, b):
    if type(a) != str or type(b) != str:
      print "Polynomials must be in binary form!"
      return
    if len(a) != len(b):
      print "Polynomials must be of same length"
      return
    # TODO: confirmar que len(C) = 2*t - 1 palavras de W-bits
    # work with C as a list, because python string are immutable
    C = list('0' * ((2*self.t) * self.W))
    for k in xrange(0, self.W):
      for j in xrange(0, self.t): 
        if int(self.getBit(a, j, k)) == 1:
          # bAux = b with j words appended to the right
          bAux = self.addWordsToRight(b, j)
          # difference of W-bit words between C and bAux
          diffWords = (len(C) - len(bAux)) // self.W
          # cAux is the adition between C and bAux
          cAux = self.polyAddition(''.join(C[(self.W * diffWords) : ]), bAux)
          # concat disaligned C with calculated cAux
          C = list(''.join(C[ : self.W * diffWords]) + cAux)
      if k != (self.W - 1):
        b = shiftLeft(b)
    # return as str
    return ''.join(C)
    
  # add j W-bit words to the left of array of words c
  def addWordsToLeft(self, c, j):
    if j <= 0:
      return c
    # j words = j * W bits
    return c.zfill(len(c) + j*self.W)
    
  # add j W-bit words to the right of array of words c
  def addWordsToRight(self, c, j):
    if j <= 0:
      return c
    return c + '0' * (j * self.W)
    
  def polyToString(self, a):
    # TODO: a must be binary string
    s = ' | '
    t = len(a) // self.W
    for i in xrange(0, t):
      s += a[(i * self.W) : ((i * self.W )+ self.W)]
      s += ' | '
    return s
    
  # return a mod self.f
  def polyMod(self, a):
    


""" End of class Binary Polynomial """

""" Auxiliary functions """
# split list in parts
def split_list(alist, wanted_parts=1):
    length = len(alist)
    return [ alist[i*length // wanted_parts: (i+1)*length // wanted_parts]
             for i in range(wanted_parts) ]

# convert a sage expression to a polynomial
def expressionToPoly(e, var):
  p = 0*var
  for op in e.coeffs():
    p += var**op[1]
  return p

# perform bitwise xor on two binary strings
# strings must be of same size
def bitwiseXor(a, b):
  if len(a) != len(b):
    print "String must be of same size"
    return
  c = ''.join(map(lambda x,y : str(int(x).__xor__(int(y))), a, b))
  return c

# shift left by one bit
# all we need to to is delete the left-most bit and add one 0 to to the right
def shiftLeft(a):
  # delete the left-most bit
  toShift = list(a[1:]) # work with list instead of string
  toShift.append('0')
  # return as string
  return ''.join(toShift)
