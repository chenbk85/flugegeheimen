#include "stdafx.h"
#include "kernel/kernel.h"
#include "helpers/socket.h"
#include "helpers/xml.h"


class RequestBuilder {
public:
	RequestBuilder() {

	};

	virtual ~RequestBuilder() {

	};

	template<class T>
	RequestBuilder &operator<<(const T &field) {
		for (size_t i = 0; i < sizeof(T); i++) {
			m_data.push_back(((const uint8_t *) &field)[i]);
		}
		return *this;
	};

	void flush() {
		m_data.clear();
	};

	const char *data() {
		return reinterpret_cast<const char *>(m_data.data());
	};

	size_t size() {
		return m_data.size();
	};
protected:
	std::vector<uint8_t> m_data;
private:
};

class ResponseParser {
public:
	ResponseParser(const char *data, size_t size) :
			m_data(reinterpret_cast<const uint8_t *>(data)), m_size(size), m_offset(0) {

	};

	ResponseParser();

	virtual ~ResponseParser() {

	};

	template<class T>
	ResponseParser &operator>>(T &field) {
		if (m_size - m_offset < sizeof(T))
			throw std::runtime_error("ResponseParser: not enough data to parse");
		for (size_t i = 0; i < sizeof(T); i++) {
			((uint8_t *) &field)[i] = m_data[m_offset + i];
		}
		m_offset += sizeof(T);
		return *this;
	}

	void flush() {
		m_offset = 0;
	};

	void setData(const char *data, size_t size) {
		m_data = reinterpret_cast<const uint8_t *>(data);
		m_size = size;
		m_offset = 0;
	};
protected:
	const uint8_t *m_data;
	size_t m_size;
	size_t m_offset;
private:
};

int main(int argc, char **argv) {
	if (argc < 2) {
		Flug::Kernel kern;
		kern.loadConfig("res/config.xml");
		kern.main();
	} else if (std::string(argv[1]) == "--timer") {
		Flug::Socket sock;
		sock.connect("192.168.0.93", "2001");
		RequestBuilder req;
		Flug::Xml config;
		config.loadData("res/config.xml");
		for (size_t i = 0; i < 8; i++) {
			std::string chanNo;
			std::stringstream ss;
			ss << i;
			ss >> chanNo;
			int chanDelay;
			config.get("/flugegeheimen/timer/channel_" + chanNo + "@delay", chanDelay);

			char res[4];
			req.flush();
			req << ((uint8_t) 0x80) << ((uint8_t) i) << ((uint8_t) 0x2) << ((uint16_t) chanDelay);
			sock.send(req.data(), req.size());

			sock.recv(reinterpret_cast<char *>(&res), sizeof(res));
			ResponseParser rparse(res, 4);
			uint8_t numReg, cmd;
			uint16_t val;
			rparse >> numReg >> cmd >> val;
			printf("Reg: %4d, Cmd: %4d, Val: % 6d\n", numReg, cmd, val);

		}
		sock.disconnect();
	} else {
		std::cout << "Wrong option " << argv[1] << std::endl;
		return 1;
	}
	return 0;
}
