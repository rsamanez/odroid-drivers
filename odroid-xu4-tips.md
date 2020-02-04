## Odroid-XU4 Fan Control

Fully manual way to control the fan speed.
```
# Set fan to manual mode
$ sudo su
$ echo 0 > /sys/devices/platform/pwm-fan/hwmon/hwmon0/automatic

# Set speed to 100%
$ echo 255 > /sys/devices/platform/pwm-fan/hwmon/hwmon0/pwm1

```

## Odroid XU4 - Expansion Board

### LEDs ON/OFF
```
$ sudo su

# Set GPIO port
$ echo 31 > /sys/class/gpio/export
$ echo out > /sys/class/gpio/gpio31/direction

# LED ON
$ echo 1 > /sys/class/gpio/gpio31/value

# LED OFF
$ echo 0 > /sys/class/gpio/gpio31/value
```
Other LEDs
```
$ echo 28 > /sys/class/gpio/export
$ echo out > /sys/class/gpio/gpio28/direction
# LED ON
$ echo 1 > /sys/class/gpio/gpio28/value
#----------------
$ echo 19 > /sys/class/gpio/export
$ echo out > /sys/class/gpio/gpio19/direction
# LED ON
$ echo 1 > /sys/class/gpio/gpio19/value
#----------------
$ echo 209 > /sys/class/gpio/export
$ echo out > /sys/class/gpio/gpio209/direction
# LED ON
$ echo 1 > /sys/class/gpio/gpio209/value
#----------------
$ echo 24 > /sys/class/gpio/export
$ echo out > /sys/class/gpio/gpio24/direction
# LED RED ON
$ echo 1 > /sys/class/gpio/gpio24/value
```
### Buttons Reading
```
# SW1
$ echo 29 > /sys/class/gpio/export
$ echo in > /sys/class/gpio/gpio29/direction
# Read Button state
$ cat /sys/class/gpio/gpio29/value
#--------------------
# SW2
$ echo 30 > /sys/class/gpio/export
$ echo in > /sys/class/gpio/gpio30/direction
# Read Button state
$ cat /sys/class/gpio/gpio30/value
#--------------------
# SW3
$ echo 22 > /sys/class/gpio/export
$ echo in > /sys/class/gpio/gpio22/direction
# Read Button state
$ cat /sys/class/gpio/gpio22/value
#--------------------
# SW4
$ echo 18 > /sys/class/gpio/export
$ echo in > /sys/class/gpio/gpio18/direction
# Read Button state
$ cat /sys/class/gpio/gpio18/value
