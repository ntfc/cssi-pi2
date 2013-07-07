import sys
from sage.all import *

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

  # a and b must be in binary form
  # C is returned in binary form
  def polyAddition(self, a, b):
    if type(a) != str or type(b) != str:
      print "Polynomials must be in binary form!"
      return
    if len(a) != len(b):
      print "INFO: Polynomials must be of same length"
      return
    return bitwiseXor(a, b)

  """# return the multiplication between a and b, not modulo!!
  def polyMult(self, a, b):
    if type(a) != str or type(b) != str:
      #print "Polynomials must be in binary form! Converting to poly form.."
      a = polyToBin(a).zfill(self.t * self.W)
      b = polyToBin(b).zfill(self.t * self.W)
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
        b = shiftLeft(b)
    # return as str
    return ''.join(C)"""
    
  # supports polynomials of arbitrary length
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
          if int(self.getBit(a, j, k, self.W)) == 1:
            i = j
            while (i < c_t) and (i-j <= b_t):
              # C[i] = C[i] XOR B[i-j]
              Ci = self.getWord(C, i, self.W)
              Ci = bitwiseXor(Ci, self.getWord(b, i-j, self.W))
              # save Ci to C[i]
              end = len(C) - (self.W * i)
              start = end - self.W
              C[start : end] = list(Ci)
              i += 1
      if k != (self.W - 1):
        b = shiftLeft(b)
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
    r_t = ceil(r.degree() / self.W) + 1
    # pre computation
    u = []
    u.append(polyToBin(r, self.var).zfill(r_t * self.W))
    for k in xrange(1, self.W):
      u.append(shiftLeft(u[k - 1]))
      
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
          uk = self.getWord(u[k], j_aux - j, self.W) # this is a str, not a list
          
          ci = self.getWord(c, j_aux, self.W) # warning: this is a list, not a str
          
          ci = bitwiseXor(ci, uk)
          
          end = len(c) - (j_aux * self.W)
          start = end - self.W
          
          c[start : end] = list(ci)
          
          j_aux += 1
    
    c = c[len(c) - (self.t * self.W) : ]
    return '0'*self.s + ''.join(c[self.s : ])

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
      T = self.getWord(A, i, self.W)
      
      word = i - 8
      Ci = self.getWord(A, word, self.W)
      # C[i - 8] = C[i - 8] XOR (T << 23)
      Ci = self.polyFastModXorAndShift(Ci, T, 23, 0)
      # save Ci to A[i-8]
      end = len(A) - (W * word)
      start = end - W
      A[start : end] = list(Ci)
      
      word = i - 7
      Ci = self.getWord(A, word, self.W)
      # C[i - 7] = C[i - 7] XOR (T >> 9)
      Ci = self.polyFastModXorAndShift(Ci, T, 9, 1)
      # save Ci to A[i-7]
      end = len(A) - (W * word)
      start = end - W
      A[start : end] = list(Ci)
      
      word = i - 5
      Ci = self.getWord(A, word, self.W)
      # C[i - 5] = C[i - 5] XOR (T << 1)
      Ci = self.polyFastModXorAndShift(Ci, T, 1, 0)
      # save Ci to A[i-5]
      end = len(A) - (W * word)
      start = end - W
      A[start : end] = list(Ci)
      
      word = i - 4
      Ci = self.getWord(A, word, self.W)
      # C[i - 4] = C[i - 4] XOR (T >> 31)
      Ci = self.polyFastModXorAndShift(Ci, T, 31, 1)
      # save Ci to A[i-4]
      end = len(A) - (W * word)
      start = end - W
      A[start : end] = list(Ci)
    
    T = self.getWord(A, 7, self.W)
    # T = C[7] >> 9
    toShift = 9
    while toShift > 0:
      T = shiftRight(T)
      toShift -= 1
    
    word = 0
    Ci = self.getWord(A, word, self.W)
    # C[0] = C[0] XOR T
    Ci = self.polyFastModXorAndShift(Ci, T, 0, 0)
    end = len(A) - (W * word)
    start = end - W
    A[start : end] = list(Ci)
      
    word = 2
    Ci = self.getWord(A, word, self.W)
    # C[2] = C[2] XOR (T << 10)
    Ci = self.polyFastModXorAndShift(Ci, T, 10, 0)
    end = len(A) - (W * word)
    start = end - W
    A[start : end] = list(Ci)
    
    word = 3
    Ci = self.getWord(A, word, self.W)
    # C[3] = C[3] XOR (T >> 22)
    Ci = self.polyFastModXorAndShift(Ci, T, 22, 1)
    end = len(A) - (W * word)
    start = end - W
    A[start : end] = list(Ci)
    
    word = 7
    Ci = self.getWord(A, word, self.W)
    # C[7] = C[7] & 0x1FF
    Ci = bitwiseAnd(Ci, ('1'*9).zfill(32))
    end = len(A) - (W * word)
    start = end - W
    A[start : end] = list(Ci)
    
    # return C[7], C[6], .. C[0]
    return ''.join(str(bit) for bit in A[ (t*2 *W) - (t * W) : ])
    
  """ NOTA: Isto foi feito a pressa apenas para testar """
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

      T = self.getWord(A, i, self.W)

      word = i - 17
      Ci = self.getWord(A, word, self.W)
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
      Ci = self.getWord(A, word, self.W)
      #### C[i - 16] = C[i - 16] XOR (T >> 20) XOR (T << 19)
      # C[i - 16] = C[i - 16] XOR (T >> 20)
      Ci = self.polyFastModXorAndShift(Ci, T, 20, 1)
      # C[i - 16] = C[i - 16] XOR (T >> 19)
      Ci = self.polyFastModXorAndShift(Ci, T, 19, 1)
      # save Ci to A[i-16]
      end = len(A) - (W * word)
      start = end - W
      A[start : end] = list(Ci)
      
    T = self.getWord(A, 16, self.W)
    """# T = C[16] >> 20
    toShift = 20
    while toShift > 0:
      T = shiftRight(T)
      toShift -= 1"""
      
    word = 0
    Ci = self.getWord(A, word, self.W)
    # C[0] = C[0] XOR (T >> 20)
    Ci = self.polyFastModXorAndShift(Ci, T, 20, 1)
    # C[0] = C[0] XOR (T >> 19)
    Ci = self.polyFastModXorAndShift(Ci, T, 19, 1)
    end = len(A) - (W * word)
    start = end - W
    A[start : end] = list(Ci)
    
    word = 16
    Ci = self.getWord(A, word, self.W)
    # C[16] = C[16] & 0xFFFF
    Ci = bitwiseAnd(Ci, ('1'*20).zfill(32))
    end = len(A) - (W * word)
    start = end - W
    A[start : end] = list(Ci)
    
    # return C[16], C[15], .. C[0]
    return ''.join(str(bit) for bit in A[ (t*2 *W) - (t * W) : ])

  
  def polyXgcd(self, a, b):
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
      z = self.var**j
      
      u = (u + (z * v))
      
      #if u.degree() >= b.degree():
        #u = u.mod(b)
      g[0] = (g[0] + (z * g[1]))
      
      #if g[0].degree() >= b.degree():
        #g[0] = g[0].mod(b)
      h[0] = (h[0] + (z * h[1]))
      
      #if h[0].degree() >= b.degree():
        #h[0] = h[0].mod(b)
    d = v
    
    g = g[1]
    h = h[1]
    return (d, g, h)
    
  def crtToPoly(self, ai, fi):
    if type(ai) != list or type(fi) != list or len(ai) != len(fi):
      print "Error: type errors"
      return
    ai = map(lambda b : polyToBin(b, self.var), ai)
    fi = map(lambda b : polyToBin(b, self.var), fi)
    N = '0'*len(ai[0])
    for b in fi:
      N = self.polyMultV2(N, b)
    print N

