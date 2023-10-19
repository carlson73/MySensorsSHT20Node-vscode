// The code of a simple node SHT-20 for mysensors.
// Inspired by the works of Andrey Berk and Alexei Bogdanov 
// Working code but not yet fully tested for stability in different modes. Also, I am just start with the SrFatcat library.
// I never tire of saying thanks to Alexey for his work.
// (https://bitbucket.org/SrFatcat/mysefektalib/src/master/).

#include "main.h"

bool button_state = true;   // Button state
bool move_state = false;   // Button state
bool first_send = true;   // Button state
uint8_t countState = 0; 
uint8_t YF201count = 0;
uint32_t sleepingPeriod = 1 * 60 * 1000;  //первое число - минуты
uint8_t counterBattery = 0;              
uint8_t counterBatterySend = 2;          // Интервал отправки батареи и RSSI. 60 раз в час
const uint8_t shortWait = 40;
uint32_t configMillis = 1*60*1000;
uint32_t previousMillis;

#ifdef SHT_HUM
float tempThreshold = 0.1;             //  Интервал изменения для отправки    
float humThreshold = 0.2;
float temp;
float hum;
float old_temp;
float old_hum;
DFRobot_SHT20    sht20;
#endif // SHT_HUM


// #define Lut_Board          // Old style plate
#ifdef Lut_Board 
//#define BLUE_LED 14
#define PIN_WIRE_SDA 28
#define PIN_WIRE_SCL 25
#endif // Lut_Board
#ifdef Move_Mode
#define MOVE_INPUT_SENSOR 5
#endif // Move_Mode
#ifdef YF_201
#define YF201_INPUT_SENSOR 24
#endif // YF_201
//#define BLUE_LED 15
#define PIN_BUTTON 24

void preHwInit() {
    pinMode(PIN_BUTTON, INPUT);
    pinMode(BLUE_LED, OUTPUT);
    digitalWrite(BLUE_LED, HIGH);
    #ifdef Move_Mode
    pinMode(MOVE_INPUT_SENSOR, INPUT);      // sets the motion sensor digital pin as input
    #endif // Move_Mode
    #ifdef YF_201
    pinMode(YF201_INPUT_SENSOR, INPUT);      // sets the motion sensor digital pin as input
    #endif // Move_Mode
}

void before() {
#ifdef NRF52811 
NRF_POWER->DCDCEN = 1; //включение режима оптимизации питания, расход снижается на 40%, но должны быть установленны емкости (если нода сделана на модуле https://a.aliexpress.com/_mKN3t2f то нужно раскомментировать эту строку)
//NRF_NFCT->TASKS_DISABLE = 1; //останавливает таски, если они есть
NRF_NVMC->CONFIG = 1; //разрешить запись
//NRF_UICR->NFCPINS = 0; //отключает nfc и nfc пины становятся доступными для использования
#ifdef SERIAL_PRINT
    NRF_UART0->ENABLE = 1;  
#else
    NRF_UART0->ENABLE = 0;  
#endif // SERIAL_PRINT
NRF_NVMC->CONFIG = 0; //запретить запись
#endif // NRF52811

#ifdef NRF52832
NRF_POWER->DCDCEN = 1; //включение режима оптимизации питания, расход снижается на 40%, но должны быть установленны емкости (если нода сделана на модуле https://a.aliexpress.com/_mKN3t2f то нужно раскомментировать эту строку)
//NRF_NFCT->TASKS_DISABLE = 1; //останавливает таски, если они есть
NRF_NVMC->CONFIG = 1; //разрешить запись
//NRF_UICR->NFCPINS = 0; //отключает nfc и nfc пины становятся доступными для использования
#ifdef SERIAL_PRINT
    NRF_UART0->ENABLE = 1;  
#else
    NRF_UART0->ENABLE = 0;  
#endif // SERIAL_PRINT
NRF_NVMC->CONFIG = 0; //запретить запись
#endif // NRF52832

#ifdef NRF52840
NRF_POWER->DCDCEN = 1; //включение режима оптимизации питания, расход снижается на 40%, но должны быть установленны емкости (если нода сделана на модуле https://a.aliexpress.com/_mKN3t2f то нужно раскомментировать эту строку)
//NRF_NFCT->TASKS_DISABLE = 1; //останавливает таски, если они есть
NRF_NVMC->CONFIG = 1; //разрешить запись
//NRF_UICR->NFCPINS = 0; //отключает nfc и nfc пины становятся доступными для использования
#ifdef SERIAL_PRINT
    NRF_UART0->ENABLE = 1;  
#else
    NRF_UART0->ENABLE = 0;  
#endif // SERIAL_PRINT
NRF_NVMC->CONFIG = 0; //запретить запись
#endif // NRF52840

  happyInit();
}

void setup() {    
    happyConfig();
  
    #ifdef SHT_HUM
      sht20.initSHT20();      // Init SHT20 Sensor
    #endif // SHT_HUM
    
    blink(1);
    
    addDreamPin(PIN_BUTTON, NRF_GPIO_PIN_NOPULL, CDream::NRF_PIN_HIGH_TO_LOW);  // Добавление пробуждения от PIN_BUTTON
    #ifdef Move_Mode  
    addDreamPin(MOVE_INPUT_SENSOR, NRF_GPIO_PIN_NOPULL, CDream::NRF_PIN_LOW_TO_HIGH);  // Добавление пробуждения от MOVE_INPUT_SENSOR 
    #endif // Move_Mode
    #ifdef YF_201 
    addDreamPin(YF201_INPUT_SENSOR, NRF_GPIO_PIN_NOPULL, CDream::NRF_PIN_LOW_TO_HIGH);  // Добавление пробуждения от MOVE_INPUT_SENSOR 
    #endif // YF_201
    interruptedSleep.init();
     #ifdef SHT_HUM
      SHT_read();
     #endif // SHT_HUM
        
}

