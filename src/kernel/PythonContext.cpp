//
// Created by user on 10.02.16.
//

#include "../stdafx.h"
#include "PythonContext.h"

namespace  Flug {

    PythonContext::PythonContext() {
        Py_Initialize();

        m_mainModule = boost::python::import("__main__");
        m_globalContext = m_mainModule.attr("__dict__");
    }

    PythonContext::~PythonContext() {
        Py_Finalize();
    }

    PythonContext &PythonContext::getInstance() {
        static PythonContext _inst;
        return _inst;
    }

    boost::python::api::object &PythonContext::getNamespace() {
        return m_globalContext;
    }

    boost::python::api::object& PythonContext::getMainModule() {
        return m_mainModule;
    }


    static std::string LocalRequest(const std::string & request,
                                    const std::string & sender) {
        //find module ptr by sender name
        //call ptr.pyLocalRequest
    }
}



