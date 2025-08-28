# NXP-Baremetal

## About
This repository contains the real-time firmware flashed on the NXP S32K148-Q176 Developer Kit as a part of Coda. This Node forwards the readings of switches/sensors through CAN to the Cluster Node.

## Used Peripherals
### MCAL
1. GPIO &rarr; For switches.
2. ADC &rarr; For RPM & speed.
3. Timer &rarr; For Ultrasonic sensors.
4. CAN &rarr; For publishing the readings/data.
### HAL
All of the HAL drivers are designed with async response in mind for the best possible performance by avoiding polling and implementing techniques like software debouncing for switches and utilizing Input Capture Unit for Ultrasonic sensors.

## FreeRTOS Tasks
There are two categories for tasks:
1. Tasks that initiate reading &rarr; No polling is needed when read operations are done asynchronously and update the latest values for each reading type.
1. Tasks that send specific data/reading type through CAN &rarr; These tasks are responsible for transmitting the latest value for each reading type with its unique ID into its own configured mailbox to the CAN bus where it can be received and utilized by Cluster Node.

## Sequence Diagram
<img width="1234" height="777" alt="BaremetalxCommManager" src="https://github.com/user-attachments/assets/fd6694b9-6edb-45b2-80a5-eed0ea0c9a89" />
