__author__ = 'user'

import json


def InitModule():
	print ("PYTHON MODULE WORKS!")

def HandleRequest(s):
	req = json.loads(s)
	subr = {'reqtype': 'getDataSync', 'subsystem': 'DummyCrate1'}
	subreq = json.dumps(subr)
	mod = Module()
	return mod.LocalRequest(subreq, 'PythonModule')

InitModule()
