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
      #print "Polynomials must be in binary form! Converting to poly form.."
      a = self.polyToBin(a)
      b = self.polyToBin(b)
    if len(a) != len(b):
      print "Polynomials must be of same length"
      return
    if len(a) > self.t*self.W:
      print "polynomial must be of length {0}".format(self.t * self.W)

    a = a.zfill(self.t * self.W)
    b = b.zfill(self.t * self.W)    
    
    m_2 = (self.m*2) - 1 # c->m. means that deg(c) <= c->m - 1
    t_2 = ceil(m_2 / self.W) # c->t
    s_2 = (self.W * t_2) - m_2 # c->s

    ## work with C as a list, because python string are immutable
    C = list('0' * (t_2  * self.W))
    
    # b needs an extra word because of the shifts
    b = b.zfill(len(b) + self.W)
    for k in xrange(0, self.W):
      for j in xrange(0, self.t): 
        if int(self.getBit(a, j, k)) == 1:
          i = j
          # since b has an extra word, we can have (i-j) <= self.t instead of < self.t
          while (i < t_2) and (i-j <= self.t):
            # C[i] = C[i] XOR B[i-j]
            Ci = self.getWord(C, i)
            Ci = bitwiseXor(Ci, self.getWord(b, i-j))
            # save Ci to C[i]
            end = len(C) - (self.W * i)
            start = end - self.W
            C[start : end] = list(Ci)
            i += 1
      if k != (self.W - 1):
        b = self.shiftLeft(b)
    # return as str
    return ''.join(C)
    
  # return the multiplication between a and b, not modulo!!
  def polyMultV2(self, a, b):
    if type(a) != str or type(b) != str:
      #print "Polynomials must be in binary form! Converting to poly form.."
      a = polyToBin(a, self.var)
      b = polyToBin(b, self.var)
    if len(a) != len(b):
      print "INFO: Polynomials of different length"
    # TODO: if string a and are padded, this is wrong..
    a_m = len(a)
    a_t = ceil(a_m / self.W)
    b_m = len(b)
    b_t = ceil(b_m / self.W)
    
    m = max(a_m, b_m)
    t = ceil(m / self.W)

    # zfill a and b
    a = a.zfill(a_t * self.W)
    b = b.zfill(b_t * self.W)
    
    c_t = t*2
    ## work with C as a list, because python string are immutable
    C = list('0' * (c_t  * self.W))

    # b needs an extra word because of the shifts
    b = b.zfill(len(b) + self.W)
    
    for k in xrange(0, self.W):
      for j in xrange(0, t):
        # a_t may be smaller than t..
        if j < a_t: 
          if int(self.getBit(a, j, k)) == 1:
            i = j
            ## since b has an extra word, we can have (i-j) <= self.t instead of < self.t
            #while (i < t_2) and (i-j <= self.t):
            while (i < c_t) and (i-j <= b_t):
              # C[i] = C[i] XOR B[i-j]
              Ci = self.getWord(C, i)
              Ci = bitwiseXor(Ci, self.getWord(b, i-j))
              # save Ci to C[i]
              end = len(C) - (self.W * i)
              start = end - self.W
              C[start : end] = list(Ci)
              i += 1
      if k != (self.W - 1):
        b = self.shiftLeft(b)
    # return as str
    return ''.join(C)

  # return a mod self.f
  # c must be in binary
  def polyMod(self, c):
    if type(c) != str:
      c = polyToBin(c, self.var)
    
    c_m = len(c)
    c_t = ceil(c_m / self.W)
    c_s = self.W * c_t - c_m
    
    c = c.zfill(c_t * self.W)
    
    m = self.m
    t = self.t

    r = self.f - self.var**self.f.degree()
    r_t = ceil(r.degree() / self.W) + 1 # tested. works
    #r_t = self.t # tested. works
    # pre computation
    u = []
    u.append(polyToBin(r, self.var).zfill(r_t * self.W))
    for k in xrange(1, self.W):
      u.append(self.shiftLeft(u[k - 1]))
    # print polys...
    #print "c = {0}".format(c)
    #for k in xrange(0, self.W):
      #print "u[{0}] = {1}".format(k, polyToHex(u[k],x, self.W))
      
    c = list(c)
    for i in xrange(c_m - 1, self.m - 1, -1):
      
      bit = len(c) - i - 1
      if c[bit] == '1':
        j = floor((i - self.m) / self.W)
        #j = j.mod(self.t)
        
        k = (i - self.m) - (self.W * j)

        j_aux = j
        # j_aux - j => word in uk
        # j_aux => word in C
        
        while (j_aux < c_t) and ((j_aux - j) < r_t):
          # C{j_aux} = C{j_aux} XOR u[k]
          uk = self.getWord(u[k], j_aux - j) # this is a str, not a list
          
          ci = self.getWord(c, j_aux) # warning: this is a list, not a str
          
          ci = bitwiseXor(ci, uk)
          
          end = len(c) - (j_aux * self.W)
          start = end - self.W
          
          c[start : end] = list(ci)
          
          j_aux += 1
    
    c = c[len(c) - (self.t * self.W) : ]
    return '0'*self.s + ''.join(c[self.s : ])

  # shift left by one bit
  # all we need to to is delete the left-most bit and add one 0 to to the right
  def shiftLeft(self, a):
    # delete the left-most bit
    toShift = list(a[1:]) # work with list instead of string
    toShift.append('0')
    # return as string    
    return ''.join(toShift)
    
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

  
  def poly_xgcd(self, a, b):
    if type(a) != type(self.var):
      a = binToPoly(a, self.var)
    if type(b) != type(self.var):
      b = binToPoly(b, self.var)
    a_m = a.degree()
    b_m = b.degree()
    # test degrees
    if a_m > b_m:
      print "Error xgcd: must have deg(a) <= deg(b)"
      return
    u = a
    v = b
    # poly g
    g = [1, 0]
    # poly h
    h = [0, 1]
    while u != 0:
      j = u.degree() - v.degree()
      if j < 0:
        (u, v) = (v,u)
        g.reverse()
        h.reverse()
        j = -j
      u = (u + (self.var**j * v))
      g[0] = (g[0] + (self.var**j * g[1]))
      h[0] = (h[0] + (self.var**j * h[1]))
    d = v
    # TODO: porque que e que precisa desta multiplicacao por x^-1 ??? :(
    g = g[1]
    h = h[1]
    return (d, g, h)
    

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

