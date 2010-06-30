/*
 * Copyright (C) 2010 The Android Open Source Project
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

/* DeviceVolume implementation */

#include "sles_allinclusive.h"

static SLresult IDeviceVolume_GetVolumeScale(SLDeviceVolumeItf self, SLuint32 deviceID,
    SLint32 *pMinValue, SLint32 *pMaxValue, SLboolean *pIsMillibelScale)
{
    switch (deviceID) {
    case SL_DEFAULTDEVICEID_AUDIOINPUT:
    case SL_DEFAULTDEVICEID_AUDIOOUTPUT:
    // FIXME move these to device-specific or platform-specific file
    case DEVICE_ID_HEADSET:
    case DEVICE_ID_HANDSFREE:
        break;
    default:
        return SL_RESULT_PARAMETER_INVALID;
    }
    if (NULL != pMinValue)
        *pMinValue = 0;
    if (NULL != pMaxValue)
        *pMaxValue = 10;
    if (NULL != pIsMillibelScale)
        *pIsMillibelScale = SL_BOOLEAN_FALSE;
    return SL_RESULT_SUCCESS;
}

static SLresult IDeviceVolume_SetVolume(SLDeviceVolumeItf self, SLuint32 deviceID, SLint32 volume)
{
    switch (deviceID) {
    case SL_DEFAULTDEVICEID_AUDIOINPUT:
    case SL_DEFAULTDEVICEID_AUDIOOUTPUT:
        break;
    // FIXME These are treated same as generic audio output for now
    case DEVICE_ID_HEADSET:
    case DEVICE_ID_HANDSFREE:
        deviceID = SL_DEFAULTDEVICEID_AUDIOOUTPUT;
        break;
    default:
        return SL_RESULT_PARAMETER_INVALID;
    }
    IDeviceVolume *this = (IDeviceVolume *) self;
    interface_lock_poke(this);
    this->mVolume[~deviceID] = volume;
    interface_unlock_poke(this);
    return SL_RESULT_SUCCESS;
}

static SLresult IDeviceVolume_GetVolume(SLDeviceVolumeItf self, SLuint32 deviceID, SLint32 *pVolume)
{
    if (NULL == pVolume)
        return SL_RESULT_PARAMETER_INVALID;
    switch (deviceID) {
    case SL_DEFAULTDEVICEID_AUDIOINPUT:
    case SL_DEFAULTDEVICEID_AUDIOOUTPUT:
        break;
    // FIXME These are treated same as generic audio output for now
    case DEVICE_ID_HEADSET:
    case DEVICE_ID_HANDSFREE:
        deviceID = SL_DEFAULTDEVICEID_AUDIOOUTPUT;
        break;
    default:
        return SL_RESULT_PARAMETER_INVALID;
    }
    IDeviceVolume *this = (IDeviceVolume *) self;
    interface_lock_peek(this);
    SLint32 volume = this->mVolume[~deviceID];
    interface_unlock_peek(this);
    *pVolume = volume;
    return SL_RESULT_SUCCESS;
}

static const struct SLDeviceVolumeItf_ IDeviceVolume_Itf = {
    IDeviceVolume_GetVolumeScale,
    IDeviceVolume_SetVolume,
    IDeviceVolume_GetVolume
};

void IDeviceVolume_init(void *self)
{
    IDeviceVolume *this = (IDeviceVolume *) self;
    this->mItf = &IDeviceVolume_Itf;
    // FIXME hard-coded array size for default in/out
    this->mVolume[0] = 10;
    this->mVolume[1] = 10;
}
