#pragma once
//=======CONFIGURATION=SECTION========
//#define MY_DEBUG

#include <variant.h>
// Choose Options 
//#define SHT_HUM   // Only if use SHT20 for temperature and humid
// Board choose. You have to change board in platformio.ini too
#define NRF52811   
//#define NRF52832 
//#define NRF52840   
// Move mode. If yos use move sensor
#define Move_Mode
// Led mode. Enable indications
#define Led_mode
// Counter impulse. You should add interrup for it
//#define YF_201 
// Enable serial output
//#define SERIAL_PRINT
// Mysensors node options
#define MY_NODE_ID 15 //здесь задается id ноды, если необходимо что бы id выдавал гейт, то нужно закомментировать данную строку
#define MY_RADIO_NRF5_ESB
#define MY_TRANSPORT_WAIT_READY_MS (myTransportComlpeteMS)
int16_t myTransportComlpeteMS = 10000;
//


//#define MY_NRF5_ESB_CHANNEL (125)
//#define MY_NRF5_ESB_MODE (NRF5_1MBPS)

#ifdef SHT_HUM
#include "DFRobot_SHT20.h"
#endif

#ifdef SHT_HUM
#define CHILD_ID_TEMP 0
#define CHILD_ID_HUM 1
#endif

#define CHILD_ID_MOVE 11
#define MY_SEND_RSSI 100
#define MY_SEND_BATTERY 99
#define MY_SEND_RESET_REASON 105
#define MY_RESET_REASON_TEXT
#define BUTTON_S_ID 12
#define YF201_ID 15

//#define MY_SEND_BATTERY_VOLTAGE // Enable/Disable auto send status battary and RSSI


#define CHILD_ID_VIRT 3   // Нужна виртуальная нода для работы библиотеки
//====================================

#include <MySensors.h>
#include "efektaGpiot.h"
#include "efektaHappyNode.h"

#ifdef SHT_HUM
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);
MyMessage msgHum(CHILD_ID_HUM, V_HUM);
#endif // SHT_HUM
#ifdef Move_Mode
MyMessage msgMove(CHILD_ID_MOVE, V_VAR1);
#endif // Move_Mode
#ifdef YF_201
MyMessage msgYF201(YF201_ID, V_VAR1);
#endif // YF_201
MyMessage msgVirt(CHILD_ID_VIRT, V_VAR);
MyMessage sendButtonMsg(BUTTON_S_ID, V_VAR1);

CHappyNode happyNode(100); // Адрес c которого будут храниться пользовательские данные
#ifndef Move_Mode
CDream interruptedSleep(1);  // Добавление пробуждения от пин. В (n) n - количество пинов от которых пробуждается (описывается в setup)
#endif // Move_Mode
#ifdef Move_Mode
CDream interruptedSleep(2);
#endif // Move_Mode
#ifdef SHT_HUM
extern DFRobot_SHT20    sht20;
void SHT_read();
void SHT_send();
#endif // SHT_HUM

extern bool button_state;

void blink (uint8_t flash);
void BUTTON_send();
void MOVE_send();
void YF201_send();