void happyPresentation() {
    happySendSketchInfo("HappyNode nRF52811 test", "V1.0");
    #ifdef Move_Mode 
      happyPresent(CHILD_ID_MOVE, S_MOTION, "Motion sensor");
    #endif // Move_Mode
    #ifdef YF_201 
      happyPresent(YF201_ID, S_WATER, "Water litrs");
    #endif // YF_201
    #ifdef SHT_HUM
      happyPresent(CHILD_ID_TEMP, S_TEMP, "Temperatura");
      happyPresent(CHILD_ID_HUM, S_HUM, "Humidity");
     #endif // SHT_HUM
    
}

void loop() {

    happyProcess();

    if (first_send) {    // Заглушка пока, отправка при старте батерейки и RSSI
      first_send = false;
      happyNode.sendBattery(MY_SEND_BATTERY);
      happyNode.sendSignalStrength(MY_SEND_RSSI);
    }
    
    int8_t wakeupReson = dream(sleepingPeriod);
    if (wakeupReson == MY_WAKE_UP_BY_TIMER){
     #ifdef SHT_HUM
      SHT_read();
     #endif // SHT_HUM
     counterBattery ++;
     if (counterBattery >= counterBatterySend) {
        happyNode.sendBattery(MY_SEND_BATTERY);
        happyNode.sendSignalStrength(MY_SEND_RSSI);
        counterBattery = 0;
     }
       
    }
  
  
   if (millis() - previousMillis > configMillis) {
    previousMillis = millis();
    #ifdef Move_Mode 
     countState = 0;
    #endif // Move_Mode
    #ifdef YF_201
     YF201_send();
    #endif // YF_201
   }
   #ifdef Move_Mode 
    if (MOVE_INPUT_SENSOR) {  // Обработка движения
      if (digitalRead(MOVE_INPUT_SENSOR) == HIGH) {
        move_state = true;
        countState++;
        
           }
   }
   #endif // Move_Mode

  #ifdef YF_201
    if (YF201_INPUT_SENSOR) {  // Обработка счетчика
       if (digitalRead(YF201_INPUT_SENSOR) == HIGH) {
      YF201count++;
       }
    }
  #endif // YF_201

    if (PIN_BUTTON) {  // Заглушка на обработку кнопки
       if (digitalRead(PIN_BUTTON) == LOW) {
        BUTTON_send();
       }
    }

if (move_state) {
  MOVE_send();
  move_state = false;
}
    


}


void receive(const MyMessage & message){
    if (happyCheckAck(message)) return;

}

#ifdef SHT_HUM
        
void SHT_read() {
    hum = sht20.readHumidity();                  // Read Humidity
    temp = sht20.readTemperature();               // Read Temperature

    if ((int)hum < 0) {
    hum = 0.0;
    }
    if ((int)hum > 99) {
    hum = 99.9;
    }

    if ((int)temp < 0) {
    temp = 0.0;
    }
    if ((int)temp > 99) {
    temp = 99.9;
    }

    SHT_send();

    if (counterBattery == 0) {
    happyNode.sendBattery(MY_SEND_BATTERY);
    happyNode.sendSignalStrength(MY_SEND_RSSI);
    blink(1);
    }
    counterBattery ++;
    if (counterBattery == counterBatterySend) {
        counterBattery = 0;
    }
}



void SHT_send() {
    if (abs(temp - old_temp) >= tempThreshold) {
    old_temp = temp;
    happySend(msgTemp.set(temp, 1));
    blink(1);
    }

    if (abs(hum - old_hum) >= humThreshold) {
    old_hum = hum;
    happySend(msgHum.set(hum, 1));
    blink(1);
    }

}
#endif // SHT_HUM

void blink (uint8_t flash) {
#ifdef Led_mode
  for (uint8_t i = 1; i <= flash; i++) {
    digitalWrite(BLUE_LED, LOW);
    wait(50);
    digitalWrite(BLUE_LED, HIGH);
    wait(50);
  }
#endif // Led_mode
}

void BUTTON_send()  {         // Send Button status with confirmations 

  happySend(sendButtonMsg.set(button_state, 1));
  button_state = !button_state;
  // if (counterBattery == 0) {
  //   happyNode.sendBattery(99);
  //   happyNode.sendSignalStrength(100);
  //   blink(1);
  //   }
  //   counterBattery ++;
  //   if (counterBattery == counterBatterySend) {
  //       counterBattery = 0;
  //   }
  blink(2);  // Succsess transmit
    }

#ifdef Move_Mode 
void MOVE_send()  {         // Send move count with confirmations 

  happySend(msgMove.set(countState, 1));
  blink(2);  // Succsess transmit
}
#endif // Move_Mode

#ifdef YF_201
void YF201_send()  {         // Send litrs by YF-201 with confirmations 
  uint8_t yf_litr = round(YF201count*2.25/1000);
  happySend(msgYF201.set(YF201count, 1));
  YF201count = 0;
  blink(2);  // Succsess transmit
}
#endif // YF_201
