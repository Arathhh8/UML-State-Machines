#include "main.h"


static void protimer_event_dispatcher(protimer_t *const mobj, event_t const *const e);
static uint8_t process_button_pad_value(uint8_t btn_pad_value);


static protimer_t protimer;



void setup() {
  // put your setup code here, to run once:
  protimer_init(&protimer);
}

void loop() {

  uint8_t b1, b2, b3, btn_pad_value;
  protimer_user_event_t ue; // ue user event
  static uint32_t current_time = millis();
  static protimer_tick_event_t te = {0};

  // 1. read the button pad status
  b1 = digitalRead(PIN_BUTTON1);
  b2 = digitalRead(PIN_BUTTON2);
  b3 = digitalRead(PIN_BUTTON3);

  btn_pad_value = (b1 << 2) | (b2 << 1) | b3;

  // Software button de-bouncing
  btn_pad_value = process_button_pad_value(btn_pad_value);

  // 2. make an event
  if(btn_pad_value ){
    if(btn_pad_value == BTN_PAD_VALUE_INC_TIME){    
      ue.super.sig = INC_TIME;
    }else if(btn_pad_value == BTN_PAD_VALUE_DEC_TIME){
      ue.super.sig = DEC_TIME;
    }else if(btn_pad_value == BTN_PAD_VALUE_SP){
      ue.super.sig = START_PAUSE;
    }else if(btn_pad_value == BTN_PAD_VALUE_ABRT){
      ue.super.sig = ABRT;
    }

    // 3. send it to event dispatcher
    protimer_event_dispatcher(&protimer, &ue.super);
  }

  // 4. dispatch the time tick event for every 100 ms
  if(millis() - current_time >= 100){
    // 100 ms has passed 
    current_time = millis();
    te.super.sig = TIME_TICK;
    if(++te.ss > 10) te.ss = 1;
     protimer_event_dispatcher(&protimer, &te.super);
  }

}


void protimer_event_dispatcher(protimer_t *const mobj, event_t const *const e){

  event_status_t status;
  protimer_state_t source, target;

  source = mobj->active_state;

  status = protimer_state_machine(mobj, e);
  

  if(status == EVENT_TRANSITION){
    target = mobj->active_state;
    event_t ee;
    // 1. Run the exit action for the source state
    ee.sig = EXIT;
    mobj->active_state = source;
    protimer_state_machine(mobj, &ee);

    // 2. Run the entry action for the target state
    ee.sig = ENTRY;
    mobj->active_state = target;
    protimer_state_machine(mobj, &ee);
  }

}

static uint8_t process_button_pad_value(uint8_t btn_pad_value){

  return 0;
}