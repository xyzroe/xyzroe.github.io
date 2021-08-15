/****************************************************************************
 *
 * MODULE:              Lumi Router
 *
 * COMPONENT:           app_serial_commands.c
 *
 * DESCRIPTION:         Serial Commands
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
#include "app_main.h"
#include "app_serial_commands.h"
#include "uart.h"

/* SDK JN-SW-4170 */
#include "PDM.h"
#include "ZQueue.h"
#include "ZTimer.h"
#include "dbg.h"
#include "portmacro.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#ifdef DEBUG_SERIAL
#define TRACE_SERIAL TRUE
#else
#define TRACE_SERIAL FALSE
#endif

#define SL_START_CHAR 0x01
#define SL_ESC_CHAR   0x02
#define SL_END_CHAR   0x03

#define MAX_PACKET_SIZE 32

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/* Enumerated list of states for receive state machine */
typedef enum {
    E_STATE_RX_WAIT_START,
    E_STATE_RX_WAIT_TYPEMSB,
    E_STATE_RX_WAIT_TYPELSB,
    E_STATE_RX_WAIT_LENMSB,
    E_STATE_RX_WAIT_LENLSB,
    E_STATE_RX_WAIT_CRC,
    E_STATE_RX_WAIT_DATA
} APP_teRxState;

/* Serial link message types */
typedef enum { E_SC_MSG_RESET = 0x0011, E_SC_MSG_ERASE_PERSISTENT_DATA = 0x0012 };

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

PRIVATE void APP_vProcessRxChar(uint8 u8Char);
PRIVATE void APP_vProcessCommand(void);
PRIVATE void APP_vWriteTxChar(uint8 u8Char);
PRIVATE uint8 APP_u8CalculateCRC(uint16 u16Type, uint16 u16Length, uint8 *pu8Data);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

PRIVATE uint8 au8LinkRxBuffer[32];
PRIVATE uint16 u16PacketType;
PRIVATE uint16 u16PacketLength;
PRIVATE uint32 sStorage;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: APP_taskAtSerial
 *
 * DESCRIPTION:
 * Task that obtains a message from the serial Rx message queue.
 *
 ****************************************************************************/
PUBLIC void APP_taskAtSerial(void)
{
    uint8 u8RxByte;
    if (ZQ_bQueueReceive(&APP_msgSerialRx, &u8RxByte)) {
        APP_vProcessRxChar(u8RxByte);
    }
}

/****************************************************************************
 *
 * NAME: APP_WriteMessageToSerial
 *
 * DESCRIPTION:
 * Write message to the serial link
 *
 ****************************************************************************/
