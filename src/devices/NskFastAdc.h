//
// Created by user on 23.10.15.
//

#ifndef PROJECT_NSKFASTADC_H
#define PROJECT_NSKFASTADC_H

#include "../kernel/DeviceDriver.h"

namespace  Flug {

    class NskFastAdc : public DeviceDriver {
    public:

        NskFastAdc() = delete;

        NskFastAdc(const std::string &deviceInstanceName, const std::string &devType);

        virtual ~NskFastAdc();

        virtual bool initModule();

        virtual bool destroyModule();

        virtual bool handleRequest(Request &req, Response &resp);

        virtual bool loadConfig(Json::Value &config);

    protected:

        void loadFirmware ();
        void getReadyForStart ();
        void readDataPage (uint16_t * data);
        uint16_t readAllPages ();

    private:

        uint16_t readRegister (uint8_t reg);
        void writeRegister (uint8_t reg, uint16_t val);

        void clearRegisters ();
        void initPll ();
        void allowStart ();
        void softStart ();
        void dropPage();

        void setOffset (uint16_t offset);
        void setPage(uint16_t address);
        void setDataPagesCount (uint16_t pages);

        uint16_t getPagesCount ();
        uint16_t getPage();
        uint16_t getOffset ();

        Socket m_controlSock;
        Socket m_infoSock;
        uint16_t m_pagesCount;

	bool m_manual;
        std::string m_addr;
        std::string m_controlPort;
        std::string m_infoPort;
        std::string m_firmwareLocation;
        std::vector<uint8_t> m_firmware;
        std::vector<uint16_t> m_data;

        bool handleGetData(Request &req, Response &resp);

        bool handleSetPagesCount(Request &req, Response &resp);

        bool handleSoftStart (Request &req, Response &resp);

        bool handleDownloadData (Request &req, Response &resp);

        bool handleWaitForTrigger (Request &req, Response &resp);

        bool handleCalibration (Request &req, Response &resp);

        bool handleSetOffset (Request &req, Response &resp);

        bool handlePing (Request &req, Response &resp);

    	bool handleLoadFirmware (Request & req, Response &resp);

	    bool handleGetConfig (Request & req, Response &resp);

        bool handleReconnect (Request & req, Response &resp);

    };

}

#endif //PROJECT_NSKFASTADC_H
