#include <Arduino.h>


void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}


static void Timer1_setup(void){
  TCCR1A = 0;                   // CTC mode
  TCCR1B = B00001100;           // Prescaler = 256, CTC mode
  TIMSK1 |= B00000010;          // Interrupt enable for OCR1A compare match
  OCR1A = 6250 - 1;            // OC match value for 100ms time basee generation
}