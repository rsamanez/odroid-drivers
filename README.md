# odroid-drivers

## ioboard-mb1242 : MB1242 I2CXL-MaxSonar-EZ
The I2CXL-MaxSonar-EZ is a industrial ultrasonic sensor with an easy to use I2C interface. This sensor have a high acoustic power output along with real-time auto calibration for changing conditions that ensure users receive the most reliable ranging data for every reading taken.
This sensor detects objects from 20cm to 765cm (25.1feet) with 1cm resolution.

This Driver was made to use in conjunction with the Odroid IO Expansion Board   
https://odroid.com/dokuwiki/doku.php?id=en:xu3_expansionboard

### Compilation
```
make -C /lib/modules/$(uname -r)/build M=/home/driver-path modules
sudo cp ioboard-mb1242.ko /lib/modules/$(uname -r)/kernel/drivers
sudo depmod
```

### Testing Driver
```
sudo su
modprobe i2c-gpio-custom bus0=10,33,23,10,10
modprobe ioboard-mb1242
echo ioboard-mb1242 0x70 > /sys/class/i2c-dev/i2c-10/device/new_device
echo 1 > /sys/class/i2c-dev/i2c-10/device/10-0070/enable
```

Get values from sensor MB1242
```
cat /sys/class/i2c-dev/i2c-10/device/10-0077/distance
```
