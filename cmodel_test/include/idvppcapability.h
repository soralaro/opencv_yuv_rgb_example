/**
 * Copyright (C) 2018. Huawei Technologies Co., Ltd. All rights reserved.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

#ifndef DVPP_IDVPPCAPABILITY_H
#define DVPP_IDVPPCAPABILITY_H

#include "dvpp_config.h"

class IDVPPCAPABILITY {
public:
    virtual ~IDVPPCAPABILITY(void) {}

    virtual int process(dvppapi_ctl_msg *MSG) = 0;
    virtual int init() = 0;
    virtual int start() = 0;
    virtual int stop() = 0;

protected:
    dvppapi_ctl_msg* MSG_;
};

#endif // DVPP_IDVPPCAPABILITY_H