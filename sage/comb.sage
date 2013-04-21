# convert polynomial to its representation in binary, with W-bit words
def polyToBin(a, f, W=8):
  m = f.degree()
  if a.degree() >= m:
    print "degree a >= degree m"
    return
  aList = a.list()
  t = ceil(m / W)
  s = (W * t) - m
  A = [0] * (t*W)

  lenA = len(A)
  for i in xrange(0, len(aList)):
    #print "{0} = {1}".format(i, len(A) - i)
    A[lenA - i - 1] = int(aList[i])
  return A

# convert an array of W-bit words to a polynomial
def binToPoly(A, f, W=8):
  F = PolynomialRing(GF(2), 'x')
  x = F.gen()
  a = x*0
  lenA = len(A)
  for i in xrange(0, f.degree()):
    # len(A) - i - 1 == last position of A == a_0
    pos = lenA - i - 1
    #print "x^{0} = {1}".format(i, pos)
    if A[pos] == 1:
      a += x**i
  return a

# get the i-th W-bit word from an array
def getWord(A, i, W=8):
  if i >= (len(A) // W) or i < 0:
    print "Error: cant found i-th word. Allowed: 0 .. {0}".format(len(A)-1)
    return
  start = len(A) - (W*i)
  end = start - W
  return A[end : start]

# get the i-th bit of the W-bit word a
def getBit(a, i, W=8):
  if len(a) != W:
    print "Word a is not a {0}-bit word!. Found W={1} instead.".format(W, len(a))
  return a[len(a) - i - 1]

# Given an array in the form C = (C[n],..,C[1],C[0]),
# this returns the truncated array (C[n],..,C[j+1],C[j])
def truncate(A, j, W=8):
  t = len(A) / W
  C = []
  for i in xrange(t - 1, j - 1, -1):
    C.extend(getWord(A,i))
  return C

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

# A and B are already in binary form
# C is returned in binary form
def polyAddition(a, b, f, W=8):
  A = polyToBin(a, f)
  B = polyToBin(b, f)
  return bitwiseXor(A, B)

# perform C{j} = C{j} + B, but return the new C
def addToTruncated(C, B, j, W=8):
  Cj = truncate(C, j, W)
  D = bitwiseXor(Cj, B)
  diff = len(C) - len(D)
  # merge the C{j} + B array with the old part of C
  D.extend(C[len(C) - diff:])
  return D

# leftmost s bits are not considered in the sift operation
# this is equivalent to b = a*x
def shiftRight(A, s, W=8):
  toShift = A[s:]
  b = toShift.pop(0)
  toShift.append(b)
  B = [0] * s
  B.extend(toShift)
  return B

def polyMult(a, b, f, W=8):
  A = polyToBin(a, f)
  B = polyToBin(b, f)

  m = f.degree()
  t = ceil(m / W)
  print t
  s = (W * t) - m
  # C is a 2t word array?
  print "t = {0}, s = {1}".format(t, s)
  C = [0] * (2*t)

  for k in xrange(0, W):
    for j in xrange(0, t):
      Aj = getWord(A, j, W)
      kthBitOfAj = getBit(Aj, k, W)
      #print "{0}-th bit of A[{1}] = {2}".format(k, j, kthBitOfAj)
      if kthBitOfAj == 1:
        #print "add B to C({0}) because {1}th bit of A[{0}] = {2}".format(j, k, kthBitOfAj)
        #print "add {0} to {1}".format(len(B), len(truncate(C, j)))
        C = addToTruncated(C, B, j, W)
    if k != (W-1):
      #print "B = B . x because k = {0}".format(k)
      #print B
      B = shiftRight(B, s, W)
      print "mult"

  return C

# split list in parts
def split_list(alist, wanted_parts=1):
    length = len(alist)
    return [ alist[i*length // wanted_parts: (i+1)*length // wanted_parts]
             for i in range(wanted_parts) ]
