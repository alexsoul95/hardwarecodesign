#include<stdio.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<linux/i2c.h>
#include<linux/i2c-dev.h>
// Small macro to display value in hexadecimal with 2 places
#define CTRL1G 0x20
#define CTRL2G 0x21

#define CTRL1MA 0x20
#define CTRL2MA 0x21
#define CTRL5MA 0x24
#define CTRL6MA 0x25
#define CTRL7MA 0x26
#define BUFFER_SIZE 1

//Gyro
#define X0_G      0x28
#define X1_G      0x29
#define Y0_G      0x2A
#define Y1_G      0x2B
#define Z0_G      0x2C
#define Z1_G      0x2D

//Acc
#define X0_MA      0x28
#define X1_MA      0x29
#define Y0_MA      0x2A
#define Y1_MA      0x2B
#define Z0_MA      0x2C
#define Z1_MA      0x2D

//Mag
#define X0_M      0x08
#define X1_M      0x09
#define Y0_M      0x0A
#define Y1_M      0x0B
#define Z0_M      0x0C
#define Z1_M      0x0D


char dataBuffer[BUFFER_SIZE];

// Write a single value to a single register
int writeRegister(int file, char address, char value){
   char buffer[2];
   buffer[0] = address;
   buffer[1] = value;
   if (write(file, buffer, 2)!=2){
      printf("Failed write to the device");
      return 1;
   }
   return 0;
}

// Read the entire 40 registers into the buffer (10 reserved)
int readRegisters(int file, char registers){
   // Writing a 0x00 to the device sets the address back to
   //  0x00 for the coming block read
    char buffer[1];
   buffer[0] = registers;
   if (write(file, buffer, 1)!=1){
      printf("Failed write to the device");
      return 1;
   }
  
   if(read(file, dataBuffer, BUFFER_SIZE)!=BUFFER_SIZE){
      printf("Failed to read in the full buffer.");
      return 1;
   }
   return 0;
}
// short is 16-bits in size on the BBB
short combineValues(char msb, char lsb){
   //shift the msb right by 8 bits and OR with lsb
   return ((short)msb<<8)|(short)lsb;
}

int main(){
int file;
printf("GYROSCOPE AND ACCELEROMETER,MAGNETOMETER test application\n");
if((file=open("/dev/i2c-1", O_RDWR)) < 0){
perror("failed to open the bus\n");
return 1;
}
if(ioctl(file, I2C_SLAVE, 0x6B) < 0){
perror("Failed to connect to the sensor\n");
return 1;
}

writeRegister(file, CTRL1G, 0x0F);
writeRegister(file, CTRL2G, 0x20);
readRegisters(file, CTRL1G);

//printf("Power mode is 0x%02x\n", dataBuffer[0]);
readRegisters(file, CTRL2G);
//printf("High Pass Filter Mode is 0x%02x\n", dataBuffer[0]);

if(ioctl(file, I2C_SLAVE, 0x1d) < 0){
perror("Failed to connect to the sensor\n");
return 1;
}

writeRegister(file, CTRL1MA, 0x57);
writeRegister(file, CTRL2MA, 0x18);
writeRegister(file, CTRL5MA, 0x64);
writeRegister(file, CTRL6MA, 0x20);
writeRegister(file, CTRL7MA, 0x00);


// Now loop a display the x, y, z gyro for 60 seconds
   int count1=0;
   while(count1 < 60){
	if(ioctl(file, I2C_SLAVE, 0x6b) < 0){
	perror("Failed to connect to the sensor\n");
	return 1;
	}
	readRegisters(file, X1_G);
	char x1h = dataBuffer[0];
	readRegisters(file, X0_G);
	char x1l = dataBuffer[0];
      	short x1 = combineValues(x1h, x1l);
	readRegisters(file, Y1_G);
	char y1h = dataBuffer[0];
	readRegisters(file, Y0_G);
	char y1l = dataBuffer[0];
      	short y1 = combineValues(y1h, y1l);
	readRegisters(file, Z1_G);
	char z1h = dataBuffer[0];
	readRegisters(file, Z0_G);
	char z1l = dataBuffer[0];
      	short z1 = combineValues(z1h, z1l);
	
      //Use \r and flush to write the output on the same line
      printf("Gyr X=%d Y=%d Z=%d\n",x1,y1,z1);
	
	if(ioctl(file, I2C_SLAVE, 0x1D) < 0){
	perror("Failed to connect to the sensor\n");
	return 1;
	}
	readRegisters(file, X1_M);
	char x3h = dataBuffer[0];
	readRegisters(file, X0_M);
	char x3l = dataBuffer[0];
      	short x3 = combineValues(x3h, x3l);
	readRegisters(file, Y1_M);
	char y3h = dataBuffer[0];
	readRegisters(file, Y0_M);
	char y3l = dataBuffer[0];
      	short y3 = combineValues(y3h, y3l);
	readRegisters(file, Z1_M);
	char z3h = dataBuffer[0];
	readRegisters(file, Z0_M);
	char z3l = dataBuffer[0];
      	short z3 = combineValues(z3h, z3l);

      //Use \r and flush to write the output on the same line
      printf("Mag X=%d Y=%d Z=%d\n\n", x3, y3, z3);

	if(ioctl(file, I2C_SLAVE, 0x1D) < 0){
	perror("Failed to connect to the sensor\n");
	return 1;
	}
	readRegisters(file, X1_MA);
	char x2h = dataBuffer[0];
	readRegisters(file, X0_MA);
	char x2l = dataBuffer[0];
      	short x2 = combineValues(x2h, x2l);
	readRegisters(file, Y1_MA);
	char y2h = dataBuffer[0];
	readRegisters(file, Y0_MA);
	char y2l = dataBuffer[0];
      	short y2 = combineValues(y2h, y2l);
	readRegisters(file, Z1_MA);
	char z2h = dataBuffer[0];
	readRegisters(file, Z0_MA);
	char z2l = dataBuffer[0];
      	short z2 = combineValues(z2h, z2l);

      //Use \r and flush to write the output on the same line
      printf("Acc X=%d Y=%d Z=%d\n\n", x2, y2, z2);
      

      usleep(2000);
      count1++;
   }


close(file);
return 0;
}
