//
// Created by user on 13.10.15.
//

#ifndef PROJECT_CONFIGURABLE_H
#define PROJECT_CONFIGURABLE_H


namespace Flug {
    class Configurable {
    public:
        void getConfigTag (std::string & str);
    protected:
        Configurable() = delete;

        Configurable(const std::string &name);
        virtual ~Configurable();

        Json::Value & getConfig ();

        Json::Value m_configuration;

    private:
        std::string m_configTag;
    };
}


#endif //PROJECT_CONFIGURABLE_H
