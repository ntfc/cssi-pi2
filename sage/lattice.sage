#TODO:
#falta o q - random_element(q)
#o mod nao esta direito


def genMatrix(n,m):
	matx = MatrixSpace(ZZ, n, m)
	return matx.random_element()


def matrixMult(a, s, q):
	if(a.ncols() != s.nrows()):
		print 'error dimensions'
		return
	return (a*s).mod(q)


def matrixAdd(a, s):
	if(a.dimensions() != s.dimensions()):
		print 'error dimensions'
		return
	return a+s


def genVectorY(sigma, m):
	dist = RealDistribution('gaussian', sigma)
	randvector = random_vector(m)
	sumpZ = sum(map(lambda x : dist.distribution_function(x), randvector))
	randvector = randvector.apply_map(lambda x: dist.get_random_element()/sumpZ)
	return randvector.transpose()




