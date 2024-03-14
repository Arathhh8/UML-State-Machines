#include "main.h"


static void protimer_event_dispatcher(protimer_t *const mobj, event_t const *const e);


static protimer_t protimer;



void setup() {
  // put your setup code here, to run once:
  protimer_init(&protimer);
}

void loop() {
  // 1. read the button pad status
  // 2. make an event
  // 3. send it to event dispatcher
  //protimer_event_dispatcher();

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