PUBLIC void APP_WriteMessageToSerial(const char *message)
{
    DBG_vPrintf(TRACE_SERIAL, "APP_WriteMessageToSerial(%s)\n", message);

    for (; *message != '\0'; message++) {
        APP_vWriteTxChar(*message);
    }
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: APP_vProcessRxChar
 *
 * DESCRIPTION:
 * Processes the received character
 *
 ****************************************************************************/
PRIVATE void APP_vProcessRxChar(uint8 u8Char)
{
    static APP_teRxState eRxState = E_STATE_RX_WAIT_START;
    static uint8 u8CRC;
    static uint16 u16Bytes;
    static bool bInEsc = FALSE;

    switch (u8Char) {
    case SL_START_CHAR:
        /* Reset state machine */
        u16Bytes = 0;
        bInEsc = FALSE;
        DBG_vPrintf(TRACE_SERIAL, "RX Start\n");
        eRxState = E_STATE_RX_WAIT_TYPEMSB;
        break;

    case SL_ESC_CHAR:
        /* Escape next character */
        bInEsc = TRUE;
        break;

    case SL_END_CHAR:
        /* End message */
        DBG_vPrintf(TRACE_SERIAL, "Got END\n");
        eRxState = E_STATE_RX_WAIT_START;
        if (u16PacketLength < MAX_PACKET_SIZE) {
            if (u8CRC == APP_u8CalculateCRC(u16PacketType, u16PacketLength, au8LinkRxBuffer)) {
                /* CRC matches - valid packet */
                DBG_vPrintf(TRACE_SERIAL, "APP_vProcessRxChar(%d, %d, %02x)\n", u16PacketType, u16PacketLength, u8CRC);
                APP_vProcessCommand();
            }
        }
        DBG_vPrintf(TRACE_SERIAL, "CRC BAD\n");
        break;

    default:
        if (bInEsc) {
            /* Unescape the character */
            u8Char ^= 0x10;
            bInEsc = FALSE;
        }
        DBG_vPrintf(TRACE_SERIAL, "Data 0x%x\n", u8Char & 0xFF);

        switch (eRxState) {
        case E_STATE_RX_WAIT_START:
            break;

        case E_STATE_RX_WAIT_TYPEMSB:
            u16PacketType = (uint16)u8Char << 8;
            eRxState++;
            break;

        case E_STATE_RX_WAIT_TYPELSB:
            u16PacketType += (uint16)u8Char;
            DBG_vPrintf(TRACE_SERIAL, "Type 0x%x\n", u16PacketType & 0xFFFF);
            eRxState++;
            break;

        case E_STATE_RX_WAIT_LENMSB:
            u16PacketLength = (uint16)u8Char << 8;
            eRxState++;
            break;

        case E_STATE_RX_WAIT_LENLSB:
            u16PacketLength += (uint16)u8Char;
            DBG_vPrintf(TRACE_SERIAL, "Length %d\n", u16PacketLength);
            if (u16PacketLength > MAX_PACKET_SIZE) {
                DBG_vPrintf(TRACE_SERIAL, "Length > MaxLength\n");
                eRxState = E_STATE_RX_WAIT_START;
            }
            else {
                eRxState++;
            }
            break;

        case E_STATE_RX_WAIT_CRC:
            DBG_vPrintf(TRACE_SERIAL, "CRC %02x\n", u8Char);
            u8CRC = u8Char;
            eRxState++;
            break;

        case E_STATE_RX_WAIT_DATA:
            if (u16Bytes < u16PacketLength) {
                DBG_vPrintf(TRACE_SERIAL, "%02x ", u8Char);
                au8LinkRxBuffer[u16Bytes++] = u8Char;
            }
            break;
        }
        break;
    }
}

/****************************************************************************
 *
 * NAME: APP_vProcessCommand
 *
 * DESCRIPTION:
 * Processed the received command
 *
 ****************************************************************************/
PRIVATE void APP_vProcessCommand(void)
{
    switch (u16PacketType) {
    case E_SC_MSG_RESET:
        APP_WriteMessageToSerial("Reset...........");
        ZTIMER_eStart(u8TimerRestart, ZTIMER_TIME_MSEC(100));
        break;

    case E_SC_MSG_ERASE_PERSISTENT_DATA:
        APP_WriteMessageToSerial("Erase PDM.......");
        PDM_vDeleteAllDataRecords();
        APP_WriteMessageToSerial("Reset...........");
        ZTIMER_eStart(u8TimerRestart, ZTIMER_TIME_MSEC(100));
        break;

    default:
        break;
    }
}

/****************************************************************************
 *
 * NAME: APP_vWriteTxChar
 *
 * DESCRIPTION:
 * Write byte to the serial link
 *
 ****************************************************************************/
PRIVATE void APP_vWriteTxChar(uint8 u8Char)
{
    ZPS_eEnterCriticalSection(NULL, &sStorage);

    if (UART_bTxReady() && ZQ_bQueueIsEmpty(&APP_msgSerialTx)) {
        /* send byte now and enable irq */
        UART_vSetTxInterrupt(TRUE);
        UART_vTxChar(u8Char);
    }
    else {
        ZQ_bQueueSend(&APP_msgSerialTx, &u8Char);
    }

    ZPS_eExitCriticalSection(NULL, &sStorage);
}

/****************************************************************************
 *
 * NAME: APP_u8CalculateCRC
 *
 * DESCRIPTION:
 * Calculate CRC of packet
 *
 * PARAMETERS: Name                   RW  Usage
 *             u8Type                 R   Message type
 *             u16Length              R   Message length
 *             pu8Data                R   Message payload
 *
 * RETURNS:
 * CRC of packet
 *
 ****************************************************************************/
PRIVATE uint8 APP_u8CalculateCRC(uint16 u16Type, uint16 u16Length, uint8 *pu8Data)
{
    int n;
    uint8 u8CRC;

    u8CRC = (u16Type >> 0) & 0xff;
    u8CRC ^= (u16Type >> 8) & 0xff;
    u8CRC ^= (u16Length >> 0) & 0xff;
    u8CRC ^= (u16Length >> 8) & 0xff;

    for (n = 0; n < u16Length; n++) {
        u8CRC ^= pu8Data[n];
    }

    return (u8CRC);
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
