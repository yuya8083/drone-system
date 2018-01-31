// -------------------------------------------------------------------------
// CV Drone (= OpenCV + AR.Drone)
// Copyright(C) 2016 puku0x
// https://github.com/puku0x/cvdrone
//
// This source file is part of CV Drone library.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of EITHER:
// (1) The GNU Lesser General Public License as published by the Free
//     Software Foundation; either version 2.1 of the License, or (at
//     your option) any later version. The text of the GNU Lesser
//     General Public License is included with this library in the
//     file cvdrone-license-LGPL.txt.
// (2) The BSD-style license that is included with this library in
//     the file cvdrone-license-BSD.txt.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files
// cvdrone-license-LGPL.txt and cvdrone-license-BSD.txt for more details.
//
//! @file   ardrone.cpp
//! @brief  A source file of AR.Drone class
//
// -------------------------------------------------------------------------

#include "ardrone.h"

//センサ値初期値
int i1 = 100;
int i2 = 100;
int i3 = 100;
int i4 = 100;
int i5 = 75;

// --------------------------------------------------------------------------
//! @brief   Constructor of AR.Drone class
//! @param   ardrone_addr IP address of AR.Drone
//! @return  None
// --------------------------------------------------------------------------
ARDrone::ARDrone(const char *ardrone_addr)
{
    // IP Address
    strncpy(ip, ARDRONE_DEFAULT_ADDR, 16);

    // Sequence number
    seq = 0;

    // Camera image
    img = NULL;

    // Version information
    memset(&version, 0, sizeof(version));

    // Navdata
    memset(&navdata, 0, sizeof(navdata));

    // Configurations
    memset(&config, 0, sizeof(config));

    // Video
    pFormatCtx  = NULL;
    pCodecCtx   = NULL;
    pFrame      = NULL;
    pFrameBGR   = NULL;
    bufferBGR   = NULL;
    pConvertCtx = NULL;

    // Thread for AT command
    threadCommand = NULL;
    mutexCommand  = NULL;

    // Thread for Navdata
    threadNavdata = NULL;
    mutexNavdata  = NULL;

    // Thread for Video
    threadVideo = NULL;
    mutexVideo  = NULL;

    // Open if the IP address was specified
    if (ardrone_addr != NULL) {
        open(ardrone_addr);
    }
}

// --------------------------------------------------------------------------
//! @brief   Destructor of ARDrone class
//! @return  None
// --------------------------------------------------------------------------
ARDrone::~ARDrone()
{
    // See you
    close();
}

// --------------------------------------------------------------------------
//! @brief   Initialize the AR.Drone.
//! @param   ardrone_addr IP address of AR.Drone
//! @return  Result of initialization
//! @retval  1 Success
//! @retval  0 Failure
// --------------------------------------------------------------------------
int ARDrone::open(const char *ardrone_addr)
{
    // Initialize FFmpeg
    av_register_all();
    avformat_network_init();
    av_log_set_level(AV_LOG_QUIET);

    // Save IP address
    strncpy(ip, ardrone_addr, 16);

    // Get version information
    if (!getVersionInfo()) return 0;
    std::cout << "AR.Drone Ver. " << version.major << "." << version.minor << "." << version.revision << "." << std::endl;

    // Initialize AT command
    if (!initCommand()) return 0;

    // Blink LEDs
    setLED(ARDRONE_LED_ANIM_BLINK_GREEN);

    // Initialize Navdata
    if (!initNavdata()) return 0;

    // Initialize Video
    if (!initVideo()) return 0;

    // Wait for updating the status
    //msleep(500);

    // Get configurations
    if (!getConfig()) return 0;

    // Stop LED animation
    setLED(ARDRONE_LED_ANIM_STANDARD);

    // Reset emergency
    resetWatchDog();
    resetEmergency();

    return 1;
}

// --------------------------------------------------------------------------
//! @brief   Update the information of the AR.Drone.
//! @return  Result of update
//! @retval  1 Success
//! @retval  0 Failure
// --------------------------------------------------------------------------
int ARDrone::update(void)
{
    return 1;
}

