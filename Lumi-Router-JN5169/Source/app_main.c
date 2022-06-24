/****************************************************************************
 *
 * MODULE:               Lumi Router
 *
 * COMPONENT:            app_main.c
 *
 * DESCRIPTION:          Application main file
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
 * Copyright NXP B.V. 2017. All rights reserved
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include <jendefs.h>

/* Application */
#include "app_device_temperature.h"
#include "app_main.h"
#include "app_router_node.h"
#include "app_serial_commands.h"
#include "app_zcl_task.h"

/* SDK JN-SW-4170 */
#include "AppHardwareApi.h"
#include "ZQueue.h"
#include "ZTimer.h"
#include "bdb_api.h"
#include "mac_vs_sap.h"
#include "portmacro.h"
#include "zps_apl_af.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#ifdef DEBUG_APP
#define TRACE_APP TRUE
#else
#define TRACE_APP FALSE
#endif

#define APP_ZTIMER_STORAGE 3

#define BDB_QUEUE_SIZE       2
#define MLME_QUEQUE_SIZE     8
#define MCPS_QUEUE_SIZE      20
#define TIMER_QUEUE_SIZE     8
#define MCPS_DCFM_QUEUE_SIZE 5
#define TX_QUEUE_SIZE        150
#define RX_QUEUE_SIZE        150

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

PUBLIC uint8 u8TimerTick;
PUBLIC uint8 u8TimerRestart;
PUBLIC uint8 u8TimerDeviceTemperature;

PUBLIC tszQueue APP_msgBdbEvents;
PUBLIC tszQueue APP_msgAppEvents;
PUBLIC tszQueue APP_msgSerialTx;
PUBLIC tszQueue APP_msgSerialRx;

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

PRIVATE ZTIMER_tsTimer asTimers[APP_ZTIMER_STORAGE + BDB_ZTIMER_STORAGE];

PRIVATE BDB_tsZpsAfEvent asBdbEvent[BDB_QUEUE_SIZE];
PRIVATE MAC_tsMlmeVsDcfmInd asMacMlmeVsDcfmInd[MLME_QUEQUE_SIZE];
PRIVATE MAC_tsMcpsVsDcfmInd asMacMcpsDcfmInd[MCPS_QUEUE_SIZE];
PRIVATE zps_tsTimeEvent asTimeEvent[TIMER_QUEUE_SIZE];
PRIVATE MAC_tsMcpsVsCfmData asMacMcpsDcfm[MCPS_DCFM_QUEUE_SIZE];
PRIVATE uint8 au8TxBuffer[TX_QUEUE_SIZE];
PRIVATE uint8 au8RxBuffer[RX_QUEUE_SIZE];

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

extern void zps_taskZPS(void);
extern void PWRM_vManagePower(void);

/****************************************************************************
 *
 * NAME: APP_vMainLoop
 *
 * DESCRIPTION:
 * Main application loop
 *
 ****************************************************************************/
PUBLIC void APP_vMainLoop(void)
{
    while (TRUE) {
        zps_taskZPS();

        bdb_taskBDB();

        ZTIMER_vTask();

        APP_taskAtSerial();

        /* Re-load the watch-dog timer. Execution must return through the idle
         * task before the CPU is suspended by the power manager. This ensures
         * that at least one task / ISR has executed within the watchdog period
         * otherwise the system will be reset. */
        vAHI_WatchdogRestart();

        /* suspends CPU operation when the system is idle or puts the device to
         * sleep if there are no activities in progress */
        PWRM_vManagePower();
    }
}

/****************************************************************************
 *
 * NAME: APP_vSetUpHardware
 *
 * DESCRIPTION:
 * Set up interrupts
 *
 ****************************************************************************/
PUBLIC void APP_vSetUpHardware(void)
{
    TARGET_INITIALISE();
    /* clear interrupt priority level */
    SET_IPL(0);
    portENABLE_INTERRUPTS();
}

/****************************************************************************
 *
 * NAME: APP_vInitResources
 *
 * DESCRIPTION:
 * Initialise resources (timers, queue's etc)
 *
 ****************************************************************************/
PUBLIC void APP_vInitResources(void)
{
    /* Initialise the Z timer module */
    ZTIMER_eInit(asTimers, sizeof(asTimers) / sizeof(ZTIMER_tsTimer));

    /* Create Z timers */
    ZTIMER_eOpen(&u8TimerTick, APP_cbTimerZclTick, NULL, ZTIMER_FLAG_PREVENT_SLEEP);
    ZTIMER_eOpen(&u8TimerRestart, APP_cbTimerRestart, NULL, ZTIMER_FLAG_PREVENT_SLEEP);
    ZTIMER_eOpen(&u8TimerDeviceTemperature, APP_cbTimerDeviceTemperatureUpdate, NULL, ZTIMER_FLAG_PREVENT_SLEEP);

    /* Create all the queues */
    ZQ_vQueueCreate(&APP_msgBdbEvents, BDB_QUEUE_SIZE, sizeof(BDB_tsZpsAfEvent), (uint8 *)asBdbEvent);
    ZQ_vQueueCreate(&zps_msgMlmeDcfmInd, MLME_QUEQUE_SIZE, sizeof(MAC_tsMlmeVsDcfmInd), (uint8 *)asMacMlmeVsDcfmInd);
    ZQ_vQueueCreate(&zps_msgMcpsDcfmInd, MCPS_QUEUE_SIZE, sizeof(MAC_tsMcpsVsDcfmInd), (uint8 *)asMacMcpsDcfmInd);
    ZQ_vQueueCreate(&zps_TimeEvents, TIMER_QUEUE_SIZE, sizeof(zps_tsTimeEvent), (uint8 *)asTimeEvent);
    ZQ_vQueueCreate(&zps_msgMcpsDcfm, MCPS_DCFM_QUEUE_SIZE, sizeof(MAC_tsMcpsVsCfmData), (uint8 *)asMacMcpsDcfm);
    ZQ_vQueueCreate(&APP_msgSerialTx, TX_QUEUE_SIZE, sizeof(uint8), (uint8 *)au8TxBuffer);
    ZQ_vQueueCreate(&APP_msgSerialRx, RX_QUEUE_SIZE, sizeof(uint8), (uint8 *)au8RxBuffer);
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
