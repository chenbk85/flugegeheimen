//
// Created by user on 9/29/15.
//

#pragma once

#include "Response.h"
#include "Request.h"
#include "Configurable.h"

namespace Flug {
	class Module : public Configurable {
	public:

		virtual bool initModule () = 0;
		virtual bool destroyModule () = 0;
		virtual bool handleRequest (Request & req, Response & resp) = 0;

		Module () = delete;
		Module (const std::string & configName);
		virtual ~Module();
		void start ();

		bool pushRemoteRequest(Request &req);
		bool popRemoteResponse(Response &resp);

        bool pushLocalRequest(Request &req);
        bool popLocalRequest(Request &req);

        bool popLocalResponse(Response &resp);
        bool pushLocalResponse(Response &resp);



		bool handleRequestWraper (Request & req, Response & resp);
	protected:
		//There I wanna have something like two (incoming & outgoing)
		//spsc message queues
		void moduleProc ();
		void mainLoop ();
		void handleIncomingRequests ();
        void localRequest (Request &req, Response &resp);
        void localMultiRequest (std::vector<Request> &reqs,
                                std::vector<Response> &resp);

		bool m_quitState;

        std::atomic<uint32_t> m_inQueueCounter;
        std::condition_variable m_inQueueCv;
        std::mutex m_inQueueMutex;

        std::atomic<uint32_t> m_locRespCounter;
        std::condition_variable m_locRespCv;
        std::mutex m_locRespMutex;

		boost::lockfree::spsc_queue<Request,
                boost::lockfree::capacity<1024> > m_remoteInQueue;

		boost::lockfree::spsc_queue<Response,
                boost::lockfree::capacity<1024> > m_remoteOutQueue;

        boost::lockfree::spsc_queue<Request,
                boost::lockfree::capacity<1024> > m_localInQueue;

        boost::lockfree::spsc_queue<Response,
                boost::lockfree::capacity<1024> > m_localOutQueue;

        boost::lockfree::spsc_queue<Response,
                boost::lockfree::capacity<1024> > m_localResponsesQueue;

        boost::lockfree::spsc_queue<Request,
                boost::lockfree::capacity<1024> > m_localRequestsQueue;

		std::thread * m_thread;
	private:
	};
}
