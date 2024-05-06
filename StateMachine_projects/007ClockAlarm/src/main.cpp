#include <Arduino.h>
#include "qpn.h"
#include "ClockAlarm_SM.h"
#include "lcd.h"


Q_DEFINE_THIS_FILE;

static void Timer1_setup(void);

void setup() {
  // put your setup code here, to run once:
  Timer1_setup();
  Clock_Alarm_ctor();
}

void loop() {
  // put your main code here, to run repeatedly:
  uint32_t tick_time = millis();

  while(millis() - tick_time >= 50){
    // Send TICK event
    tick_time = millis();
    Q_SIG(super_ClockAlarm) = TICK_SIG; // to review
    QHSM_DISPATCH(super_ClockAlarm);
  }
}


static void Timer1_setup(void){
  TCCR1A = 0;                   // CTC mode
  TCCR1B |= B00001100;           // Prescaler = 256, CTC mode
  TIMSK1 |= B00000010;          // Interrupt enable for OCR1A compare match
  OCR1A = 6250 - 1;            // OC match value for 100ms time basee generation
}