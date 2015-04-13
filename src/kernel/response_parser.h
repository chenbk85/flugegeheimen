#pragma once


namespace Flug {

	class ResponseParser {
	public:
		ResponseParser(const char *data, size_t size);

		ResponseParser();

		virtual ~ResponseParser();

		template<class T>
		ResponseParser &operator>>(T &field) {
			if (m_size - m_offset < sizeof(T))
				throw std::runtime_error("ResponseParser: not enough data to parse");
			for (size_t i = 0; i < sizeof(T); i++) {
				((uint8_t * ) & field)[i] = m_data[m_offset + i];
			}
			m_offset += sizeof(T);
			return *this;
		};

		void flush();

		void setData(const char *data, size_t size);

	protected:
		const uint8_t *m_data;
		size_t m_size;
		size_t m_offset;
	private:
	};
}
