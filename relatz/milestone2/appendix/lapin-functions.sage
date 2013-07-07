""" Aux methods """
def bernoulli(tau):
  return int(random() < tau)

# p: poly, var=x, fill=length in binary
def polyToBin(p, var, fill=0):
  l = p.list()
  # traverse in reversed order
  return (''.join(str(l[bit]) for bit in xrange(len(l)-1, -1, -1))).zfill(fill)
  
# convert a binary string to a polynomial
# [1,0,1,1,1] = x^4 + x^2 + x + 1
def binToPoly(b, var):
  v = 0*var
  n = len(b)
  for i in xrange(0, n):
    if int(b[i]) == 1:
      v += var**(n - i - 1)
  return v

def intToBin(i, fill=0):
  return ''.join(bin(i)[2:]).zfill(fill)

# converts a binary string to int
def binToInt(i):
  return Integer(i, 2)

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