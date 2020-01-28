#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>



int main(int argc, char *argv[])
{
        unsigned char rbuf[2];
        unsigned char wbuf[1];
        unsigned char abuf[3];
        int status = 0;
        if(argc<2){
            printf("How to use:\n./mb1242-change-address actual-address[7 bits] new-address[7 bits]\n\n");
            printf("To check the current address use:\n   sudo i2cdetect -y -r 10\n\n");
            printf("To enable the I2C bus10 use:\n   sudo modprobe i2c-gpio-custom bus0=10,33,23,10,10\n\n");
            return 0;
        }
        unsigned char actual_address = atoi(argv[1]);
        unsigned char new_address = atoi(argv[2]);
        // Net name I2C_1 Physical address : 0x12C70000
        int fd = open("/dev/i2c-10", O_RDWR);
        if (fd < 0) {
                printf("ERROR: open failed\n");
                return -1;
        }
        printf("Actual Address: %d\nNew Address: %d\n",actual_address,new_address);
        status = ioctl(fd, I2C_SLAVE, actual_address);
        if (status < 0) {
                printf("ERROR: ioctl error\n");
                close(fd);
                return -1;
        }
        // changing address
        abuf[0] = 170;
        abuf[1] = 165;
        abuf[2] = new_address*2; // the new address
        write(fd,abuf,3);
        usleep(1000000);
        status = ioctl(fd, I2C_SLAVE, new_address);
        if (status < 0) {
                printf("ERROR: ioctl error\n");
                close(fd);
                return -1;
        }

        wbuf[0] = 81;
        write(fd, wbuf, 1);
        usleep(100000);
        if (read(fd, rbuf, 2) != 2){
           printf("Failed to read from the i2c bus.\n");
        }else{
           printf("Distancia : %d - %d \n",rbuf[0],rbuf[1]); 
        }         
        close(fd);
}
