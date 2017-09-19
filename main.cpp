#include "mbed.h"
#include "C12832.h"
#include "MMA7660.h"
#include "CCS811.h"
#include "TSL2561.h"
#include "Sht31.h"
#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"
#include "vl53l0x_i2c_platform.h"

#include "PinDetect.h"

#define USE_I2C_2V8

Serial pc (USBTX,USBRX);

//Peripherals
I2C i2c1(PF_0, PF_1);
C12832 lcd(PE_14, PE_12, PD_12, PD_11, PE_9);
MMA7660 MMA(PF_0, PF_1);
TSL2561 tsl2561(PF_0, PF_1, TSL2561_ADDR_HIGH);
Sht31 sht31(PF_0, PF_1);
CCS811 ccs811(&i2c1);

//IO
PinDetect sw1(PG_4);
Ticker led_flash;
DigitalOut green_led(LED3);

//Variables
uint16_t x,y,z;
int j=0;

void led(){
    green_led =! green_led;
    }

void counter (){
    pc.printf ("J=%d\n\r", j);
    lcd.cls();
    j++;
    if (j>4) j=0;
    }

VL53L0X_Error WaitMeasurementDataReady(VL53L0X_DEV Dev) {
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    uint8_t NewDatReady=0;
    uint32_t LoopNb;
    
    if (Status == VL53L0X_ERROR_NONE) {
        LoopNb = 0;
        do {
            Status = VL53L0X_GetMeasurementDataReady(Dev, &NewDatReady);
            if ((NewDatReady == 0x01) || Status != VL53L0X_ERROR_NONE) {
                break;
            }
            LoopNb = LoopNb + 1;
            VL53L0X_PollingDelay(Dev);
        } while (LoopNb < VL53L0X_DEFAULT_MAX_LOOP);

        if (LoopNb >= VL53L0X_DEFAULT_MAX_LOOP) {
            Status = VL53L0X_ERROR_TIME_OUT;
        }
    }

    return Status;
}

VL53L0X_Error WaitStopCompleted(VL53L0X_DEV Dev) {
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    uint32_t StopCompleted=0;
    uint32_t LoopNb;

    if (Status == VL53L0X_ERROR_NONE) {
        LoopNb = 0;
        do {
            Status = VL53L0X_GetStopCompletedStatus(Dev, &StopCompleted);
            if ((StopCompleted == 0x00) || Status != VL53L0X_ERROR_NONE) {
                break;
            }
            LoopNb = LoopNb + 1;
            VL53L0X_PollingDelay(Dev);
        } while (LoopNb < VL53L0X_DEFAULT_MAX_LOOP);

        if (LoopNb >= VL53L0X_DEFAULT_MAX_LOOP) {
            Status = VL53L0X_ERROR_TIME_OUT;
        }

    }

    return Status;
}


