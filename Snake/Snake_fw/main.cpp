#include "board.h"
#include "ch.h"
#include "hal.h"
#include "uart.h"
#include "led.h"
#include "vibro.h"
#include "Sequences.h"
#include "radio_lvl1.h"
#include "kl_i2c.h"
#include "kl_lib.h"
#include "MsgQ.h"
//#include "main.h"
#include "SimpleSensors.h"
#include "buttons.h"

#if 1 // ======================== Variables and defines ========================
// Forever
EvtMsgQ_t<EvtMsg_t, MAIN_EVT_Q_LEN> EvtQMain;
static const UartParams_t CmdUartParams(115200, CMD_UART_PARAMS);
CmdUart_t Uart{&CmdUartParams};
static void ITask();
static void OnCmd(Shell_t *PShell);

// ==== Periphery ====
//LedRGB_t Led { LED_R_PIN, LED_G_PIN, LED_B_PIN };

// ==== Timers ====
//static TmrKL_t TmrEverySecond {MS2ST(1000), evtIdEverySecond, tktPeriodic};
//static TmrKL_t TmrRxTableCheck {MS2ST(2007), evtIdCheckRxTable, tktPeriodic};
//static int32_t TimeS;
#endif

int main(void) {
    // ==== Init Vcore & clock system ====
//    Clk.SetCoreClk(cclk16MHz);
//    Clk.SetCoreClk(cclk48MHz);
    Clk.UpdateFreqValues();

    // === Init OS ===
    halInit();
    chSysInit();
    EvtQMain.Init();

    // ==== Init hardware ====
    Uart.Init();
    Printf("\r%S %S\r", APP_NAME, XSTRINGIFY(BUILD_TIME));
    Clk.PrintFreqs();

//    i2c1.Init();
//    i2c1.ScanBus();

//    Led.Init();
//    Led.StartOrRestart(lsqStart);

//    SimpleSensors::Init();
//    Adc.Init();

    // ==== Time and timers ====
//    TmrEverySecond.StartOrRestart();
//    TmrRxTableCheck.StartOrRestart();

    // ==== Radio ====
//    if(Radio.Init() == retvOk) Led.StartOrRestart(lsqStart);
//    else Led.StartOrRestart(lsqFailure);
//    chThdSleepMilliseconds(1008);

    // Main cycle
    ITask();
}

__noreturn
void ITask() {
    while(true) {
        EvtMsg_t Msg = EvtQMain.Fetch(TIME_INFINITE);
        switch(Msg.ID) {
//            case evtIdEverySecond:
//                TimeS++;
//                ReadAndSetupMode();
//                break;

            case evtIdButtons:
//                Printf("Btn %u\r", Msg.BtnEvtInfo.Type);
                break;

            case evtIdShellCmd:
                OnCmd((Shell_t*)Msg.Ptr);
                ((Shell_t*)Msg.Ptr)->SignalCmdProcessed();
                break;
            default: Printf("Unhandled Msg %u\r", Msg.ID); break;
        } // Switch
    } // while true
} // ITask()

void ProcessCharging(PinSnsState_t *PState, uint32_t Len) {

}

#if 1 // ================= Command processing ====================
void OnCmd(Shell_t *PShell) {
	Cmd_t *PCmd = &PShell->Cmd;
    __attribute__((unused)) int32_t dw32 = 0;  // May be unused in some configurations
//    Uart.Printf("%S\r", PCmd->Name);
    // Handle command
    if(PCmd->NameIs("Ping")) {
        PShell->Ack(retvOk);
    }
    else if(PCmd->NameIs("Version")) PShell->Print("%S %S\r", APP_NAME, XSTRINGIFY(BUILD_TIME));

    else PShell->Ack(retvCmdUnknown);
}
#endif