// --------------------------------------------------------------------------
//! @brief   Finalize the AR.Drone class.
//! @return  None
// --------------------------------------------------------------------------
void ARDrone::close(void)
{
    // Stop AR.Drone
    if (!onGround()) landing();

    // Stop LED animation
    setLED(ARDRONE_LED_ANIM_STANDARD);

    // Finalize video
    finalizeVideo();

    // Finalize Navdata
    finalizeNavdata();

    // Finalize AT command
    finalizeCommand();
}
int ARDrone::TakeDistance(std::string dis) {

	try {

		if ((int)dis.find("1=") == 0) {
			std::string s1 = dis.substr(2);
			//std::cout << "1の抽出" << s1 << std::endl;

			i1 = std::stoi(s1);
		}

		if ((int)dis.find("2=") == 0) {
			std::string s2 = dis.substr(2);
			//std::cout << "2の抽出" << s2 << std::endl;

			i2 = std::stoi(s2);
		}

		if ((int)dis.find("3=") == 0) {
			std::string s3 = dis.substr(2);
			//std::cout << "3の抽出" << s3 << std::endl;

			i3 = std::stoi(s3);
		}

		if ((int)dis.find("4=") == 0) {
			std::string s4 = dis.substr(2);
			//std::cout << "4の抽出" << s4 << std::endl;

			i4 = std::stoi(s4);
		}

		if ((int)dis.find("5=") == 0) {
			std::string s5 = dis.substr(2);
			//std::cout << "5の抽出" << s5 << std::endl;

			i5 = std::stoi(s5);
		}
	}
	catch (std::invalid_argument e) {
		std::cout << "エラー" << std::endl;
		Sleep(1000);
		return 0;
	}

	printf("センサ値　前：%d, 右：%d, 後ろ：%d, 左：%d, 上：%d\n",i1, i2, i3, i4, i5);
	return AvoidSystem(i1, i2, i3, i4, i5);
}

int ARDrone::AvoidSystem(int i1, int i2, int i3, int i4, int i5) {

	static int Way = 0;
	
	if ((i1 >= 100) && (i2 >= 100) && (i3 >= 100) && (i4 >= 100) && (i5 >= 75)) {
		Way = 0;
		return Way;
	}
	else if (i5 < 75) {
		Way = 16;
		return Way;//下へ
	}
	else if (i1 < 100 && i2 < 100 && i3 < 100 && i4 < 100) {
		Way = 15;
		return Way;//上へ
	}
	else if (i1 < 100 && i2 < 100 && i3 < 100) {
		Way = 11;
		return Way;//左上へ
	}
	else if (i2 < 100 && i3 < 100 && i4 < 100) {
		Way = 12;
		return Way;//前上へ
	}
	else if (i3 < 100 && i4 < 100 && i1 < 100) {
		Way = 13;
		return Way;//右上へ
	}
	else if (i4 < 100 && i1 < 100 && i2 < 100) {
		Way = 14;
		return Way;//後ろ上へ
	}
	else if (i1 < 100 && i2 < 100) {
		Way = 5;
		return Way;//左後ろへ
	}
	else if (i2 < 100 && i3 < 100) {
		Way = 6;
		return Way;//左前へ
	}
	else if (i3 < 100 && i4 < 100) {
		Way = 7;
		return Way;//右前へ
	}
	else if (i4 < 100 && i1 < 100) {
		Way = 8;
		return Way;//右後ろへ
	}
	else if (i1 < 100 && i3 < 100) {
		Way = 9;
		return Way;//上へ
	}
	else if (i2 < 100 && i4 < 100) {
		Way = 10;
		return Way;//上へ
	}
	else if (i1 < 100) {
		Way = 1;
		return Way;//後ろへ
	}
	else if (i2 < 100) {
		Way = 2;
		return Way;//左へ
	}
	else if (i3 < 100) {
		Way = 3;
		return Way;//前へ
	}
	else if (i4 < 100) {
		Way = 4;
		return Way;//右へ
	}
	/*
	if (i1 < 100) {
		Way = 1;
		return Way;//左後ろへ
	}
	if (i2 < 100) {
		Way = 2;
		return Way;//左前へ
	}
	if (i3 < 100) {
		Way = 3;
		return Way;//右前へ
	}
	if (i4 < 100) {
		Way = 4;
		return Way;//右後ろへ
	}

	if (i1 < 100 && i2 < 100) {
		Way = 5;
		return Way;//左へ
	}
	if (i2 < 100 && i3 < 100) {
		Way = 6;
		return Way;//前へ
	}
	if (i3 < 100 && i4 < 100) {
		Way = 7;
		return Way;//右へ
	}
	if (i4 < 100 && i1 < 100) {
		Way = 8;
		return Way;//後ろへ
	}

	if (i1 < 100 && i3 < 100) {
		Way = 9;
		return Way;//上へ
	}
	if (i2 < 100 && i4 < 100) {
		Way = 10;
		return Way;//上へ
	}

	if (i1 < 100 && i2 < 100 && i3 < 100) {
		Way = 11;
		return Way;//左前上へ
	}
	if (i2 < 100 && i3 < 100 && i4 < 100) {
		Way = 12;
		return Way;//右前上へ
	}
	if (i3 < 100 && i4 < 100 && i1 < 100) {
		Way = 13;
		return Way;//右後ろ上へ
	}
	if (i4 < 100 && i1 < 100 && i2 < 100) {
		Way = 14;
		return Way;//左後ろ上へ
	}

	if (i1 < 100 && i2 < 100 && i3 < 100 && i4 < 100) {
		Way = 15;
		return Way;//上へ
	}

	if ((i1 >= 100) && (i2 >= 100) && (i3 >= 100) && (i4 >= 100)) {
		Way = 0;
		return Way;
	}
	*/
	return Way;
}