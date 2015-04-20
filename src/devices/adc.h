#pragma once

#include "../helpers/xml.h"
#include "../helpers/socket.h"
#include "../helpers/binres.h"

namespace Flug {
	namespace Nsk {
		class Adc {
		public:

			static const size_t ADC_PAGE_SIZE = 0x800;
			static const uint8_t ADC_CONTROLL_CMD = 0x80;
			enum Register {ADC_REG_OFFSET = 0x5, ADC_REG_ADDR = 0x6, ADC_REG_PAGES_NUM = 0x7, ADC_REG_PLL = 0x2,
				ADC_REG_CLEAR = 0x1, ADC_REG_SOFT_START = 0x3, ADC_REG_ENABLE_START = 0x4};
			enum RegOperstion {ADC_REGOP_READ = 0x1, ADC_REGOP_WRITE = 0x2};

			Adc ();
			virtual ~Adc ();

			void loadConfig (const std::string & xmlPath, size_t deviceNo);
			void connect ();
			void disconnect ();

			void initDevice ();
			void loadFirmware ();
			void setRegister (Register reg, RegOperstion regOp, uint8_t valLo, uint8_t valHi);
			void setRegister (Register reg, RegOperstion regOp, uint16_t val);
			uint16_t getRegister (Register reg);

			void clearRegisters ();
			void initPll ();
			void allowStart ();
			void softStart ();
			void setCounter (uint16_t counter);
			void setOffset (uint16_t offset);
			void setPagesNumber (uint16_t pages);

			void readPage (char * buf);

		protected:
			std::string m_addr;
			std::string m_dataPort;
			std::string m_controlPort;
			std::string m_firmwarePath;

			Flug::Xml m_config;
			Flug::Socket m_controlSocket;
			Flug::Socket m_dataSocket;
			Flug::BinRes m_firmware;
		private:
		};
	}
}