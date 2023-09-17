# ESP32 AWS IoT MQTT Project

This project demonstrates an MQTT-based application on the ESP32 microcontroller using AWS IoT services. It includes both hardware setup and software configurations to get started, with the ability to remotely control an LED using a Python script.



## Project Components

This project consists of the following components:

- ESP32 hardware setup and configuration.
- AWS IoT configuration and endpoint setup.
- MQTT communication with AWS IoT.
- 2 LEDs and 2 resistors, for LED toggle and Wifi status check.
- A button.
- A breadboard.
- 5 jumper cables.

## Prerequisites

Before you begin, ensure you have the following prerequisites:

- An ESP32 development board.
- Access to AWS IoT services.
- Basic knowledge of ESP32, AWS IoT, and MQTT.

## Hardware Setup Instructions

Follow these steps to set up hardware components:

- Place the ESP32 board on the breadboard.
- Make the GND connection.
- Connect LEDs to GPIO 2 and GPIO 4.
- Connect the button to GPIO 5.
- Using Micro-USB cable connect ESP32 board to the computer.


### AWS IoT Configuration

1. Access the AWS IoT Console to create or configure an IoT device and obtain your AWS IoT endpoint hostname.

2. **Obtain Certificates:**
   - In the AWS IoT Console, navigate to your IoT device's settings.
   - Under "Device Security," generate or download the required certificates and keys (e.g., X.509 certificates) for your device. Save these securely.

3. **Replace Certificates in the Project:**
   - Locate the `main/certs` and `mqtt-python` directories in your project.
   - Replace the following certificate files with the ones you obtained:
     - `root_cert_auth.pem`: Root CA certificate.
     - `certificate.pem.crt`: Device certificate.
     - `private.pem.key`: Device private key.

4. Run `idf.py menuconfig` to access the ESP-IDF configuration menu.

5. Under `Example Configuration`, set the `MQTT Broker Endpoint` to the AWS IoT endpoint hostname obtained in step 1.

6. Set the `MQTT Client ID` to a unique value, ensuring it complies with AWS IoT's requirements.


### Certificate Verification (Optional)

1. This project includes a Root CA certificate (`main/certs/root_cert_auth.pem`) for AWS IoT. You can use it to verify the AWS IoT server certificate.

2. To locally verify the Root CA certificate, run the following command (replace `hostname` with your AWS MQTT endpoint host):

   ```shell
   openssl s_client -showcerts -connect hostname:8883 < /dev/null

### Running the Project

To run this project on your ESP32 device, follow these steps:

1. Open a terminal and navigate to the project directory:
  idf.py build
  idf.py -p /dev/ttyUSB0 flash
  idf.py -p /dev/ttyUSB0 monitor

2. Open a terminal and navigate to mqtt-python file in the project directory:
  python3 mqtt_publisher.py
