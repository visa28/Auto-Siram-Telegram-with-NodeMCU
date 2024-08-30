# Auto-Siram-Telegram-with-NodeMCU
This repository contains the source code and necessary files for the Auto Siram (Automatic Watering) system, which is controlled via Telegram and implemented using a NodeMCU microcontroller. The project aims to automate the process of watering plants by using a smart irrigation system that can be controlled remotely through Telegram. This allows users to monitor and manage the watering schedule of their plants efficiently, even when they are not physically present.

Key Features:

Remote Control via Telegram: Users can send commands through a Telegram bot to start or stop the watering process.
Real-time Monitoring: The system can send real-time updates and notifications to the user via Telegram about the status of the watering system.
Automation: Based on certain conditions, such as soil moisture levels or predefined schedules, the system can automatically activate the watering process.
User-friendly Interaction: Simple commands and responses via Telegram make it easy for users to interact with the system.
Scalability: The system can be extended to manage multiple plants or garden sections.
Technology Stack:

Hardware:

NodeMCU (ESP8266): A low-cost Wi-Fi microcontroller used to connect to the internet and control the watering mechanism.
Soil Moisture Sensor: To detect the moisture level in the soil and decide when watering is needed.
Relay Module: To control the water pump based on the commands received.
Water Pump: The mechanism that delivers water to the plants.
Software:

Arduino IDE: Used for programming the NodeMCU.
Telegram Bot API: To create a bot that interacts with users, sending commands to and receiving updates from the NodeMCU.
C/C++: Programming language used for the firmware running on the NodeMCU.
How it Works:

Setup and Configuration: The NodeMCU is connected to the soil moisture sensor and relay module controlling the water pump. The NodeMCU is also configured to communicate with a Telegram bot using the Wi-Fi network.

User Interaction: Users interact with the system by sending commands to the Telegram bot. For example, they can send a command to start or stop watering.

Automatic Watering: The system can automatically start watering when the soil moisture level falls below a certain threshold. The moisture sensor sends data to the NodeMCU, which then decides whether to activate the pump.

Real-time Notifications: The system sends real-time status updates to the user via Telegram, informing them about the watering process, moisture levels, and other relevant information.

Data Logging: The system can log data over time, providing insights into soil moisture levels and watering patterns, which can be useful for optimizing plant care.
