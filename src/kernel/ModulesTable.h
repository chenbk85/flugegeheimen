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
    ModulesTable & getInstance();
    void setLocalDispatcherPtr (LocalDispatcher * ptr);
    void setRemoteDispatcherPtr (RemoteDispatcher * ptr);

private:
    LocalDispatcher * m_local;
    RemoteDispatcher * m_remote;
};
}


#endif //PROJECT_MODULESTABLE_H
