//
// Created by user on 10.02.16.
//

#ifndef PROJECT_MODULESTABLE_H
#define PROJECT_MODULESTABLE_H

#include "Module.h"

namespace Flug {
class LocalDispatcher;
class RemoteDispatcher;
class ModulesTable {
public:
    virtual ~ModulesTable();
    static ModulesTable & getInstance();
    void setLocalDispatcherPtr (LocalDispatcher * ptr);
    void setRemoteDispatcherPtr (RemoteDispatcher * ptr);

    Module * getLocalModule (const std::string & modName);

private:
    LocalDispatcher * m_local;
    RemoteDispatcher * m_remote;
    ModulesTable();
};
}


#endif //PROJECT_MODULESTABLE_H
