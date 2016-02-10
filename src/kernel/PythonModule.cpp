//
// Created by user on 04.02.16.
//

#include "../stdafx.h"
#include "PythonContext.h"
#include "PythonModule.h"


#define FG_REQUEST_HANDLER "HandleRequest"


class Foo {
public:
    static void testStaticFunc() {
        std::cout << "STATIC FUNCTION WORKS!" << std::endl;
    }
};


namespace  Flug {



    PythonModule::PythonModule(const std::string &configName) :
    ScriptModule(configName){

    }

    bool PythonModule::initModule() {

        PythonContext & ctx = PythonContext::getInstance();

        try {
            boost::python::object foo_class = boost::python::class_<Foo>("Foo")
                    .def("func", &Foo::testStaticFunc,
                         boost::python::return_value_policy<boost::python::manage_new_object>())
                    .staticmethod("func")  // **
                    ;

            PyObject * dictPtr = ctx.getNamespace().ptr();
            PyDict_SetItemString(dictPtr, "Foo", foo_class.ptr());

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

        boost::python::object main_module = boost::python::import("__main__");
        boost::python::object global(main_module.attr("__dict__"));
        boost::python::object handlingFunction = global[FG_REQUEST_HANDLER];
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



