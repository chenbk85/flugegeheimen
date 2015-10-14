//
// Created by user on 13.10.15.
//

#include "../stdafx.h"
#include "Configurable.h"


namespace Flug {
    Configurable::Configurable(const std::string & name) :
    m_configTag(name) {
    }

    Configurable::~Configurable() {
    }

    void Configurable::getConfigTag(std::string &str) {
        str = m_configTag;
    }

    Json::Value& Configurable::getConfig() {
        return m_configuration;
    }
}