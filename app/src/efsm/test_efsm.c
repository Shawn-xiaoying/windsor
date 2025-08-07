#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// #include <unistd.h>
#include "efsm.h"
#include "mt_gpio.h"
#define NRF_LOG_MODULE_NAME test_efsm
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

typedef struct
{
    efsm_t super;
    uint16_t timeCount;
    uint32_t globalTick;
} led_t;
led_t ao_led;
#define AO (efsm_t *)led

void Led_StateOn(led_t *led, uint16_t evt);
void Led_StateOff(led_t *led, uint16_t evt);

void Led_Ctor(led_t *led)
{
    Efsm_Ctor((efsm_t *)led, (efsmState_t)Led_StateOn);
    led->globalTick = 0;
    EFSM_REG_STATE(Led_StateOn); // ��ǰע��state������Efsm_Trans()�Ĳ������
    EFSM_REG_STATE(Led_StateOff);
}

void Led_StateOn(led_t *led, uint16_t evt)
{
    switch (evt)
    {
    case EFSM_EVT_EXIT:
        NRF_LOG_INFO("%d:Led_StateOn, Exit", led->globalTick);
        break;
    case EFSM_EVT_ENTRY:
        led->timeCount = 0;
        NRF_LOG_INFO("%d:Led_StateOn, Entry", led->globalTick);
        RUN_LED_ON();
        break;
    case EFSM_EVT_TICK:
        ++led->globalTick;
        if (++led->timeCount >= 2)
        {
            EFSM_TRANS(Led_StateOff);
        }
        break;
    }
}
void Led_StateOff(led_t *led, uint16_t evt)
{
    switch (evt)
    {
    case EFSM_EVT_EXIT:
        NRF_LOG_INFO("%d:Led_StateOff, Exit", led->globalTick);
        break;
    case EFSM_EVT_ENTRY:
        led->timeCount = 0;
        NRF_LOG_INFO("%d:Led_StateOff, Entry", led->globalTick);
        RUN_LED_OFF();
        break;
    case EFSM_EVT_TICK:
        ++led->globalTick;
        if (++led->timeCount >= 2)
        {
            EFSM_TRANS(Led_StateOn);
        }
        break;
    }
}
void init_led_efsm(void)
{

    Led_Ctor(&ao_led);
}

int test_efsm_task(void)
{

    {
        Efsm_EvtTrig((efsm_t *)&ao_led, EFSM_EVT_TICK);
        Efsm_Hand((efsm_t *)&ao_led);
        //    Sleep(1);
    }
    NRF_LOG_INFO("esfm_task");
    // system("pause");
    return 0;
}
