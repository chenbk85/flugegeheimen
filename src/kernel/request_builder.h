#pragma once

namespace Flug {
	class RequestBuilder {
	public:

		RequestBuilder();
		virtual ~RequestBuilder();
		const uint8_t * data() const;
		size_t size() const;
		void flush ();

		template<class T>
		RequestBuilder &operator<<(const T &field) {
			for (size_t i = 0; i < sizeof(T); i++) {
				m_data.push_back(((const uint8_t *) &field)[i]);
			}
			return *this;
		};

		RequestBuilder &push (const uint8_t * data, size_t size);


	protected:
		std::vector <uint8_t> m_data;
	private:
	};
}