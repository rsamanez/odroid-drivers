## Odroid-XU4 Fan Control

Fully manual way to control the fan speed.
```
# Set fan to manual mode
$ sudo su
$ echo 0 > /sys/devices/platform/pwm-fan/hwmon/hwmon0/automatic

# Set speed to 100%
$ echo 255 > /sys/devices/platform/pwm-fan/hwmon/hwmon0/pwm1

```
