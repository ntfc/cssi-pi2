class BinaryPolynomial:
  def __init__(self, f, W=8):
    self.var = PolynomialRing(GF(2),'x').gen()
    if type(f) != type(self.var):
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
    A = [0] * (self.t * self.W)

    lenA = len(A)
    for i in xrange(0, len(aList)):
      A[lenA - i - 1] = int(aList[i])
    return A

  # convert an array of W-bit words to a polynomial
  def binToPoly(self, A):
    a = self.var*0
    lenA = len(A)
    for i in xrange(0, self.m):
      # len(A) - i - 1 == last position of A == a_0
      pos = lenA - i - 1
      #print "x^{0} = {1}".format(i, pos)
      if A[pos] == 1:
        a += self.var**i
    return a

  # get the i-th W-bit word from an array
  def getWord(self, A, i):
    if i >= (len(A) // self.W) or i < 0:
      print "Error: cant found {0}-th word. Allowed: 0 .. {1}".format(i, len(A)-1)
      return
    start = len(A) - (self.W * i)
    end = start - self.W
    return A[end : start]

  # get the i-th bit of the W-bit word a
  def getBit(self, a, i):
    if len(a) != self.W:
      print "Word a is not a {0}-bit word!. Found W={1} instead.".format(self.W, len(a))
    return a[len(a) - i - 1]

  # Given an array in the form C = (C[n],..,C[1],C[0]),
  # this returns the truncated array (C[n],..,C[j+1],C[j])
  def truncate(self, A, j):
    t = len(A) / self.W
    C = []
    for i in xrange(t - 1, j - 1, -1):
      C.extend(self.getWord(A, i))
    return C

  # A and B are already in binary form
  # C is returned in binary form
  def polyAddition(self, a, b):
    A = self.polyToBin(a)
    B = self.polyToBin(b)
    return bitwiseXor(A, B)

""" Auxiliary functions """

# convert a sage expression to a polynomial
def expressionToPoly(e, var):
  p = 0*var
  for op in e.coeffs():
    p += var**op[1]
  return p

# perform bitwise xor on two lists
# lists can have different length
def bitwiseXor(a, b):
  lenA = len(a)
  lenB = len(b)
  if len(a) >= len(b):
    diff = lenA - lenB
    c = a[:diff]
    c.extend(map(lambda x,y : int(x).__xor__(int(y)), a[diff:], b))
  else:
    diff = lenB - lenA
    c = b[:diff]
    c.extend(map(lambda x,y : int(x).__xor__(int(y)), a, b[diff:]))
  return c

# leftmost s bits are not considered in the sift operation
# this is equivalent to b = a*x
def shiftRight(A, s=0):
  toShift = A[s:]
  b = toShift.pop(0)
  toShift.append(b)
  B = [0] * s
  B.extend(toShift)
  return B

def shiftLeft(A, s=0):
  toShift = A[s:]
  b = toShift.pop(len(toShift)-1)
  B = [0] * s
  B.append(b)
  B.extend(toShift)
  return B