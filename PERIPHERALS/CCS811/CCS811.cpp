#include "CCS811.h"

CCS811::CCS811(I2C* i2c) : m_i2c(i2c), pc(USBTX, USBRX) {
    
}

/**
 ** Initial CCS811 need write MODE register and should Write APP START register to begin measurement.
 **/
void CCS811::init() {
    wait_ms(50);
    
    char send[2];
    char read[8];
    char hwv[8];
    char hwd[8];
      
    read[0] = CCS811_REG_STATUS; //0x00
    
    m_i2c->write(CCS811_I2C_ADDR, read, 1);
    m_i2c->read(CCS811_I2C_ADDR, hwv, 1);
    
    wait_us(50);
    
    send[0] = CCS811_REG_APP_START; //0xF4
    
    wait_us(50);
    
    m_i2c->write(CCS811_I2C_ADDR, send, 1);
    
    wait_us(50);

    read[0] = CCS811_REG_STATUS; //0x00
    
    wait_us(50);

    m_i2c->write(CCS811_I2C_ADDR, read, 1);
    m_i2c->read(CCS811_I2C_ADDR, hwd, 1);
    
    wait_us(50);

    
    send[0] = CCS811_REG_MEAS_MODE; //0x01
    send[1] = CCS811_MEASUREMENT_MODE1; //0x10
   
    m_i2c->write(CCS811_I2C_ADDR, send, 2);
    wait_us(50);   
    
    wait_us(50);

    
    read[0] = CCS811_REG_STATUS; //0x00
    
    wait_us(50);

    
    m_i2c->write(CCS811_I2C_ADDR, read, 1);
    m_i2c->read(CCS811_I2C_ADDR, hwd, 1);
    
    wait_us(50);   
}

int CCS811::setMeasureMode(char mode) {
    
    char send[2];
    
    send[0] = CCS811_REG_MEAS_MODE;
    send[1] = mode;
    
    m_i2c->write(CCS811_I2C_ADDR, send, 2);
    
//    send[0] = CCS811_REG_APP_START;
//    send[1] = 0x00;
    
//    m_i2c->write(CCS811_I2C_ADDR, send, 2);
    
    return 0;
}

bool CCS811::readstatus() {

    char read[8];
    char hwd[8];
       
    read[0] = CCS811_REG_STATUS; //0x00
       
    m_i2c->write(CCS811_I2C_ADDR, read, 1);
    m_i2c->read(CCS811_I2C_ADDR, hwd, 1);
       
    pc.printf("STATUS 0x%X\r\n", hwd[0]);
    
    return 0;
}

bool CCS811::readmeas() {

    char read[8];
    char hwd[8];    
    
    read[0] = CCS811_REG_MEAS_MODE; //0x01
   
    m_i2c->write(CCS811_I2C_ADDR, read, 1);
    m_i2c->read(CCS811_I2C_ADDR, hwd, 1);
       
    pc.printf("meas 0x%X\r\n", hwd[0]);
    
    return 0;
}

bool CCS811::readerror() {

    char read[8];
    char hwv[8];
    
    read[0] = CCS811_REG_ERROR_ID; //0xE0

    m_i2c->write(CCS811_I2C_ADDR, read, 1);
    m_i2c->read(CCS811_I2C_ADDR, hwv, 1);

    pc.printf("error 0x%X \r\n", hwv[0]);
    
    return 0;
}

/**
 ** Here is that you can read CCS811 with co2 ppm and tvoc bbm is unsigned value
 **/
int CCS811::readData(uint16_t *ECO2, uint16_t *TVOC) {

    char recv[8];
    char send[1];

    send[0] = CCS811_REG_ALG_RESULT_DATA;
    m_i2c->write(CCS811_I2C_ADDR, send, 1, true);
    m_i2c->read(CCS811_I2C_ADDR, recv, 8, false);
    wait_ms(1);
/*
    pc.printf("%X %X\r\n", recv[0], recv[1]);
    pc.printf("%X %X\r\n", recv[2], recv[3]);
    pc.printf("%X %X\r\n", recv[4], recv[5]);
    pc.printf("%X %X\r\n", recv[6], recv[7]);
*/    
    *ECO2 = (uint16_t) (recv[0] <<8) + recv[1];
    *TVOC = (uint16_t) (recv[2] <<8) + recv[3];
    
    return 0;
    
}

/**
 ** Here for check is CCS811 hardware from i2c bus
 **/
bool CCS811::checkHW() {

    char read[1];
    char hid[1];
    
    read[0] = CCS811_REG_HW_ID;
    
    m_i2c->write(CCS811_I2C_ADDR, read, 1, false);
    m_i2c->read(CCS811_I2C_ADDR, hid, 1, false);
    
//    pc.printf("%X\r\n", hid[0]);
    
    if (hid[0] == 0x81) {
        return true;
    } else {
        return false;
    }
    
}

/**
 **  Here is provide you soft reset CCS811 module 
 **/
bool CCS811::softRest() {
     
    char rstCMD[5] = {CCS811_REG_SW_RESET, 0x11,0xE5,0x72,0x8A};

    m_i2c->write(CCS811_I2C_ADDR, rstCMD, 5);
    
    return false;
      
}

