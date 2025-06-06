
# ODROID XU4 Custom Drivers

**Author:** [Rommel Samanez (rsamanez)](https://github.com/rsamanez)  
**Platform:** ODROID XU4  
**OS:** Ubuntu Linux 18.04 LTS  
**Sensor:** MaxBotix I2CXL-MaxSonar-EZ (MB1242)  
**Interface:** I2C via ODROID IO Expansion Board

## 📘 Overview

This repository provides a custom Linux kernel driver (`ioboard-mb1242.ko`) for interfacing the MaxBotix MB1242 ultrasonic sensor with the ODROID XU4 single-board computer. The MB1242 communicates over the I2C protocol and offers reliable distance measurements ranging from 20 cm to 765 cm with 1 cm resolution. This driver facilitates seamless integration of the sensor with the ODROID IO Expansion Board, enabling accurate distance sensing for various applications.

## ⚙️ Features

- **Distance Measurement:** Retrieves distance data from the MB1242 sensor via I2C.
- **Address Configuration:** Includes a utility (`mb1242-change-address`) to modify the sensor's I2C address.
- **GPIO Access:** Provides guidelines for configuring GPIO access without root privileges.
- **System Integration:** Offers instructions for compiling and loading the kernel module on Ubuntu 18.04 LTS.

## 🛠️ Installation

1. **Compile the Kernel Module:**

   ```bash
   make -C /lib/modules/$(uname -r)/build M=$(pwd) modules
   ```

2. **Install the Module:**

   ```bash
   sudo cp ioboard-mb1242.ko /lib/modules/$(uname -r)/kernel/drivers/
   sudo depmod
   ```

3. **Load the Module and Configure I2C:**

   ```bash
   sudo modprobe i2c-gpio-custom bus0=10,33,23,10,10
   sudo modprobe ioboard-mb1242
   echo ioboard-mb1242 0x70 | sudo tee /sys/class/i2c-dev/i2c-10/device/new_device
   echo 1 | sudo tee /sys/class/i2c-dev/i2c-10/device/10-0070/enable
   ```

4. **Read Distance Data:**

   ```bash
   cat /sys/class/i2c-dev/i2c-10/device/10-0070/distance
   ```

## 🔧 Changing the Sensor's I2C Address

The default I2C address of the MB1242 sensor is `0x70` (112 in decimal). To change it:

```bash
sudo ./mb1242-change-address 112 100
```

Replace `112` with the current address and `100` with the desired new address.

## 🧰 Additional Tools

- **I2C Tools Installation:**

  ```bash
  sudo apt-get install i2c-tools
  ```

- **Detect I2C Devices:**

  ```bash
  sudo i2cdetect -y -r 10
  ```

## 🔐 Configuring GPIO Access Without Root

To allow non-root users to access GPIO:

1. **Create UDEV Rules:**

   Add the following to `/etc/udev/rules.d/10-odroid.rules`:

   ```bash
   SUBSYSTEM=="gpio", KERNEL=="gpiochip*", ACTION=="add", PROGRAM="/bin/sh -c 'chown root:gpio /sys/class/gpio/export /sys/class/gpio/unexport; chmod 220 /sys/class/gpio/export /sys/class/gpio/unexport'"
   SUBSYSTEM=="gpio", KERNEL=="gpio*", ACTION=="add", PROGRAM="/bin/sh -c 'chown root:gpio /sys%p/active_low /sys%p/direction /sys%p/edge /sys%p/value /sys%p/uevent; chmod 660 /sys%p/active_low /sys%p/direction /sys%p/edge /sys%p/value /sys%p/uevent'"
   ```

2. **Add User to GPIO Group:**

   ```bash
   sudo usermod -aG gpio odroid
   ```

3. **Automate GPIO Export on Boot:**

   Add the following line to `/etc/rc.local` before `exit 0`:

   ```bash
   sudo -u odroid /usr/local/bin/gpio export 173 out
   ```

   Replace `173` with the desired GPIO pin number.

---

# Controladores Personalizados para ODROID XU4

**Autor:** [Rommel Samanez (rsamanez)](https://github.com/rsamanez)  
**Plataforma:** ODROID XU4  
**Sistema Operativo:** Ubuntu Linux 18.04 LTS  
**Sensor:** MaxBotix I2CXL-MaxSonar-EZ (MB1242)  
**Interfaz:** I2C mediante la Placa de Expansión IO de ODROID

## 📘 Descripción General

Este repositorio proporciona un controlador personalizado del kernel de Linux (`ioboard-mb1242.ko`) para la integración del sensor ultrasónico MB1242 de MaxBotix con la computadora de placa única ODROID XU4. El MB1242 se comunica a través del protocolo I2C y ofrece mediciones de distancia confiables que van desde 20 cm hasta 765 cm con una resolución de 1 cm. Este controlador facilita la integración del sensor con la Placa de Expansión IO de ODROID, permitiendo una detección de distancia precisa para diversas aplicaciones.

## ⚙️ Características

- **Medición de Distancia:** Recupera datos de distancia del sensor MB1242 a través de I2C.
- **Configuración de Dirección:** Incluye una utilidad (`mb1242-change-address`) para modificar la dirección I2C del sensor.
- **Acceso a GPIO:** Proporciona pautas para configurar el acceso a GPIO sin privilegios de root.
- **Integración del Sistema:** Ofrece instrucciones para compilar y cargar el módulo del kernel en Ubuntu 18.04 LTS.

... (contenido continúa igual como en la sección en inglés, adaptado al español)
