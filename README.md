# odroid-drivers

## ioboard-mb1242 : MB1242 I2CXL-MaxSonar-EZ
The I2CXL-MaxSonar-EZ is a industrial ultrasonic sensor with an easy to use I2C interface. This sensor have a high acoustic power output along with real-time auto calibration for changing conditions that ensure users receive the most reliable ranging data for every reading taken.
This sensor detects objects from 20cm to 765cm (25.1feet) with 1cm resolution.

This Driver was made to use in conjunction with the Odroid IO Expansion Board   
https://odroid.com/dokuwiki/doku.php?id=en:xu3_expansionboard

### Compilation
```
odroid@odroid:~$ make -C /lib/modules/$(uname -r)/build M=/home/driver-path modules
odroid@odroid:~$ sudo cp ioboard-mb1242.ko /lib/modules/$(uname -r)/kernel/drivers
odroid@odroid:~$ sudo depmod
```

### Testing Driver
```
odroid@odroid:~$ sudo su
root@odroid:~$ modprobe i2c-gpio-custom bus0=10,33,23,10,10
root@odroid:~$ modprobe ioboard-mb1242
root@odroid:~$ echo ioboard-mb1242 0x70 > /sys/class/i2c-dev/i2c-10/device/new_device
root@odroid:~$ echo 1 > /sys/class/i2c-dev/i2c-10/device/10-0070/enable
```

Get values from sensor MB1242
```
odroid@odroid:~$ sudo cat /sys/class/i2c-dev/i2c-10/device/10-0077/distance
```
### Changing the MB1242 defaul Address
By default the sensor address is 0x70(112 in decimal), this is a 7bits address. to change it use the mb1242-change-address application   
./mb1242-chane-address current-address new-address
```
odroid@odroid:~$ sudo ./mb1242-change-address 112 100
```
To check all the I2C devices in the bus10
```
odroid@odroid:~$ sudo i2cdetect -y -r 10
```
Instaling I2C tools
```
odroid@odroid:~$ sudo apt-get install i2c-tools
```
### ODROID XU4 - Rootless GPIO interaction
append this to your udev rules ( or create it's own )   
/etc/udev/rules.d/10-odroid.rules
```
SUBSYSTEM=="gpio", KERNEL=="gpiochip*", ACTION=="add", PROGRAM="/bin/sh -c 'chown root:odroid /sys/class/gpio/export /sys/class/gpio/unexport ; chmod 220 /sys/class/gpio/export /sys/class/gpio/unexport'"
SUBSYSTEM=="gpio", KERNEL=="gpio*", ACTION=="add", PROGRAM="/bin/sh -c 'chown root:odroid /sys%p/active_low /sys%p/direction /sys%p/edge /sys%p/value ; chmod 660 /sys%p/active_low /sys%p/direction /sys%p/edge /sys%p/value'"
```
Oh sorry I forgot too mention in my version, it gives permission for anyone in the gpio group. So you need to explicitly add odroid to the group. But it also allows you to add others if they just get added to the group by root once.   

The above UDEV rules are not providing interrupts access. The complete rules with interrupts enables are below, access to uevent is also needed:
```
SUBSYSTEM=="gpio", KERNEL=="gpiochip*", ACTION=="add", PROGRAM="/bin/sh -c 'chown root:gpio /sys/class/gpio/export /sys/class/gpio/unexport ; chmod 220 /sys/class/gpio/export /sys/class/gpio/unexport'"
SUBSYSTEM=="gpio", KERNEL=="gpio*", ACTION=="add", PROGRAM="/bin/sh -c 'chown root:gpio /sys%p/active_low /sys%p/direction /sys%p/edge /sys%p/value /sys%p/uevent; chmod 660 /sys%p/active_low /sys%p/direction /sys%p/edge /sys%p/value /sys%p/uevent'"
```
A simple way to get access to GPIO without being root.

Example:
To get access to pin 4 on the 30-pin header, add this line to the file /etc/rc.local, just before the line "exit 0" (you need to be root to edit it):
```
sudo -u odroid /usr/local/bin/gpio export 173 out
```
Now, every time the computer reboots, it will export the gpio line, just as if you had used echo 173 /sys/class/gpio/export, except that the owner of the "value" file that is created will be "odroid" and not "root". You can write to it without being root.
```
sudo -u odroid /usr/local/bin/gpio export 173 out
```
Now, every time the computer reboots, it will export the gpio line, just as if you had used echo 173 /sys/class/gpio/export, except that the owner of the "value" file that is created will be "odroid" and not "root". You can write to it without being root.


