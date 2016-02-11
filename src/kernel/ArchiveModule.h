//
// Created by user on 28.10.15.
//

#ifndef PROJECT_ARCHIVEMODULE_H
#define PROJECT_ARCHIVEMODULE_H

#include "Module.h"
#include "JsonBson.h"
#include "MongoClient.h"
#include "ArchiveBackend.h"

namespace Flug {
    class ArchiveModule : public Module {
    public:

        ArchiveModule() = delete;

        ArchiveModule(ArchiveBackend *backend);

        virtual ~ArchiveModule();

        virtual bool initModule();

        virtual bool destroyModule();

        virtual bool rebootModule();

        virtual bool handleRequest(Request &req, Response &resp);

        virtual bool loadConfig(Json::Value &config);

    protected:

        std::string getDatastoreName (const std::string & name);

        bool handleFindUnique(Request &req, Response &resp);

        bool handleGetDatastoresList(Request &req, Response &resp);

        bool handleRegisterDatastore(Request &req, Response &resp);

        bool handleInsert(Request &req, Response &resp);

        ArchiveBackend *m_backend;

    };
}


#endif //PROJECT_ARCHIVEMODULE_H
