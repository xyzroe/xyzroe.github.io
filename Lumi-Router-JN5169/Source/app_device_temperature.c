/****************************************************************************
 *
 * MODULE:              Lumi Router
 *
 * COMPONENT:           app_device_temperature.c
 *
 * DESCRIPTION:         Set of functions/task for read device temperature
 *
 ****************************************************************************
 *
 * This software is owned by NXP B.V. and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on NXP products [NXP Microcontrollers such as JN5168, JN5179].
 * You, and any third parties must reproduce the copyright and warranty notice
 * and any other legend of ownership on each copy or partial copy of the
 * software.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright NXP B.V. 2016. All rights reserved
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include <jendefs.h>

/* Application */
#include "app_device_temperature.h"
#include "app_main.h"
#include "app_zcl_task.h"

/* SDK JN-SW-4170 */
#include "AppHardwareApi.h"
#include "ZTimer.h"
#include "dbg.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#ifdef DEBUG_DEVICE_TEMPERATURE
#define TRACE_DEVICE_TEMPERATURE TRUE
#else
#define TRACE_DEVICE_TEMPERATURE FALSE
#endif

#define DEVICE_TEMPERATURE_UPDATE_TIME ZTIMER_TIME_SEC(10)

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

PRIVATE void APP_vDeviceTemperatureUpdate(void);
PRIVATE int16 APP_i16GetDeviceTemperature(void);
PRIVATE int16 APP_i16ConvertChipTemp(uint16 u16AdcValue);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: APP_vDeviceTemperatureInit
 *
 * DESCRIPTION:
 * Init Device Temperature
 *
 ****************************************************************************/
PUBLIC void APP_vDeviceTemperatureInit(void)
{
    vAHI_ApConfigure(E_AHI_AP_REGULATOR_ENABLE,
                     E_AHI_AP_INT_DISABLE,
                     E_AHI_AP_SAMPLE_8,
                     E_AHI_AP_CLOCKDIV_500KHZ,
                     E_AHI_AP_INTREF);

    while (!bAHI_APRegulatorEnabled())
        ;

    APP_vDeviceTemperatureUpdate();

    DBG_vPrintf(TRACE_DEVICE_TEMPERATURE, "APP: Init Device Temperature\n");

    /* Start the Device Temperature timer */
    ZTIMER_eStart(u8TimerDeviceTemperature, DEVICE_TEMPERATURE_UPDATE_TIME);
}

/****************************************************************************
 *
 * NAME: APP_cbTimerDeviceTemperatureUpdate
 *
 * DESCRIPTION:
 * CallBack For Device Temperature Update timer
 *
 ****************************************************************************/
PUBLIC void APP_cbTimerDeviceTemperatureUpdate(void *pvParam)
{
    APP_vDeviceTemperatureUpdate();
    ZTIMER_eStart(u8TimerDeviceTemperature, DEVICE_TEMPERATURE_UPDATE_TIME);
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: APP_vDeviceTemperatureUpdate
 *
 * DESCRIPTION:
 * Device Temperature update
 *
 ****************************************************************************/
PRIVATE void APP_vDeviceTemperatureUpdate(void)
{
    int16 i16DeviceTemperature = APP_i16GetDeviceTemperature();

    DBG_vPrintf(TRACE_DEVICE_TEMPERATURE, "APP: Temp = %d C\n", i16DeviceTemperature);

    sLumiRouter.sDeviceTemperatureConfigurationServerCluster.i16CurrentTemperature = i16DeviceTemperature;
}

/****************************************************************************
 *
 * NAME: APP_i16GetDeviceTemperature
 *
 * DESCRIPTION:
 * Read Device Temperature
 *
 * RETURNS:
 * Device Temperature
 *
 ****************************************************************************/
PRIVATE int16 APP_i16GetDeviceTemperature(void)
{
    uint16 u16AdcTempSensor;

    vAHI_AdcEnable(E_AHI_ADC_SINGLE_SHOT, E_AHI_AP_INPUT_RANGE_2, E_AHI_ADC_SRC_TEMP);
    vAHI_AdcStartSample();

    while (bAHI_AdcPoll())
        ;

    u16AdcTempSensor = u16AHI_AdcRead();

    return APP_i16ConvertChipTemp(u16AdcTempSensor);
}

/****************************************************************************
 *
 * NAME: APP_i16ConvertChipTemp
 *
 * DESCRIPTION:
 * Helper Function to convert 10bit ADC reading to degrees C
 * Formula: DegC = Typical DegC - ((Reading12 - Typ12) * ScaleFactor)
 * Where C = 25 and temps sensor output 730mv at 25C (from datasheet)
 * As we use 2Vref and 10bit adc this gives (730/2400)*4096  [=Typ12 =1210]
 * Scale factor is half the 0.706 data-sheet resolution DegC/LSB (2Vref)
 *
 * PARAMETERS:      Name            Usage
 * uint16           u16AdcValue     Adc Temperature Value
 *
 * RETURNS:
 * Chip Temperature in DegC
 *
 ****************************************************************************/
PRIVATE int16 APP_i16ConvertChipTemp(uint16 u16AdcValue)
{
    return (int16)((int32)25 - ((((int32)(u16AdcValue * 4) - (int32)1210) * (int32)353) / (int32)1000));
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
