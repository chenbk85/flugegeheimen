__author__ = 'user'


def test():
	print ("PYTHON MODULE WORKS!");
	hello = file('res/test.txt', 'w')
	hello.write('hello, world')
	hello.close()

test()
