/*
 * radio_lvl1.cpp
 *
 *  Created on: Nov 17, 2013
 *      Author: kreyl
 */

#include "radio_lvl1.h"
#include "cc1101.h"
#include "uart.h"
#include "led.h"
#include "MsgQ.h"

cc1101_t CC(CC_Setup0);

//#define DBG_PINS

#ifdef DBG_PINS
#define DBG_GPIO1   GPIOB
#define DBG_PIN1    10
#define DBG1_SET()  PinSetHi(DBG_GPIO1, DBG_PIN1)
#define DBG1_CLR()  PinSetLo(DBG_GPIO1, DBG_PIN1)
#define DBG_GPIO2   GPIOB
#define DBG_PIN2    9
#define DBG2_SET()  PinSetHi(DBG_GPIO2, DBG_PIN2)
#define DBG2_CLR()  PinSetLo(DBG_GPIO2, DBG_PIN2)
#else
#define DBG1_SET()
#define DBG1_CLR()
#endif

rLevel1_t Radio;

#if 1 // ================================ Task =================================
static THD_WORKING_AREA(warLvl1Thread, 256);
__noreturn
static void rLvl1Thread(void *arg) {
    chRegSetThreadName("rLvl1");
    Radio.ITask();
}
#endif // task

__noreturn
void rLevel1_t::ITask() {
    while(true) {
        chThdSleepMilliseconds(540);
        CC.Recalibrate();
        if(CC.Receive(45, &PktRx, RPKT_LEN, &Rssi) == retvOk) {
//            Printf("Cmd=%u; Rssi=%d\r", PktRx.Cmd, Rssi);
            if(PktRx.Cmd == RCMD_POWER_ON or PktRx.Cmd == RCMD_POWER_OFF) {
                EvtQMain.SendNowOrExit(EvtMsg_t(evtIdRCmd, PktRx.Cmd));
                PktTx.Reply = retvOk;
            }
            else PktTx.Reply = retvCmdUnknown;
            CC.Transmit(&PktTx, RPKT_LEN);
        } // if RX ok
    } // while true
}

#if 1 // ============================
uint8_t rLevel1_t::Init() {
#ifdef DBG_PINS
    PinSetupOut(DBG_GPIO1, DBG_PIN1, omPushPull);
    PinSetupOut(DBG_GPIO2, DBG_PIN2, omPushPull);
#endif

    if(CC.Init() == retvOk) {
        CC.SetTxPower(CC_Pwr0dBm);
        CC.SetPktSize(RPKT_LEN);
        CC.SetChannel(0);
        chThdCreateStatic(warLvl1Thread, sizeof(warLvl1Thread), HIGHPRIO, (tfunc_t)rLvl1Thread, NULL);
        return retvOk;
    }
    else return retvFail;
}
#endif
