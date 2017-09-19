#include "mbed.h"
#include "vl53l0x_i2c_platform.h"
#include "vl53l0x_platform.h"

I2C i2c(PF_0, PF_1);

int VL53L0X_scan( void )
{
	int Status;
	int i;
	char cmd[2];
	cmd[0]=0x01;
	cmd[1]=0x00;
	i2c.frequency(100000);
	for (i = 0; i<0xFF; i++)
	{
		cmd[0]=i;
		Status = i2c.write(i, cmd, 1);
		if (Status == 0)
			break;
	}
	return i;
}

int32_t VL53L0X_write_multi(uint8_t address, uint8_t index, uint8_t  *pdata, int32_t count)
{
	int Status;
	char data[count+1];
	data[0]=index;
	for (int i = 1; i< count+1; i++)
	{
		data[i]=pdata[i-1];
	}
	Status = i2c.write(address, data, count+1);
	return Status;
}

int32_t VL53L0X_read_multi(uint8_t address,  uint8_t index, uint8_t  *pdata, int32_t count)
{
	int Status;
	char cmd[1];
	char data[count];
	cmd[0]=index;
	Status = i2c.write(address, cmd, 1);
	Status = i2c.read(address, data, count);
	for (int i = 0; i< count; i++)
	{
		pdata[i] = data[i];
	}
	return Status;
}

int32_t VL53L0X_write_byte(uint8_t address,  uint8_t index, uint8_t   data)
{
	int Status;
	char cmd[2];
	cmd[0]=index;
	cmd[1]=data;
	Status = i2c.write(address, cmd, 2);
	return Status;
}

int32_t VL53L0X_write_word(uint8_t address,  uint8_t index, uint16_t  data)
{
	int Status;
	char cmd[2];
	char ind;
	ind = index;
	cmd[1]=data>>0&0xFF;
	cmd[0]=data>>8&0xFF;
	Status = i2c.write(address, &ind, 1);
	Status |= i2c.write(address, cmd, 2);
	return Status;
}

int32_t VL53L0X_write_dword(uint8_t address, uint8_t index, uint32_t  data)
{
	int Status;
	char cmd[4];
	char ind;
	ind = index;
	cmd[3]=data>>0&0xFF;
	cmd[2]=data>>8&0xFF;
	cmd[1]=data>>16&0xFF;
	cmd[0]=data>>24&0xFF;
	Status = i2c.write(address, &ind, 1);
	Status |= i2c.write(address, cmd, 4);
	return Status;
}

int32_t VL53L0X_read_byte(uint8_t address,  uint8_t index, uint8_t  *pdata)
{
	int Status;
	char data;
	char cmd[1];
	cmd[0]=index;
	Status = i2c.write(address, cmd, 1);
	Status |= i2c.read(address, &data, 1);
	*pdata = data;
	return Status;
}

int32_t VL53L0X_read_word(uint8_t address,  uint8_t index, uint16_t *pdata)
{
	int Status;
	uint16_t tmp;
	char data[2];
	char cmd[1];
	tmp=0;
	cmd[0]=index;
	Status = i2c.write(address, cmd, 1);
	Status |= i2c.read(address, data, 2);
	tmp |= data[1]<<0;
	tmp |= data[0]<<8;
	*pdata = tmp;
	return Status;	
}

int32_t VL53L0X_read_dword(uint8_t address, uint8_t index, uint32_t *pdata)
{
	int Status;
	uint32_t tmp;
	char data[4];
	char cmd[1];
	tmp=0;
	cmd[0]=index;
	Status = i2c.write(address, cmd, 1);
	Status |= i2c.read(address, data, 4);
	tmp |= data[3]<<0;
	tmp |= data[2]<<8;
	tmp |= data[1]<<16;
	tmp |= data[0]<<24;
	*pdata=tmp;
	return Status;	
}