# convert a poly to an hex list
# x = x**233 + x**74 + 1 => []
def polyToHex(a, x, W=32):
  # convert to bin if necessary
  if type(a) == type(x):
    t = ceil(len(a.list()) / W)
    a = polyToBin(a, x).zfill(t * W)
  t = ceil(len(a) / W)
  a = a.zfill(t * W)
  start = len(a) - W
  end = len(a)
  p = []
  nBytes = W/4
  for i in xrange(t-1, -1, -1):
    h = hex(Integer(a[start : end], 2))
    p.append(h.zfill(nBytes))
    start -= W
    end -= W
  p.reverse()
  return p

def test_mod():
  R = PolynomialRing(GF(2), 'x')
  x = R.gen()
  f = x**532 + x + 1
  #f = x**27
  #f = x^127+x^8+x^7+x^3+1
  R = R.quotient(f, 'x')
  B = BinaryPolynomial(f, 32)
  a = R.random_element()
  
  l = a.list()
  # traverse in reversed order
  a = (''.join(str(l[bit]) for bit in xrange(len(l)-1, -1, -1)))
  
  b = R.random_element()
  
  l = b.list()
  # traverse in reversed order
  b = (''.join(str(l[bit]) for bit in xrange(len(l)-1, -1, -1)))
  
  a = binToPoly(a, B.var)
  b = binToPoly(b, B.var)
  
  c = a*b

  #return B.binToPoly(B.polyMod(c)) == c.mod(f)
  fi = x^127 + x^8 + x^7 + x^3 + 1
  #fi = x**12 + x + 1
  B2 = BinaryPolynomial(fi, 32)
  mod = binToPoly(B2.polyMod(c), B.var)
  return mod == c.mod(fi)
