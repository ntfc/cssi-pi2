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
      print "Error: cant found {0}-th word. Allowed: 0 .. {1}".format(i, (len(A)//self.W)-1)
      return
    start = len(A) - (self.W * i)
    end = start - self.W
    return A[end : start]

  # set the word i. the new value is then returned
  def setWord(self, A, i, new_word):
    if i >= (len(A) // self.W) or i < 0:
      print "Error: cant found {0}-th word. Allowed: 0 .. {1}".format(i, (len(A) // self.W)-1)
      return
    if len(new_word) != self.W:
      print "Wrong word length. Found {0}, needed {1}".format(len(new_word), self.W)
    start = len(A) - (self.W * i)
    end = start - self.W

    C = A[ : end]
    C += new_word
    C += A[start : ]
    return C
    
  # get the k-th bit of the j-th W-bit word of polynomial a
  def getBit(self, a, j, k):
    # TODO: validate j and k and a
    # j-th word of polynomial begins at position len(a) - (W*j) - 1
    wordStart = len(a) - (self.W * j) - 1
    # k-th bit is at position wordStart - k
    bitPosition = wordStart - k
    return a[bitPosition]    

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

  # return the multiplication between a and b, not modulo!!
  def polyMult(self, a, b):
    if type(a) != str or type(b) != str:
      print "Polynomials must be in binary form!"
      return
    if len(a) != len(b):
      print "Polynomials must be of same length"
      return
    
    # work with C as a list, because python string are immutable
    m = (self.m*2) - 1 # c->m
    t = ceil(m / self.W) # c->t
    s = self.W*t - m
    #C = list('0' * (t * self.W))
    C = '0' * (t * self.W)

    """ THIS MIGHT BE WRONG. C IMPLEMENTATION IS THE CORRECT ONE! """
    for k in xrange(0, self.W):
      for j in xrange(0, self.t): 
        if int(self.getBit(a, j, k)) == 1:
          """# bAux = b with j words appended to the right
          bAux = self.addWordsToRight(b, j)
          # difference of W-bit words between C and bAux
          diffWords = floor((len(C) - len(bAux)) / self.W)
          # cAux is the adition between C and bAux
          cAux = self.polyAddition(''.join(C[(self.W * diffWords) : ]), bAux)
          # concat disaligned C with calculated cAux
          C = list(''.join(C[ : self.W * diffWords]) + cAux)"""
          newB = self.addWordsToRight(b, j).zfill(t * self.W)
          newB = ('0'*self.s) + (newB[self.s:])
          C = ('0'*s) + (self.polyAddition(C, newB)[s:])
      if k != (self.W - 1):
        b = self.shiftLeft(b)
    # return as str
    return ''.join(C)
    
  # shift left by one bit
  # all we need to to is delete the left-most bit and add one 0 to to the right
  # IMPORTANT: when shifting left, we cannot use the s leftmost bits
  def shiftLeft(self, a):
    # delete the left-most bit
    toShift = list(a[1:]) # work with list instead of string
    toShift.append('0')
    # return as string
    return ('0'*self.s) + (''.join(toShift))[self.s:]
    
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
    return c + ('0' * (j * self.W))
    
  def polyToString(self, a):
    # TODO: a must be binary string
    s = ' | '
    t = len(a) // self.W
    for i in xrange(0, t):
      s += a[(i * self.W) : ((i * self.W )+ self.W)]
      s += ' | '
    return s
    
  # return a mod self.f
  # a must be in binary
  def polyMod(self, a):
    # pre-computation
    r = f - (self.var**f.degree())
    u = []
    for i in xrange(0, self.W):
      u.append(self.polyToBin(r * self.var**i).zfill(len(a)))
    
    for i in xrange(self.m * 2, self.m, -1):
      if a[i] == '1':
        j = floor( (i-self.m) / 32) # isto esta bem?
        print j
    
  # aux method for polyFastMod
  # word: in binary
  # t: in binary, without shifts
  # n: positions to shifts
  # to: 0 (left) or 1 (right)
  # returns word XOR (t SHIFT n)
  def polyFastModXorAndShift(self, word, t, n, to):
    t_shifted = t
    while n > 0:
      if to == 0:
        t_shifted = shiftLeft(t_shifted)
      else:
        t_shifted = shiftRight(t_shifted)
      n -= 1
    c = bitwiseXor(word, t_shifted)
    return c

  # fast reduction modulo x**233 + x**74 + 1
  def polyFastMod(self, a):
    if type(a) != type(self.var):
      print "Type must be {0}".format(type(self.var))
    # values for x**233 + x**74 + 1
    t = 8
    W = 32
    m = 233
    s = 23
    a_t = 16
    if type(a) == type(self.var):
      # convert to binary form
      a = polyToBin(a, self.var).zfill(a_t * W)
      
    A = list(a) # work with list rather than str
    
    for i in xrange(15, 8 - 1, -1):
      T = self.getWord(A, i)
      
      word = i - 8
      Ci = self.getWord(A, word)
      # C[i - 8] = C[i - 8] XOR (T << 23)
      Ci = self.polyFastModXorAndShift(Ci, T, 23, 0)
      # save Ci to A[i-8]
      end = len(A) - (W * word)
      start = end - W
      A[start : end] = list(Ci)
      
      word = i - 7
      Ci = self.getWord(A, word)
      # C[i - 7] = C[i - 7] XOR (T >> 9)
      Ci = self.polyFastModXorAndShift(Ci, T, 9, 1)
      # save Ci to A[i-7]
      end = len(A) - (W * word)
      start = end - W
      A[start : end] = list(Ci)
      
      word = i - 5
      Ci = self.getWord(A, word)
      # C[i - 5] = C[i - 5] XOR (T << 1)
      Ci = self.polyFastModXorAndShift(Ci, T, 1, 0)
      # save Ci to A[i-5]
      end = len(A) - (W * word)
      start = end - W
      A[start : end] = list(Ci)
      
      word = i - 4
      Ci = self.getWord(A, word)
      # C[i - 4] = C[i - 4] XOR (T >> 31)
      Ci = self.polyFastModXorAndShift(Ci, T, 31, 1)
      # save Ci to A[i-4]
      end = len(A) - (W * word)
      start = end - W
      A[start : end] = list(Ci)
    
    T = self.getWord(A, 7)
    # T = C[7] >> 9
    toShift = 9
    while toShift > 0:
      T = shiftRight(T)
      toShift -= 1
    
    word = 0
    Ci = self.getWord(A, word)
    # C[0] = C[0] XOR T
    Ci = self.polyFastModXorAndShift(Ci, T, 0, 0)
    end = len(A) - (W * word)
    start = end - W
    A[start : end] = list(Ci)
      
    word = 2
    Ci = self.getWord(A, word)
    # C[2] = C[2] XOR (T << 10)
    Ci = self.polyFastModXorAndShift(Ci, T, 10, 0)
    end = len(A) - (W * word)
    start = end - W
    A[start : end] = list(Ci)
    
    word = 3
    Ci = self.getWord(A, word)
    # C[3] = C[3] XOR (T >> 22)
    Ci = self.polyFastModXorAndShift(Ci, T, 22, 1)
    end = len(A) - (W * word)
    start = end - W
    A[start : end] = list(Ci)
    
    word = 7
    Ci = self.getWord(A, word)
    # C[7] = C[7] & 0x1FF
    Ci = bitwiseAnd(Ci, ('1'*9).zfill(32))
    end = len(A) - (W * word)
    start = end - W
    A[start : end] = list(Ci)
    
    # return C[7], C[6], .. C[0]
    return ''.join(str(bit) for bit in A[ (t*2 *W) - (t * W) : ])
    
  # fast reduction modulo x**532 + x**1 + 1
  def polyFastMod_lapin(self, a):
    if type(a) != type(self.var):
      print "Type must be {0}".format(type(self.var))
    t = 17
    W = 32
    m = 532
    s = 12
    a_t = 34
    if type(a) == type(self.var):
      a = polyToBin(a, self.var).zfill(a_t * W)
    
    A = list(a) # work on list
    for i in xrange(33, 17 - 1, -1):

      T = self.getWord(A, i)

      word = i - 17
      Ci = self.getWord(A, word)
      #### C[i - 17] = C[i - 17] XOR (T << 12) XOR (T << 13)
      # C[i - 17] = C[i - 17] XOR (T << 12)
      Ci = self.polyFastModXorAndShift(Ci, T, 12, 0)
      # C[i - 17] = C[i - 17] XOR (T << 13)
      Ci = self.polyFastModXorAndShift(Ci, T, 13, 0)
      # save Ci to A[i-17]
      end = len(A) - (W * word)
      start = end - W
      A[start : end] = list(Ci)
      
      word = i - 16
      Ci = self.getWord(A, word)
      #### C[i - 16] = C[i - 16] XOR (T >> 20) XOR (T << 19)
      # C[i - 16] = C[i - 16] XOR (T >> 20)
      Ci = self.polyFastModXorAndShift(Ci, T, 20, 1)
      # C[i - 16] = C[i - 16] XOR (T >> 19)
      Ci = self.polyFastModXorAndShift(Ci, T, 19, 1)
      # save Ci to A[i-16]
      end = len(A) - (W * word)
      start = end - W
      A[start : end] = list(Ci)
      
    T = self.getWord(A, 16)
    """# T = C[16] >> 20
    toShift = 20
    while toShift > 0:
      T = shiftRight(T)
      toShift -= 1"""
      
    word = 0
    Ci = self.getWord(A, word)
    # C[0] = C[0] XOR (T >> 20)
    Ci = self.polyFastModXorAndShift(Ci, T, 20, 1)
    # C[0] = C[0] XOR (T >> 19)
    Ci = self.polyFastModXorAndShift(Ci, T, 19, 1)
    end = len(A) - (W * word)
    start = end - W
    A[start : end] = list(Ci)
    
    word = 16
    Ci = self.getWord(A, word)
    # C[16] = C[16] & 0xFFFF
    Ci = bitwiseAnd(Ci, ('1'*20).zfill(32))
    end = len(A) - (W * word)
    start = end - W
    A[start : end] = list(Ci)
    
    # return C[16], C[15], .. C[0]
    return ''.join(str(bit) for bit in A[ (t*2 *W) - (t * W) : ])

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
  
def bitwiseAnd(a, b):
  if len(a) != len(b):
    print "String must be of same size"
    return
  c = ''.join(map(lambda x,y : str(int(x).__and__(int(y))), a, b))
  return c

# shift left by one bit
# all we need to to is delete the left-most bit and add one 0 to to the right
def shiftLeft(a):
  # delete the left-most bit
  toShift = list(a[1:]) # work with list instead of string
  toShift.append('0')
  # return as string
  return ''.join(toShift)
  
# shift right by one bit
# all we need to to is delete the right-most bit and add one 0 to to the left
def shiftRight(a):
  # delete the right-most bit
  toShift = list(a[ : len(a) - 1]) # work with list instead of string
  toShift.insert(0, '0')
  # return as string
  return ''.join(toShift)

def polyToBin(a, x):
  aList = a.list()
  # reverse list
  aList.reverse()
  # convert list to str
  A = ''.join(str(ch) for ch in aList)
  # return str A with t W-bit words
  return A

