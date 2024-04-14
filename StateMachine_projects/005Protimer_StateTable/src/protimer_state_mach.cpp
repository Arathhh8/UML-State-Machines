#include "main.h"
#include"lcd.h"

/* PROTOTYPES OF HELPER FUNCTIONS */
static void display_time(uint32_t time);
static void display_message(String s, uint8_t col, uint8_t row);
static void display_clear(void);
static void do_beep(void);


void protimer_init(protimer_t *mobj){
    event_t ee; // entry action
    ee.sig = ENTRY;
    mobj->active_state = IDLE;
    mobj->pro_time = 0;
    protimer_state_machine(mobj, &ee);

}

/**********************************************************************************************************************************************
 ************************************************************** EVENT HANDLERS ****************************************************************
 **********************************************************************************************************************************************/

/****************************  IDLE EVENT HANDLER  ****************************/

event_status_t IDLE_Entry(protimer_t *const mobj, event_t const *const e){
    mobj->curr_time = 0;
    mobj->elapsed_time = 0;
    display_time(0);
    display_message("Set ", 0, 0);
    display_message("time ", 1, 0);
    return EVENT_HANDLED;
}

event_status_t IDLE_Exit(protimer_t *const mobj, event_t const *const e){
    display_clear();
    return EVENT_HANDLED;
}

event_status_t IDLE_Inc_time(protimer_t *const mobj, event_t const *const e){
    mobj->curr_time += 60;
    mobj->active_state = TIME_SET;
    return EVENT_TRANSITION;
}

event_status_t IDLE_Start_pause(protimer_t *const mobj, event_t const *const e){
    mobj->active_state = STAT;
    return EVENT_TRANSITION;
}

event_status_t IDLE_Time_tick(protimer_t *const mobj, event_t const *const e){
    if( ((protimer_tick_event_t *)(e))->ss == 5 ){
        do_beep();
        return EVENT_HANDLED;
    }
    return EVENT_IGNORED;
}

/****************************  TIME_SET EVENT HANDLER  ****************************/

event_status_t TIME_SET_Entry(protimer_t *const mobj, event_t const *const e){
    display_time(mobj->curr_time);
    return EVENT_HANDLED;
}

event_status_t TIME_SET_Exit(protimer_t *const mobj, event_t const *const e){
    display_clear();
    return EVENT_IGNORED;
}

event_status_t TIME_SET_Inc_time(protimer_t *const mobj, event_t const *const e){
    mobj->curr_time += 60;
    display_time(mobj->curr_time);
    return EVENT_HANDLED;  
}

event_status_t TIME_SET_Dec_time(protimer_t *const mobj, event_t const *const e){
    if(mobj->curr_time >= 60){
        mobj->curr_time -= 60;
        display_time(mobj->curr_time);
        return EVENT_HANDLED;
    }
}

event_status_t TIME_SET_Start_pause(protimer_t *const mobj, event_t const *const e){
    if(mobj->curr_time >= 60){
        mobj->active_state = COUNTDOWN;
        return EVENT_TRANSITION;
    }
    return EVENT_IGNORED;
}

event_status_t TIME_SET_Abrt(protimer_t *const mobj, event_t const *const e){
    mobj->active_state = IDLE;
    return EVENT_TRANSITION;
}

/****************************  COUNTDOWN EVENT HANDLER  ****************************/

event_status_t COUNTDOWN_Exit(protimer_t *const mobj, event_t const *const e){
    mobj->pro_time += mobj->elapsed_time;
    mobj->elapsed_time = 0;
    return EVENT_HANDLED;
}

event_status_t COUNTDOWN_Time_tick(protimer_t *const mobj, event_t const *const e){
    if( ((protimer_tick_event_t *)(e))->ss == 10){
        --mobj->curr_time;
        ++mobj->elapsed_time;
        display_time(mobj->curr_time);
        if(!mobj->curr_time){
            mobj->active_state = IDLE;
            return EVENT_TRANSITION;
        }
        return EVENT_HANDLED;
    }
    return EVENT_IGNORED;
}

event_status_t COUNTDOWN_Start_pause(protimer_t *const mobj, event_t const *const e){
    mobj->active_state = PAUSE;
    return EVENT_TRANSITION;   
}

event_status_t COUNTDOWN_Abrt(protimer_t *const mobj, event_t const *const e){
    mobj->active_state = IDLE;
    return EVENT_TRANSITION;
}

/****************************  PAUSE EVENT HANDLER  ****************************/

event_status_t PUASE_ENTRY(protimer_t *const mobj, event_t const *const e){
    display_message("Paused", 5, 1);
    return EVENT_HANDLED;
}

event_status_t PUASE_Exit(protimer_t *const mobj, event_t const *const e){
    display_clear();
    return EVENT_HANDLED;
}

event_status_t PUASE_Inc_time(protimer_t *const mobj, event_t const *const e){
    mobj->curr_time += 60;
    mobj->active_state = TIME_SET;
    return EVENT_TRANSITION;   
}

event_status_t PUASE_Dec_time(protimer_t *const mobj, event_t const *const e){
    if(mobj->curr_time >= 60){
        mobj->curr_time -= 60;
        mobj->active_state = TIME_SET;
        return EVENT_TRANSITION;
    }
    return EVENT_IGNORED;
}


/****************************  STAT EVENT HANDLER  ****************************/

event_status_t STAT_Entry(protimer_t *const mobj, event_t const *const e){
    display_time(mobj->pro_time);
    display_message("Productive time", 1, 1);
    return EVENT_HANDLED;
}

event_status_t STAT_Exit(protimer_t *const mobj, event_t const *const e){
    display_clear();
    return EVENT_HANDLED;
}

event_status_t STAT_Time_tick(protimer_t *const mobj, event_t const *const e){
    static uint8_t tick_count;
    if(++tick_count == 30){
        tick_count = 0;
        mobj->active_state = IDLE;
        return EVENT_TRANSITION;
    }
    return EVENT_IGNORED;
}

/**********************************************************************************************************************************************
 ************************************************************** HELPER FUNCTIONS **************************************************************
 **********************************************************************************************************************************************/

static void display_time(uint32_t time){
    char buf[7];
    String time_msg;

    uint16_t m = time / 60;
    uint8_t s = time % 60;
    sprintf(buf, "%03d:%02d", m,s);

    time_msg = (String)buf;
    lcd_set_cursor(0, 5);
    lcd_print_string(time_msg);
}
static void display_message(String s, uint8_t col, uint8_t row){
    lcd_set_cursor(col, row);
    lcd_print_string(s);
}

static void display_clear(void){
    lcd_clear();
}

static void do_beep(void){
    tone(PIN_BUZZER, 6000, 25);
}