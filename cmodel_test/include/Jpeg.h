/**
 * Copyright (C) 2018. Huawei Technologies Co., Ltd. All rights reserved.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

#ifndef DVPP_JPEG_H
#define DVPP_JPEG_H

#include <stdint.h>
#include "ExportMacro.h"

constexpr uint32_t RGB_OUT_CHANNEL_NUM = 3;

struct JpegDecodeToRgbIn {
    uint8_t* jpegData;
    uint32_t jpegDataSize;
    uint32_t regionDecodeZoneLeft;
    uint32_t regionDecodeZoneRight;
    uint32_t regionDecodeZoneTop;
    uint32_t regionDecodeZoneBottom;

    JpegDecodeToRgbIn()
        : jpegData(nullptr),
          jpegDataSize(0),
          regionDecodeZoneLeft(0),
          regionDecodeZoneRight(0),
          regionDecodeZoneTop(0),
          regionDecodeZoneBottom(0) {}
};

struct JpegDecodeToRgbOut {
    uint8_t* rgbData; // only support RGB24
    uint32_t rgbDataSize;
    uint32_t outChannel;
    uint32_t imgWidth;
    uint32_t imgHeight;
    bool isHardwareDecode;

    JpegDecodeToRgbOut()
        : rgbData(nullptr),
          rgbDataSize(0),
          outChannel(RGB_OUT_CHANNEL_NUM),
          imgWidth(0),
          imgHeight(0),
          isHardwareDecode(true) {}
};

/*
* @brief : decode jpeg file to rgb
* @param [in] jpegDecodeToRgbIn : input data
* @param [out] jpegDecodeToRgbOut : output data
* @return : success, return DVPP_SUCCESS; fail, return DVPP_FAIL
*/
DVPP_EXPORT int32_t DvppJpegDecodeToRgb(JpegDecodeToRgbIn& jpegDecodeToRgbIn, JpegDecodeToRgbOut& jpegDecodeToRgbOut);

#endif // DVPP_JPEG_H