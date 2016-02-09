//
// Created by user on 04.02.16.
//

#include "../stdafx.h"
#include "PythonModule.h"

namespace  Flug {


    PythonModule::PythonModule(const std::string &configName) :
    ScriptModule(configName){

    }

    bool PythonModule::initModule() {
        boost::python::api::object main_module = boost::python::import("__main__");
        boost::python::api::object global(main_module.attr("__dict__"));
        try {
            boost::python::exec_file(m_scriptPath.c_str(), global, global);
        } catch (const boost::python::error_already_set & ex) {
            PyErr_Print();
        }
    }

    bool PythonModule::destroyModule() {
    }


    PythonModule::~PythonModule() {

    }

    bool PythonModule::handleRequest(Request &req, Response &resp) {
        boost::python::str pyReq = req.m_string.c_str();
        boost::python::str pyResp;

        boost::python::object main_module = boost::python::import("__main__");
        boost::python::object global(main_module.attr("__dict__"));
        boost::python::object handlingFunction = global["callable"];
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
}