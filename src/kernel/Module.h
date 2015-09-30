//
// Created by user on 9/29/15.
//

#pragma once

namespace Flug {
	class Module : public std::thread {
	public:
		virtual bool initModule () = 0;
		virtual bool destroyModule () = 0;
		virtual bool handleRequest (const std::string & request, std::string & response) = 0;

		Module ();
		virtual ~Module();

		bool pushRequest (const std::string & req);
		bool popResponse (std::string & resp);
	protected:
		//There I wanna have something like two (incoming & outgoing)
		//spsc message queues
		void moduleProc ();
		void mainLoop ();
		void handleIncomingRequests ();

		bool m_quitState;
		boost::lockfree::spsc_queue<std::string> m_inQueue;
		boost::lockfree::spsc_queue<std::string> m_outQueue;
	private:
	};
}

