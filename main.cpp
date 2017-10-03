#include "mbed.h"
#include "C12832.h"
#include "SPI_MX25R.h"
#include <ctype.h>

#define MMA7660_ADDR 0x92
#define CCS811_ADDR 0xB6
#define TSL2561_ADDR 0x98
#define SHT31_ADDR 0x88
#define VL53L0X_ADDR 0x52

#define I2C_SDA PF_0
#define I2C_SCL PF_1
#define LED1 PD_9
#define LED2 PA_12
#define LED3 PD_8
#define LED4 PA_11
#define LED5 PC_2
#define LED6 PA_3
#define LED7 PF_6
//Peripherals
I2C i2c1(PF_0, PF_1);
C12832 lcd(PE_14, PE_12, PD_12, PD_11, PE_9);
SPI_MX25R spi_mem(PE_14, PE_13, PE_12, PE_11);
Ticker led_flash;
//IO
DigitalIn sw1(PF_2);
DigitalIn sw2(PG_4);
DigitalOut led_green(LED_GREEN, 1);
DigitalOut led_blue(LED_BLUE, 1);
DigitalOut led_red(LED_RED, 1);
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);
DigitalOut led5(LED5);
DigitalOut led6(LED6);
DigitalOut led7(LED7);

//Variables

void passed()
{
    led_green = !led_green;
}

void failed()
{
    led_red = !led_red;
}

void waiting()
{
    led_blue = !led_blue;
}

void counter()
{
    lcd.cls();
}

int main()
{

int status1, status2, status3, status4, status5, status6 = 0;
led_flash.attach(&waiting, 1);

printf("ID = 0x%04X\n\r", spi_mem.readREMS());

if (spi_mem.readREMS() == 0xC216)
{
    printf("SPIF OK!\n\r");
    status6 = 0;
}

else status6 = -1;
//Setup peripherals

printf("ID = 0x%04X\n\r", spi_mem.readREMS());

if (i2c1.write(MMA7660_ADDR, NULL, 0) == 0)
{
    printf("ACCEL OK!\n\r");
    status1 = 0;
}

else
{
    printf("ACCEL FAIL!\n\r");
    led_flash.attach(&failed, 0.5);
    lcd.locate(0, 8);
    lcd.printf("S1 F!");
    status1 = -1;
}

if (i2c1.write(CCS811_ADDR, NULL, 0) == 0)
{
    printf("IAQ OK!\n\r");
    status2 = 0;
}

else
{
    printf("IAQ FAIL!\n\r");
    led_flash.attach(&failed, 0.5);
    lcd.locate(25, 8);
    lcd.printf("S2 F!");
    status2 = -1;
}

if (i2c1.write(TSL2561_ADDR, NULL, 0) == 0)
{
    printf("LIGHT OK!\n\r");
    status3 = 0;
}

else
{
    printf("LIGHT FAIL!\n\r");
    led_flash.attach(&failed, 0.5);
    lcd.locate(50, 8);
    lcd.printf("S3 F!");
    status3 = -1;
}

if (i2c1.write(SHT31_ADDR, NULL, 0) == 0)
{
    printf("TEMP OK!\n\r");
    status4 = 0;
}

else
{
    printf("TEMP FAIL!\n\r");
    led_flash.attach(&failed, 0.5);
    lcd.locate(0, 16);
    lcd.printf("S4 F!");
    status4 = -1;
}

if (i2c1.write(VL53L0X_ADDR, NULL, 0) == 0)
{
    printf("LASER OK!\n\r");
    status5 = 0;
}

else
{
    printf("LASER FAIL!\n\r");
    led_flash.attach(&failed, 0.5);
    lcd.locate(25, 16);
    lcd.printf("S5 F!");
    status5 = -1;
}

if (status6==-1)
{
    printf("SPIF FAIL!\n\r");
    led_flash.attach(&failed, 0.5);
    lcd.locate(50, 16);
    lcd.printf("S6 F!");
}

if ((status1 == 0) && (status2 == 0) && (status3 == 0) && (status4 == 0) && (status5 == 0) && (status6 == 0))
{
    led_flash.attach(&passed, 0.5);
    lcd.locate(48, 16);
    lcd.printf("ALL SENSORS OK!");
    lcd.locate(0, 0);
    lcd.printf("PASSED");
    led_blue = 1;
}

if ((status1 == -1) || (status2 == -1) || (status3 == -1) || (status4 == -1) || (status5 == -1) || (status6 == -1))
{
    lcd.locate(80, 16);
    lcd.printf("SENS FAIL!");
    lcd.locate(0, 0);
    lcd.printf("FAIL");
    led_blue = 1;
}
}
