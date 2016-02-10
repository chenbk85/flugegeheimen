//
// Created by user on 10.02.16.
//

#include "../stdafx.h"
#include "ModulesTable.h"
#include "LocalDispatcher.h"

namespace Flug {

    ModulesTable::ModulesTable() {

    }

    ModulesTable &ModulesTable::getInstance() {
        static ModulesTable inst;
        return inst;
    }

    ModulesTable::~ModulesTable() {

    }

    void ModulesTable::setLocalDispatcherPtr(LocalDispatcher *ptr) {
        m_local = ptr;
    }

    Module *ModulesTable::getLocalModule(const std::string &modName) {
        if (!m_local) {
            m_local = LocalDispatcherPtr::get().getPointer();
            if (!m_local) {
                throw std::runtime_error("NULL LocalDispatcher pointer");
            }
        }

        return m_local->getModule(modName);
    }
}

