//
// Created by user on 28.10.15.
//

#ifndef PROJECT_ARCHIVEMODULE_H
#define PROJECT_ARCHIVEMODULE_H

#include "Module.h"
#include "JsonBson.h"
#include "MongoClient.h"
#include "InterlockedArchiveBackend.h"

namespace Flug {
    class ArchiveModule : public Module {
    public:

        ArchiveModule() = delete;

        ArchiveModule(InterlockedArchiveBackend * backend);

        virtual ~ArchiveModule();

        virtual bool initModule();

        virtual bool destroyModule();

        virtual bool handleRequest(Request &req, Response &resp);

        virtual bool loadConfig (Json::Value & config);

    protected:

        bool handleQuery (Request &req, Response &resp);

        InterlockedArchiveBackend * m_backend;

    };
}


#endif //PROJECT_ARCHIVEMODULE_H
