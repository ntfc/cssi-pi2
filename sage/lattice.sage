

def genMatrix(n,m,q):
	matx = random_matrix(ZZ, n, m, x=0, y=q)
	return matx


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
	randvector = random_vector(ZZ, m)
	sumpZ = sum(map(lambda x : dist.distribution_function(x), randvector))
	randvector = randvector.apply_map(lambda x: dist.get_random_element()/sumpZ)
	return randvector.transpose()




