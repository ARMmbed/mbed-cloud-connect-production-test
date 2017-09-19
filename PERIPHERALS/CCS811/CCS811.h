#ifndef CCS811_H
#define CCS811_H

//CCS811 register define
#define CCS811_I2C_ADDR                 0xB6    //This is already left shifted 0x5B, if using other address, should be 0xB4 which is 0x5A left shifted.
#define CCS811_REG_STATUS               0x00    //Status.
#define CCS811_REG_MEAS_MODE            0x01    //Mesurement mode and conditions register.
#define CCS811_REG_ALG_RESULT_DATA      0x02    //Algorithm result. 2 bytes co2 ppm next 2 bytes ppb VOC level.
#define CCS811_REG_RAW_DATA             0x03    //Raw ADC data.
#define CCS811_REG_ENV_DATA             0x05    //Temperature and humidity data can be write to enavle compensation.
#define CCS811_REG_NTC                  0x06    //Provides the voltage across the reference registor and the voltage across the NTC resistor.
#define CCS811_REG_THRESHOLDS           0x10    //Thresholds for operation when interrupts are only generated when eCO2 ppm crosses a threshold.
#define CCS811_REG_BASELINE             0x11    //The encoded current baseline value can be read.A previously saved encoded baseline can be written.
#define CCS811_REG_HW_ID                0x20    //Hardware ID. The value is 0x81.
#define CCS811_REG_HW_VERSION           0x21    //Hardware version. The value is 0x1X.
#define CCS811_REG_BOOT_VERSION         0x23    //Firmware boot version. The First 2 bytes contain the firmware version number for the boot code.
#define CCS811_REG_APP_VERSION          0x24    //Firmware application version. The first 2 bytes contain the firmware version number for the application code.
#define CCS811_REG_ERROR_ID             0xE0    //Error ID. When the status register reports and error its source is lcated in this register.
#define CCS811_REG_APP_START            0xF4    //Application start. Used to transition the CCS811 state from boot to application mode, a write with no data is required. Before performing a write to APP_START the Status register should be accessed to check if there is a valid application present.
#define CCS811_REG_SW_RESET             0xFF    //If the correct 4 byres (0x11, 0xE5, 0x72, 0x8A)are written to this register in a single sequence the device will reset and return to BOOT bode.

//mode setting
#define CCS811_MEASUREMENT_MODE0        0x00    //Idle(Measurements are disabled in this mode).
#define CCS811_MEASUREMENT_MODE1        0x10    //Constant power mode, IAQ measurement every second.
#define CCS811_MEASUREMENT_MODE2        0x20    //Pulse heating mode IAQ measurement every 10 seconds.
#define CCS811_MEASUREMENT_MODE3        0x30    //Low power pulse heating mode IAQ measurement every 60 seconds.
#define CCS811_MEASUREMENT_MODE4        0x40    //Constant power mode, sensor measurement every 250ms. 1xx: Reserved modes (For future use).

//Interrupt control 
#define CCS811_INT_DATARDY              0x08    //At the end of each measurement cycle (250ms, 1s, 10s, 60s) a flag is set in the STATUS register regardless of the setting of this bit.
#define CCS811_INT_THRESH               0x04    //0: Interrupt mode (if enabled) operates normally 1: Interrupt mode (if enabled) only asserts the nINT signal (driven low) if the new ALG_RESULT_DATA crosses one of the thresholds set in the THRESHOLDS register by more than the hysteresis value (also in the THRESHOLDS register).

#include "mbed.h"

class CCS811 {
    public:
        CCS811(I2C* i2c);
        void init();
        int setMeasureMode(char mode);
        int readData(uint16_t *ECO2, uint16_t *TVOC);
        bool checkHW();
        bool softRest();
        bool readstatus();
        bool readmeas();
        bool readerror();
    protected:
        I2C* m_i2c;
        Serial pc;
    private:
};


#endif