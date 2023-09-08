/*
    NeaPolis Innovation Summer Campus 2023 Examples
    Copyright (C) 2020-2023 Salvatore Dello Iacono [delloiaconos@gmail.com]
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
        http://www.apache.org/licenses/LICENSE-2.0
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/*
 * adcmanager.h - Adc Manager Header File
*/
#ifndef __ADCMANAGER_H__
#define __ADCMANAGER_H__

#include "hal.h"

/*   ADC Analog Watchdog Trigger values
 * Check the reference manual when changing...
 */
#define ADCMGR_ADW1_LTH         0U
#define ADCMGR_ADW1_HTH         2048U


/* AdcManager defines */
#define ADCMANAGER_TIMEOUT          TIME_S2I(5)
#define ADCMANAGER_HOLDOFF          TIME_S2I(2)

/* AdcBlinker defines */
#define ADCMGR_LED_LINE             LINE_LED_GREEN

#define ADCBLINKER_DEF_PERIOD       TIME_MS2I(2000)
#define ADCBLINKER_HIGH_PERIOD      TIME_MS2I(200)
#define ADCBLINKER_LOW_PERIOD       TIME_MS2I(800)

thread_t * startAdcManager( adcsample_t ** samples_ptr, size_t * len  );

/* AdcManager Messages. */
#define ADCMGR_MSG_ADW1     ( (msg_t) ADC_ERR_AWD1)
#define ADCMGR_MSG_KO       ( (msg_t) ADC_ERR_AWD1+5 )

#endif // __ADCMANAGER_H__
