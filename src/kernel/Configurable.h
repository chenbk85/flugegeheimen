//
// Created by user on 13.10.15.
//

#pragma once


namespace Flug {
    class Configurable {
    public:
    protected:
        Configurable() = delete;

        Configurable(const std::string &name);

        virtual ~Configurable();

        Json::Value m_configuration;

        const std::string & getConfigTag ();

    private:
        std::string m_configTag;
    };
}


