#include "board.h"
#include "ch.h"
#include "hal.h"
#include "uart.h"
#include "led.h"
#include "radio_lvl1.h"
#include "kl_lib.h"
#include "MsgQ.h"
#include "SimpleSensors.h"
#include "ws2812b.h"
#include "Effects.h"
#include "Charger.h"

#if 1 // ======================== Variables and defines ========================
// Forever
EvtMsgQ_t<EvtMsg_t, MAIN_EVT_Q_LEN> EvtQMain;
static const UartParams_t CmdUartParams(115200, CMD_UART_PARAMS);
CmdUart_t Uart{&CmdUartParams};
static void ITask();
static void OnCmd(Shell_t *PShell);

static const NeopixelParams_t NpxParams {NPX_SPI, NPX_DATA_PIN, NPX_DMA, NPX_DMA_MODE(NPX_DMA_CHNL), NPX_CNT, npxRGB};
Neopixels_t Leds{&NpxParams};

static Charger_t Charger;

// ==== Timers ====
static TmrKL_t TmrEverySecond {TIME_MS2I(1000), evtIdEverySecond, tktPeriodic};
static int32_t TimeS;
#endif

int main(void) {
#if 1 // ==== Init Vcore & clock system ====
    Clk.EnablePrefeth();
    if(Clk.EnableHSE() == retvOk) {
//        Clk.SetVoltageRange(mvrHiPerf);
        Clk.SetupFlashLatency(10, mvrHiPerf);
        // 12MHz / 3 = 4; 4 * 20 / 8 = 10
        Clk.SetupPllMulDiv(3, 20, 8, 2);
        Clk.SetupBusDividers(ahbDiv1, apbDiv1, apbDiv1);
        if(Clk.EnablePLL() == retvOk) {
            Clk.EnablePLLROut();
            Clk.SwitchToPLL();
        }
    }
    Clk.UpdateFreqValues();
#endif

    // === Init OS ===
    halInit();
    chSysInit();
    EvtQMain.Init();

    // ==== Init hardware ====
    Uart.Init();
    Printf("\r%S %S\r", APP_NAME, XSTRINGIFY(BUILD_TIME));
    Clk.PrintFreqs();

    Leds.Init();

//    SimpleSensors::Init();
//    Adc.Init();

    Charger.Init();
    Effects::Init();

    TmrEverySecond.StartOrRestart();

    // ==== Radio ====
    if(Radio.Init() == retvOk) {} //Led.StartOrRestart(lsqStart);
    //else Led.StartOrRestart(lsqFailure);
    chThdSleepMilliseconds(1008);

    // Main cycle
    ITask();
}

__noreturn
void ITask() {
    while(true) {
        EvtMsg_t Msg = EvtQMain.Fetch(TIME_INFINITE);
        switch(Msg.ID) {
            case evtIdEverySecond:
                TimeS++;
//                Printf("S");
                Leds.SetCurrentColors();
                break;

            case evtIdRCmd:
                Printf("Cmd: %u\r", Msg.Value);
                if(Msg.Value == RCMD_POWER_ON) Effects::PowerOn();
                else if(Msg.Value == RCMD_POWER_OFF) Effects::PowerOff();
                break;

            case evtIdLedsDone:
                Printf("LedsDone\r");
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

    else if(PCmd->NameIs("On"))  Effects::PowerOn();
    else if(PCmd->NameIs("Off")) Effects::PowerOff();

    else if(PCmd->NameIs("Params")) {
        if(PCmd->GetNext<int32_t>(&Params.SpeedMin) != retvOk) return;
        if(PCmd->GetNext<int32_t>(&Params.SpeedMax) != retvOk) return;
        if(PCmd->GetNext<int32_t>(&Params.SizeMin)  != retvOk) return;
        if(PCmd->GetNext<int32_t>(&Params.SizeMax)  != retvOk) return;
        if(PCmd->GetNext<int32_t>(&Params.DelayMin) != retvOk) return;
        if(PCmd->GetNext<int32_t>(&Params.DelayMax) != retvOk) return;
        PShell->Ack(0);
    }

    else if(PCmd->NameIs("Clr")) {
        Color_t Clr;
        if(PCmd->GetNext<uint8_t>(&Clr.R) != retvOk) return;
        if(PCmd->GetNext<uint8_t>(&Clr.G) != retvOk) return;
        if(PCmd->GetNext<uint8_t>(&Clr.B) != retvOk) return;
//            Clr.Print();
//            PrintfEOL();
        if(Leds.TransmitDone) {
            Leds.SetAll(Clr);
            Leds.SetCurrentColors();
            PShell->Ack(0);
        }
        else Printf("Busy\r");
    }

    else PShell->Ack(retvCmdUnknown);
}
#endif
