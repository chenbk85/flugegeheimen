//
// Created by user on 10.02.16.
//

#pragma once

namespace Flug {

    class PythonContext {
    public:
        boost::python::api::object & getMainModule ();
        boost::python::api::object & getNamespace ();

        static PythonContext & getInstance ();
        virtual ~PythonContext ();
    private:
        PythonContext ();

        boost::python::api::object m_mainModule;
        boost::python::api::object m_globalContext;
    };

    static std::string LocalRequest(const std::string & request,
                                    const std::string & sender);

}

