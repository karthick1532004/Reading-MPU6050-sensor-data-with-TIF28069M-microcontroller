#include "F2806x_Device.h"
#include "F2806x_Examples.h"
#include "F2806x_Gpio.h"
#include "F2806x_I2c.h"

#define MPU6050_ADDR 0x68 // MPU6050 I2C address
#define ACCEL_XOUT_H_REG 0x3B // Accelerometer X-axis data high byte register address

//void delay_ms_2(void);
void InitI2CGpio(void);
void InitI2C(void);
//void write_i2c_data(void);

Uint16 i2c_slave_data;
Uint16 accel_slave_data;
Uint16 high,low,value;
float temp,accel_x,accel_y,accel_z;
float gyro_x,gyro_y,gyro_z;

void
delay_ms_2(Uint16 ms)
{
    double j;
       for (j = 0; j < ms; j++)
       {

       }
}

void main()
{
    InitSysCtrl();
    InitPieCtrl();
    InitPieVectTable();

    InitI2CGpio();
    InitI2C();

    // Check MPU6050 WHO_AM_I register to ensure communication
    I2caRegs.I2CMDR.all = 0x2620; // Start, Stop, Master mode, Transmit mode, Standard mode
    I2caRegs.I2CSAR = MPU6050_ADDR;
    I2caRegs.I2CCNT = 1;
    I2caRegs.I2CDXR = 0x75;
    while(I2caRegs.I2CSTR.bit.XRDY == 0);

    I2caRegs.I2CMDR.all = 0x2C20;
    while (I2caRegs.I2CSTR.bit.RRDY == 0); // Wait for receive to complete
    i2c_slave_data = I2caRegs.I2CDRR; // Read received data from MPU6050

    delay_ms_2(20000);

    //I2caRegs.I2CMDR.bit.STT = 1;
    write_i2c_data(MPU6050_ADDR,0x6B,0x00,2);
    delay_ms_2(20000);
    write_i2c_data(MPU6050_ADDR,0x19,0x07,2);
    delay_ms_2(200);
    write_i2c_data(MPU6050_ADDR,0x1A,0x00,2);
    delay_ms_2(200);
    write_i2c_data(MPU6050_ADDR,0x1B,0x00,2);
    delay_ms_2(200);
    write_i2c_data(MPU6050_ADDR,0x1C,0x00,2);
    delay_ms_2(20000);
    for(;;){
    temp = read_raw_data(MPU6050_ADDR,0x41,1) / 340.0 + 36.53;
    delay_ms_2(2000);
    accel_x = read_raw_data(MPU6050_ADDR,0x3B,1) / 16384.0 ;
    delay_ms_2(2000);
    accel_y = read_raw_data(MPU6050_ADDR,0x3B+2,1) / 16384.0;
    delay_ms_2(2000);
    accel_z = read_raw_data(MPU6050_ADDR,0x3B+4,1) / 16384.0;
    delay_ms_2(2000);
    gyro_x = read_raw_data(MPU6050_ADDR,0x43,1) / 131.0;
    delay_ms_2(2000);
    gyro_y = read_raw_data(MPU6050_ADDR,0x43+2,1) / 131.0;
    delay_ms_2(2000);
    gyro_z = read_raw_data(MPU6050_ADDR,0x43+4,1) / 131.0;
    }




}

void InitI2CGpio()
{
    // Initialize GPIO pins for I2C
    EALLOW;
    GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;   // Enable pull-up for GPIO32 (SDAA)
    GpioCtrlRegs.GPBPUD.bit.GPIO33 = 0;   // Enable pull-up for GPIO33 (SCLA)
    GpioCtrlRegs.GPBQSEL1.bit.GPIO32 = 3; // Asynch input GPIO32 (SDAA)
    GpioCtrlRegs.GPBQSEL1.bit.GPIO33 = 3; // Asynch input GPIO33 (SCLA)
    GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 1;
    GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 1; // Configure GPIO33 as SCLA
    EDIS;
}

void InitI2C()
{
    // Initialize I2C
    I2caRegs.I2CSAR = 0x0000; // Set slave address
    I2caRegs.I2CPSC.all = 9;          // Prescaler - need to configure for your system clock
    I2caRegs.I2CCLKL = 10;            // NOTE: must be non zero
    I2caRegs.I2CCLKH = 5;             // NOTE: must be non zero
    I2caRegs.I2CIER.all = 0x24;       // Enable SCD & ARDY interrupts
    I2caRegs.I2CMDR.all = 0x0020;     // Take I2C out of reset
}

void write_i2c_data(Uint16 salve_addres, Uint16 address, Uint16 data, Uint16 cnt){
    I2caRegs.I2CMDR.all = 0x2620;
    I2caRegs.I2CSAR = salve_addres;
    I2caRegs.I2CCNT = cnt;
    I2caRegs.I2CDXR = address;
    while(I2caRegs.I2CSTR.bit.XRDY == 0){};
    //I2caRegs.I2CMDR.all = 0x2620;
    I2caRegs.I2CDXR = data;
    while(I2caRegs.I2CSTR.bit.XRDY == 0);
    I2caRegs.I2CMDR.bit.STP = 1;

}

int read_i2c_data(Uint16 slave_addres, Uint16 address,Uint16 cnt){
    Uint16 value_x;
    I2caRegs.I2CMDR.all = 0x2620;
    I2caRegs.I2CSAR = slave_addres;
    I2caRegs.I2CCNT = cnt;
    I2caRegs.I2CDXR = address;
    while(I2caRegs.I2CSTR.bit.XRDY == 0){};
    I2caRegs.I2CMDR.all = 0x2c20;
    while (I2caRegs.I2CSTR.bit.RRDY == 0); // Wait for receive to complete
    value_x = i2c_slave_data = I2caRegs.I2CDRR; // Read received data from MPU6050
    return value_x;
}

int read_raw_data(Uint16 slave_address,Uint16 address,Uint16 cnt ){
     high = read_i2c_data(slave_address,address,cnt);
     delay_ms_2(2000);
     low = read_i2c_data(slave_address,address+1,cnt);
     delay_ms_2(2000);
     value = high << 8 | low;
     if (value> 32768){
         value = value - 65536;
     }
     return value;
}




