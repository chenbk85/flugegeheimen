//
// Created by user on 04.02.16.
//

#include "../stdafx.h"
#include "PythonContext.h"
#include "ModulesTable.h"
#include "LocalDispatcher.h"
#include "PythonModule.h"


#define FG_REQUEST_HANDLER "HandleRequest"

namespace  Flug {

    class PythonExported {
    public:
        static std::string LocalRequest(const std::string & request, const std::string & sender) {
            std::cout << "Handling python local request from " << sender << std::endl;
            std::cout << request << std::endl;

            //ModulesTable & modTbl = ModulesTable::getInstance();
            //Module * senderMod = modTbl.getLocalModule(sender);
            Module * senderMod = LocalDispatcherPtr::get().getPointer()->getModule(sender);

            if (!senderMod) {
                throw std::runtime_error("Can't find the " + sender + " module (snder)");
            }

            Request req(request, senderMod);
            Response resp;

            senderMod->localRequest(req, resp);

            return resp.m_string;
        }
    };

    PythonModule::PythonModule(const std::string &configName) :
    ScriptModule(configName) {

    }

    bool PythonModule::initModule() {

        PythonContext & ctx = PythonContext::getInstance();

        try {
            boost::python::exec_file(m_scriptPath.c_str(), ctx.getNamespace(), ctx.getNamespace());
        } catch (const boost::python::error_already_set & ex) {
            PyErr_Print();
        }

        return true;
    }

    bool PythonModule::destroyModule() {
        return true;
    }


    PythonModule::~PythonModule() {

    }

    bool PythonModule::handleRequest(Request &req, Response &resp) {
        boost::python::str pyReq = req.m_string.c_str();
        boost::python::str pyResp;

        PythonContext & ctx = PythonContext::getInstance();
        boost::python::object ModuleProxy = boost::python::class_<PythonExported>("Module")
                .def("LocalRequest", &PythonExported::LocalRequest,
                     boost::python::return_value_policy<boost::python::return_by_value>())
                .staticmethod("LocalRequest")  // **
        ;

        PyObject * dictPtr = ctx.getNamespace().ptr();
        PyDict_SetItemString(dictPtr, "Module", ModuleProxy.ptr());
        boost::python::object handlingFunction = ctx.getNamespace()[FG_REQUEST_HANDLER];
        boost::python::object respPtr;

        try {
            respPtr = handlingFunction(pyReq);
        } catch (const boost::python::error_already_set & ex) {
            PyErr_Print();
        }

        std::string rsp = extract<std::string>(respPtr);
        resp.m_string = rsp;

        return true;
    }

    bool PythonModule::rebootModule() {
        return false;
    }

    void PythonModule::loadScript(const std::string &path) {
        m_scriptPath = path;
    }

    std::string PythonModule::parsePythonException() {
        PyObject *type_ptr = NULL, *value_ptr = NULL, *traceback_ptr = NULL;
        PyErr_Fetch(&type_ptr, &value_ptr, &traceback_ptr);
        std::string ret("Unfetchable Python error");
        if(type_ptr != NULL){
            boost::python::handle<> h_type(type_ptr);
            boost::python::str type_pstr(h_type);
            boost::python::extract<std::string> e_type_pstr(type_pstr);
            if(e_type_pstr.check())
                ret = e_type_pstr();
            else
                ret = "Unknown exception type";
        }
        if(value_ptr != NULL) {
            boost::python::handle<> h_val(value_ptr);
            boost::python::str a(h_val);
            boost::python::extract<std::string> returned(a);
            if(returned.check())
                ret +=  ": " + returned();
            else
                ret += std::string(": Unparseable Python error: ");
        }
        if(traceback_ptr != NULL){
            boost::python::handle<> h_tb(traceback_ptr);
            boost::python::object tb(boost::python::import("traceback"));
            boost::python::object fmt_tb(tb.attr("format_tb"));
            boost::python::object tb_list(fmt_tb(h_tb));
            boost::python::object tb_str(boost::python::str("\n").join(tb_list));
            boost::python::extract<std::string> returned(tb_str);
            if(returned.check())
                ret += ": " + returned();
            else
                ret += std::string(": Unparseable Python traceback");
        }
        return ret;
    }

    std::string PythonModule::pyLocalRequest(const std::string &reqStr) {
        Request req(reqStr, this);
        Response resp;

        localRequest(req, resp);

        return resp.m_string;
    }
}



