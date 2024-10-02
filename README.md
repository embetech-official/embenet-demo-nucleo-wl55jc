# embeNET Demo for NUCLEO-WL55JC board

## What's inside ?

This repository contains an exemplary project demonstrating the use of embeNET Suite on [NUCLEO-WL55JC](https://www.st.com/en/evaluation-tools/nucleo-wl55jc.html) evaluation boards. 
The project is designed for the [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html). 

## What's included ?

The demo includes the following components of the embeNET Suite:
- The embeNET Node library in demo mode, providing IPv6 wireless mesh networking connectivity
- A port of the embeNET Node for the STM32WL5 chip
- embeNET Node Management Service (ENMS) providing telemetry services
- MQTT-SN client

## What are the demo version limitations ?

The demo *can only be used for evaluation purposes* (see LICENSE.txt for details).
The demo is limited to 10 nodes only (including root node).

## What you'll need to run the demo

- PC with Windows
- One [NUCLEO-WL55JC](https://www.st.com/en/evaluation-tools/nucleo-wl55jc.html) board connected to the PC (via USB cable) that will act as the root of the network
- At least one more (and up to 9) [NUCLEO-WL55JC](https://www.st.com/en/evaluation-tools/nucleo-wl55jc.html) boards that will act as the network nodes
- STM32CubeIDE, available for download from [the official ST site](https://www.st.com/en/development-tools/stm32cubeide.html)
- Any serial port terminal, for example [ExtraPutty](https://sourceforge.net/projects/extraputty), to view the logs
- (Optionally)STM32CubeProgrammer available for download from [the official ST site](https://www.st.com/en/development-tools/stm32cubeprog.html)

Optionally, to play with the MQTT-SN demo service you'll need:
- MQTT For Small Things (SN) written in Java, available for download from [github](https://github.com/simon622/mqtt-sn)
- MQTT Client Toolbox, available for download from [the official MQTTX page](https://mqttx.app)

Optionally, to easily interact with the custom UDP service you'll need
- [UDP - Sender/Reciever app from Microsoft Store](https://www.microsoft.com/store/apps/9nblggh52bt0)

## How to start?

Read the ['Getting started with embeNET demo for NUCLEO-WL55JC board'](https://embe.tech/docs/?q=doxyview/Getting%20started%20with%20Nucleo-WL55JC/index.html) tutorial.