int main(){
    
    //Setup peripherals
    led_flash.attach(&led, 1);
    
    tsl2561.begin();
    tsl2561.setGain(TSL2561_GAIN_0X);
    tsl2561.setTiming(TSL2561_INTEGRATIONTIME_402MS);
    
    ccs811.init();
    
    sw1.attach_deasserted(&counter);
    sw1.setSampleFrequency( 10000 );
    
    lcd.cls();
    lcd.locate(60,3);
    lcd.printf("MBED");
    
    
    //Setup laster
    int var=1, measure=0;
    int ave=0, sum=0;
    VL53L0X_Dev_t MyDevice;
    VL53L0X_Dev_t *pMyDevice = &MyDevice;
    VL53L0X_RangingMeasurementData_t    RangingMeasurementData;
    VL53L0X_RangingMeasurementData_t   *pRangingMeasurementData    = &RangingMeasurementData;
    
    // Initialize Comms laster
    pMyDevice->I2cDevAddr      = 0x52;
    pMyDevice->comms_type      =  1;
    pMyDevice->comms_speed_khz =  400;
    
    
    VL53L0X_RdWord(&MyDevice, VL53L0X_REG_OSC_CALIBRATE_VAL,0);
    VL53L0X_DataInit(&MyDevice); 
    uint32_t refSpadCount;
    uint8_t isApertureSpads;
    uint8_t VhvSettings;
    uint8_t PhaseCal;
    
    VL53L0X_StaticInit(pMyDevice); 
    VL53L0X_PerformRefSpadManagement(pMyDevice, &refSpadCount, &isApertureSpads); // Device Initialization
    VL53L0X_PerformRefCalibration(pMyDevice, &VhvSettings, &PhaseCal); // Device Initialization
    VL53L0X_SetDeviceMode(pMyDevice, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING); // Setup in single ranging mode
    VL53L0X_SetLimitCheckValue(pMyDevice, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, (FixPoint1616_t)(0.25*65536)); //High Accuracy mode, see API PDF
    VL53L0X_SetLimitCheckValue(pMyDevice, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, (FixPoint1616_t)(18*65536)); //High Accuracy mode, see API PDF
    VL53L0X_SetMeasurementTimingBudgetMicroSeconds(pMyDevice, 200000); //High Accuracy mode, see API PDF
    VL53L0X_StartMeasurement(pMyDevice);

    while(1) {
        
        if (j==0){
        float t = sht31.readTemperature();
        float h = sht31.readHumidity();
        lcd.locate(60,3);
        lcd.printf("MBED");
        lcd.locate(0,3);
        lcd.printf("[TEMP/HUM]");
        lcd.locate(0,15);
        lcd.printf("TEMP:%3.2fC, HUM:%3.2f%%", t, h); 
        wait(1);
        }
        
        if (j==1){
        x = tsl2561.getLuminosity(TSL2561_VISIBLE);     
//      y = tsl2561.getLuminosity(TSL2561_FULLSPECTRUM);
        z = tsl2561.getLuminosity(TSL2561_INFRARED);
        lcd.locate(60,3);
        lcd.printf("MBED");
        lcd.locate(0,3);
        lcd.printf("[LIGHT]");
        lcd.locate(0,14);
        lcd.printf("VIS: %d, INFR: %d ",x, z);
        wait(1);
        }
        
        if (j==2){
        lcd.locate(60,3);
        lcd.printf("MBED");
        lcd.locate(0,3);
        lcd.printf("[ACCEL]");
        lcd.locate(0,14);
        lcd.printf("x=%.2f y=%.2f z=%.2f",MMA.x(), MMA.y(), MMA.z());
        wait(1);
        }
        
        if (j==3){
        uint16_t eco2, tvoc;
        ccs811.readData(&eco2, &tvoc);
        lcd.locate(60,3);
        lcd.printf("MBED");
        lcd.locate(0,3);
        lcd.printf("[AIR QUAL]");
        lcd.locate(0,15);
        lcd.printf("eCO2:%dppm, TVO:%dppb", eco2, tvoc); 
        wait(2);
        }

        if (j==4){
        lcd.locate(60,3);
        lcd.printf("MBED");
        lcd.locate(0,3);
        lcd.printf("[DISTANCE]");
        lcd.locate(0,15);
            while(var<=10){
                WaitMeasurementDataReady(pMyDevice);
                VL53L0X_GetRangingMeasurementData(pMyDevice, pRangingMeasurementData);
                measure=pRangingMeasurementData->RangeMilliMeter;
                //printf("In loop measurement %d\n", mea);
                sum=sum+measure;
                // Clear the interrupt
                VL53L0X_ClearInterruptMask(pMyDevice, VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY);
                VL53L0X_PollingDelay(pMyDevice);
                var++;
                }
        ave=sum/var;
        lcd.cls();
        lcd.printf("%dmm", ave);
        var=1;
        sum=0;
        //VL53L0X_StopMeasurement(pMyDevice);
        //WaitStopCompleted(pMyDevice);
        //VL53L0X_ClearInterruptMask(pMyDevice,VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY);
        }
           
    }
}
