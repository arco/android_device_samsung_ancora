/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_BMA222_SENSOR_H
#define ANDROID_BMA222_SENSOR_H

#include <stdint.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include "sensors.h"
#include "SensorBase.h"
#include "InputEventReader.h"

/*****************************************************************************/


struct bma222acc_t {
        short x, /**< holds x-axis acceleration data sign extended. Range -512 to 511. */
              y, /**< holds y-axis acceleration data sign extended. Range -512 to 511. */
              z; /**< holds z-axis acceleration data sign extended. Range -512 to 511. */
} ;

/* bma ioctl command label */
#define IOCTL_BMA_GET_ACC_VALUE     0
#define DCM_IOC_MAGIC               's'
#define IOC_SET_ACCELEROMETER       _IO (DCM_IOC_MAGIC, 0x64)
#define BMA150_CALIBRATION          _IOWR(DCM_IOC_MAGIC,48,short)

#define BMA_POWER_OFF               0
#define BMA_POWER_ON                1

struct input_event;

class Bma222Sensor : public SensorBase {
    int mEnabled;
    InputEventCircularReader mInputReader;
    sensors_event_t mPendingEvent;
    bool mHasPendingEvent;
    char input_sysfs_path[PATH_MAX];
    int input_sysfs_path_len;


public:
            Bma222Sensor();
    virtual ~Bma222Sensor();
    virtual int readEvents(sensors_event_t* data, int count);
    virtual bool hasPendingEvents() const;
    virtual int setDelay(int32_t handle, int64_t ns);
    virtual int enable(int32_t handle, int enabled);
};

/*****************************************************************************/

#endif  // ANDROID_GYRO_SENSOR_H
