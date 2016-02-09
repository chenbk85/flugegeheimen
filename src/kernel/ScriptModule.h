//
// Created by user on 08.02.16.
//

#pragma once

#include "Module.h"

namespace Flug {
class ScriptModule : public Module {
public:
    virtual bool initModule () = 0;
    virtual bool destroyModule () = 0;
    virtual bool handleRequest (Request & req, Response & resp) = 0;
    virtual bool rebootModule () = 0;

    ScriptModule() = delete;
    ScriptModule(const std::string & configName);
    virtual  ~ScriptModule();

    virtual void loadScript (const std::string & path) = 0;
protected:
private:
};
}

