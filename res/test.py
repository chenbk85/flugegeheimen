__author__ = 'user'

import json


def InitModule():
	print ("PYTHON MODULE WORKS!")
	q = Foo()
	q.func()

def HandleRequest(s):
	req = json.loads(s)
	req['asdf'] = 'asdfas'
	return json.dumps(req)

InitModule()
