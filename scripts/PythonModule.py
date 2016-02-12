import json
import math
import time

ModuleName = 'PythonModule'
ArchiveName = 'ThomsonScattering'
DatastoreName = 'datastore_' + ArchiveName
CrateModule = 'NskCrate1'

def GetDate():
	return time.strftime("%Y-%m-%d")
def GetTime():
	return time.strftime("%H:%M:%S")

def RegisterDatabase():
	print ("Registering python module datastore")
	registrationReq = {'subsystem': 'database',
		'reqtype': 'registerDatastore',
		'name': ArchiveName}

	mod = Module()
	ret = mod.LocalRequest(json.dumps(registrationReq), ModuleName)
	resp = json.loads(ret)

	if resp['status'] == 'success':
		print ('Datastore registered')
	else:
		print ('Datastore registration failed')


def InitModule():
	RegisterDatabase()

def ProxySimpleRequest(req, target):
	req['subsystem'] = target
	return Module().LocalRequest(json.dumps(req), ModuleName)

def ProxyRequest(request):
	return ProxySimpleRequest(request, CrateModule);

def ArchiveData(data):
	data['date'] = GetDate();
	data['time'] = GetTime();
	req = {'subsystem': 'database',
		'reqtype': 'insert',
		'datastore': DatastoreName,
		'data': data}
	Module().LocalRequest(json.dumps(req), ModuleName)

def HandleDownloadData(req):
	subr = {'reqtype': 'downloadData', 'subsystem': CrateModule}
	sdata = Module().LocalRequest(json.dumps(subr), ModuleName)
	data = json.loads(sdata)
	if data['status'] == 'success':
		ArchiveData(data)
	return sdata

DispatchingTable = {
	'req': HandleDownloadData,
	'downloadData': HandleDownloadData,
	'waitForTrigger': ProxyRequest,
	'softStart': ProxyRequest,
	'calibrate': ProxyRequest,
	'getAdcsList': ProxyRequest,
	'ping': ProxyRequest,
	'setPagesCount': ProxyRequest}


def HandleRequest(s):
	req = json.loads(s)
	return DispatchingTable[req['reqtype']](req)


InitModule()
