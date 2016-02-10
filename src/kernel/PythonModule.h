//
// Created by user on 04.02.16.
//

#pragma once


#include "ScriptModule.h"



namespace Flug {
    using namespace boost::python;

    class PythonModule : public ScriptModule {
    public:
        virtual bool initModule ();
        virtual bool destroyModule ();
        virtual bool handleRequest (Request & req, Response & resp);
        virtual bool rebootModule ();

        PythonModule() = delete;
        PythonModule(const std::string & configName);
        virtual  ~PythonModule();

        virtual void loadScript (const std::string & path);

        std::string parsePythonException ();

        std::string pyLocalRequest (const std::string & req);
    protected:
    private:
        std::string m_scriptPath;
    };
}