""" Auxiliary functions """
# convert a sage expression to a polynomial
def expressionToPoly(e, var):
  p = 0*var
  for op in e.coeffs():
    p += var**op[1]
  return p

def bitwiseAnd(a, b):
  if len(a) != len(b):
    print "String must be of same size"
    return
  c = ''.join(map(lambda x,y : str(int(x).__and__(int(y))), a, b))
  return c

# perform bitwise xor on two bins
# lists can have different length
def bitwiseXor(a, b):
  lenA = len(a)
  lenB = len(b)
  if len(a) >= len(b):
    diff = lenA - lenB
    c = a[:diff]
    c += ''.join(str(e) for e in map(lambda x,y : int(x).__xor__(int(y)), a[diff:], b))
  else:
    diff = lenB - lenA
    c = b[:diff]
    c += ''.join(str(e) for e in map(lambda x,y : int(x).__xor__(int(y)), a, b[diff:]))
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

# convert a binary string to a polynomial
# [1,0,1,1,1] = x^4 + x^2 + x + 1
def binToPoly(b, var):
  # TODO: validate types
  # TODO: var must be sage.rings.polynomial.polynomial_gf2x.Polynomial_GF2X
  v = 0*var
  n = len(b)
  for i in xrange(0, n):
    if int(b[i]) == 1:
      v += var**(n - i - 1)
  return v

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

# set the word i. the new value is then returned
def setWord(A, i, new_word, W):
  if i >= (len(A) // W) or i < 0:
    print "Error: cant found {0}-th word. Allowed: 0 .. {1}".format(i, (len(A) // W)-1)
    return
  if len(new_word) != W:
    print "Wrong word length. Found {0}, needed {1}".format(len(new_word), W)
  start = len(A) - (W * i)
  end = start - W

  C = A[ : end]
  C += new_word
  C += A[start : ]
  return C
  
# get the k-th bit of the j-th W-bit word of polynomial a
def getBit(self, a, j, k, W):
  # TODO: validate j and k and a
  # j-th word of polynomial begins at position len(a) - (W*j) - 1
  wordStart = len(a) - (W * j) - 1
  # k-th bit is at position wordStart - k
  bitPosition = wordStart - k
  return a[bitPosition] 

# get the i-th W-bit word from an array
def getWord(A, i, W):
  if i >= (len(A) // W) or i < 0:
    print "Error: cant found {0}-th word. Allowed: 0 .. {1}".format(i, (len(A)//W)-1)
    return
  start = len(A) - (W * i)
  end = start - W
  return A[end : start]

# reduces a polynomial to its CRT form
# a: polynomial to reduce
# fi: list of modulus
def reduceToCRT(a, fi):
  return map(lambda f : a.mod(f), fi)

# f1 and f2 must be in CRT form
# result in CRT form
def multCRT(p1, p2, fi):
  p1zip = zip(p1, fi)
  p2zip = zip(p2, fi)

  # returns a list with all multiplications done modulo each fi
  # CRT_list(return value, fi) == (p1*p2).mod(fi)
  return map(lambda ((x1, y1), (x2, y2)) : (x1 * x2).mod(y1), zip(p1zip, p2zip))

# p1 and p2 must be in CRT form
# result in CRT form
def addCRT(p1, p2, fi):
  pAddZip = zip(p1,p2)

  return map(lambda (x,y,z) : (x+y).mod(z), zip(p1, p2, fi))
