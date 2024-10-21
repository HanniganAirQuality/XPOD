# XPOD
The XPOD is a low-cost air quality monitor that is meant for researchers. It's less user-friendly than the YPOD.

# Using this Firmware
To utilize the xpod_V3.1.2 firmware, you need to:
1. Move all un-zipped libraries into your your Documents/Arduino/libraries folder on your computer
2. Ensure that your xpod_V3.1.2 folder includes:
	xpod_V3.1.2.ino
	xpod_node.h
	wind_vane.h
	wind_vane.cpp
	quad_module.h
	quad_module.cpp
	pms_module.h
	pms_module.cpp
	OPC.h
	OPC.cpp
	mq_module.h
	mq_module.cpp
	gps_module.h
	gps_module.cpp
	digipot.h
	digipot.cpp
	bme_module.h
	bme_module.cpp
	ads_module.h
	ads_module.cpp

# Data Analysis Notes
The XPOD V3.1.2 Firmware is currently the only one which has associated headers - please see the xlsx file in the V3.1.2 folder

# Update Tracker
Thanks to Izzy for this suggestion! Here we will be tracking each version of the firmware.
| Version       | "Named" Ver.   | Pilot         | Date               | Description & Purpose                		|
| ------------- | -------------- | ------------- | -------------      | ----------------------------------------------- |
**| V3.1.2        | Initial Upload | Percy         | July 10, 2024      | Moving documents to central git	     		|**
| V3.2.0        | UTC Pilot      | Percy         | October 02, 2024   | Fulfills feature request for UTC timestamps	|
| V3.2.1        | EXT LED Mod	 | Percy         | October 03, 2024   | External LED support				|
| V3.2.2        | Landscaping 	 | Percy         | October 04, 2024   | Trims unused FW - Motor control, GPS, OPC	|
**| V3.2.3        | PCB LED Repair | Percy         | October 18, 2024   | Fixes LEDs on PCB and updates header file	|**
** Please note that none of these versions have the PM fix yet, because it hasn't been tested fully yet. Message me if you want that though.**
