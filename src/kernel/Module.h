//
// Created by user on 9/29/15.
//

#pragma once

#include "Response.h"
#include "Request.h"

namespace Flug {
	class Module : public std::thread {
	public:
		virtual bool initModule () = 0;
		virtual bool destroyModule () = 0;
		virtual bool handleRequest (Request & req, Response & resp) = 0;

		Module ();
		virtual ~Module();

		bool pushRequest (Request & req);
		bool popResponse (Response & resp);
		bool handleRequestWraper (Request & req, Response & resp);
	protected:
		//There I wanna have something like two (incoming & outgoing)
		//spsc message queues
		void moduleProc ();
		void mainLoop ();
		void handleIncomingRequests ();

		bool m_quitState;
		boost::lockfree::spsc_queue<Request, boost::lockfree::capacity<1024> > m_inQueue;
		boost::lockfree::spsc_queue<Response, boost::lockfree::capacity<1024> > m_outQueue;
	private:
	};
}

