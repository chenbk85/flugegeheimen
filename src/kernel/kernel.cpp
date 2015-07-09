#include "../stdafx.h"
#include "kernel.h"
#include "../helpers/xml.h"
//#include "../config.h"

namespace Flug {


	Kernel::Kernel() {
	}

	Kernel::~Kernel() {
	}

	void Kernel::main() {
		m_gateway.bind(m_gatewayPort);
		while (1) {
			m_gateway.listen();
			/*Socket tmpSocket = m_gateway.accept();
			if ((int)tmpSocket == -1) {
				throw std::runtime_error ("Accepted wrong socket");
			}
			if (m_sink == -1) {
				m_sink = tmpSocket;
			} else {
				tmpSocket.disconnect();
			}*/

			m_sink = m_gateway.accept();

			/*char buf[100];
			bool sinkSocketAlive = true;
			while (sinkSocketAlive) {
				try {
					m_sink.recv(buf, 4);
					if (!memcmp(buf, "ping", 4)) {
						m_sink.send("HOLY FUCK! IT WORKS!", sizeof("HOLY FUCK! IT WORKS!"));
					} else {
						m_sink.send("HOLY FUCK!", sizeof("HOLY FUCK!"));
					}
				} catch (const std::runtime_error & err) {
					sinkSocketAlive = false;
				}
			}*/

			std::string req;


			m_sink.recvString (req);
			if (req == "ping") {
				m_sink.sendString ("Yuppi! It works, you freaking whore.");
			} else {
				m_sink.sendString ("Something gone wrong: bad request.");
			}

			m_sink.disconnect();

		}
	}

	void Kernel::loadConfig(const std::string &confPath) {
		Xml xml;
		xml.loadData("res/config.xml");
		xml.get("/flugegeheimen/server/net@port", m_gatewayPort);
		std::cout << "Started Flugegeheimen on " << m_gatewayPort << std::endl;
	}

	void Kernel::openGateway () {
		m_gateway.bind(m_gatewayPort);
	}


}
