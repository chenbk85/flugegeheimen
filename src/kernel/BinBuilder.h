//
// Created by user on 10/12/15.
//

#ifndef PROJECT_BINBUILDER_H
#define PROJECT_BINBUILDER_H


namespace Flug {
	class BinBuilder {
	public:
		BinBuilder ();
		virtual ~BinBuilder();

		void flush ();
		void getData (std::vector<uint8_t> & data);
		void setData (const uint8_t * ptr, size_t size);

		template <class T> BinBuilder & operator<< (const T & data) {
			const uint8_t * ptr = reinterpret_cast<const uint8_t *>(&data);
			for (size_t iter = 0; iter < sizeof(T); iter++) {
				m_data.push_back(ptr[iter]);
			}
			return *this;
		};

		template <class T> BinBuilder & operator>> (T & data) {
			if (m_data.size() < sizeof(T)) {
				throw std::runtime_error("Not enough data in BinBuilder");
			}
			data = *(reinterpret_cast<T *>(m_data.data()));
			m_data.erase(m_data.begin(), m_data.begin() + sizeof(T));
			return *this;
		};

	protected:
		std::vector<uint8_t> m_data;
	private:
	};
}


#endif //PROJECT_BINBUILDER_H
