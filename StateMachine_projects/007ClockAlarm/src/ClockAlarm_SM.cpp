/*.$file${HSMs::../src::ClockAlarm_SM.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*
* Model: ClockAlarm.qm
* File:  ${HSMs::../src::ClockAlarm_SM.cpp}
*
* This code has been generated by QM 5.1.1 <www.state-machine.com/qm/>.
* DO NOT EDIT SECTIONS BETWEEN THE COMMENTS "$...vvv".."$end...^^^".
* All your changes in these sections will be lost.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*/
/*.$endhead${HSMs::../src::ClockAlarm_SM.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
#include<Arduino.h>
#include "qpn.h"
#include "lcd.h"
#include "ClockAlarm_SM.h"

/*.$declare${HSMs::Clock_Alarm} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*.${HSMs::Clock_Alarm} ....................................................*/
typedef struct Clock_Alarm {
/* protected: */
    QHsm super;

/* private: */
    uint32_t temp_time;
    uint32_t alarm_time;
    uint8_t alarm_status;
    uint32_t time_mode;

/* private state histories */
    QStateHandler hist_Clock;
} Clock_Alarm;

/* public: */
static uint32_t Clock_Alarm_get_curr_time(void);
static void Clock_Alarm_update_curr_time(void);
static void Clock_Alarm_set_curr_time(uint32_t new_curr_time);
static void Clock_Alarm_display_curr_time(Clock_Alarm * const me, uint8_t row, uint8_t col);
extern uint32_t Clock_Alarm_curr_time;
extern Clock_Alarm Clock_Alarm_obj;

/* protected: */
static QState Clock_Alarm_initial(Clock_Alarm * const me);
static QState Clock_Alarm_Clock(Clock_Alarm * const me);
static QState Clock_Alarm_Ticking(Clock_Alarm * const me);
static QState Clock_Alarm_Settings(Clock_Alarm * const me);
static QState Clock_Alarm_Clock_Setting(Clock_Alarm * const me);
static QState Clock_Alarm_Alarm_Setting(Clock_Alarm * const me);
static QState Clock_Alarm_Alarm_Notify(Clock_Alarm * const me);
/*.$enddecl${HSMs::Clock_Alarm} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

// Prototypes of helper functions
String get_am_or_pm(uint32_t time24h);
void  display_write(String str_, uint8_t r, uint8_t c);
String integertime_to_string(uint32_t time_);
uint32_t convert_12hformat_to_24h(uint32_t time12h, time_format_t ampm);
uint32_t convert_24hformat_to_12h(uint32_t time24h);
void display_clock_setting_time(Clock_Alarm *me,uint8_t row, uint8_t col);

/*.$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*. Check for the minimum required QP version */
#if (QP_VERSION < 690U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpn version 6.9.0 or higher required
#endif
/*.$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*.$define${HSMs::Clock_Alarm_ctor} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*.${HSMs::Clock_Alarm_ctor} ...............................................*/
void Clock_Alarm_ctor(void) {
    QHsm_ctor(&Clock_Alarm_obj.super, Q_STATE_CAST(&Clock_Alarm_initial));
}
/*.$enddef${HSMs::Clock_Alarm_ctor} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*.$define${HSMs::Clock_Alarm} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*.${HSMs::Clock_Alarm} ....................................................*/
uint32_t Clock_Alarm_curr_time;
Clock_Alarm Clock_Alarm_obj;
/*.${HSMs::Clock_Alarm::get_curr_time} .....................................*/
static uint32_t Clock_Alarm_get_curr_time(void) {
    uint8_t saved_sreg = SREG;
    uint32_t temp;
    cli();
    temp = Clock_Alarm_curr_time;
    SREG = saved_sreg;
    return temp;
}

/*.${HSMs::Clock_Alarm::update_curr_time} ..................................*/
static void Clock_Alarm_update_curr_time(void) {
    if(++Clock_Alarm_curr_time == MAX_TIME){
        Clock_Alarm_curr_time = 0;
    }
}

/*.${HSMs::Clock_Alarm::set_curr_time} .....................................*/
static void Clock_Alarm_set_curr_time(uint32_t new_curr_time) {
    uint8_t save_reg = SREG;
    cli();
    Clock_Alarm_curr_time = new_curr_time;
    SREG = save_reg;
}

/*.${HSMs::Clock_Alarm::display_curr_time} .................................*/
static void Clock_Alarm_display_curr_time(Clock_Alarm * const me, uint8_t row, uint8_t col) {
    String time_as_string;
    uint32_t time_;

    uint32_t time24h = Clock_Alarm_get_curr_time()/10; //convert to number of seconds
    uint8_t ss = time24h % 10U;       //extract sub-second to append later

    time_ = (me->time_mode == MODE_24H)?time24h:convert_24hformat_to_12h(time24h);
    time_as_string = integertime_to_string(time_); //hh:mm:ss
    time_as_string.concat('.');
    time_as_string.concat(ss);

    /*if mode is 12H , concatenate  am/pm information */
    if(me->time_mode == MODE_12H){
        time_as_string.concat(' ');
        time_as_string.concat(get_am_or_pm(time24h));
    }

    display_write(time_as_string,row,col);

}

/*.${HSMs::Clock_Alarm::SM} ................................................*/
static QState Clock_Alarm_initial(Clock_Alarm * const me) {
    /*.${HSMs::Clock_Alarm::SM::initial} */
    Clock_Alarm_set_curr_time(INITIAL_CURR_TIME);
    me->alarm_time = INITIAL_ALARM_TIME;
    me->time_mode = MODE_12;
    me->alarm_status = ALARM_OFF;
    /* state history attributes */
    /* state history attributes */
    me->hist_Clock = Q_STATE_CAST(&Clock_Alarm_Ticking);
    return Q_TRAN(&Clock_Alarm_Ticking);
}
/*.${HSMs::Clock_Alarm::SM::Clock} .........................................*/
static QState Clock_Alarm_Clock(Clock_Alarm * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*.${HSMs::Clock_Alarm::SM::Clock} */
        case Q_EXIT_SIG: {
            /* save deep history */
            me->hist_Clock = QHsm_state(me);
            status_ = Q_HANDLED();
            break;
        }
        /*.${HSMs::Clock_Alarm::SM::Clock::ALARM} */
        case ALARM_SIG: {
            status_ = Q_TRAN(&Clock_Alarm_Alarm_Notify);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/*.${HSMs::Clock_Alarm::SM::Clock::Ticking} ................................*/
static QState Clock_Alarm_Ticking(Clock_Alarm * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*.${HSMs::Clock_Alarm::SM::Clock::Ticking} */
        case Q_ENTRY_SIG: {
            Clock_Alarm_display_curr_time(me,TICKING_CURR_TIME_ROW, TICKING_CURR_TIME_COL)
            status_ = Q_HANDLED();
            break;
        }
        /*.${HSMs::Clock_Alarm::SM::Clock::Ticking::SET} */
        case SET_SIG: {
            status_ = Q_TRAN(&Clock_Alarm_Clock_Setting);
            break;
        }
        /*.${HSMs::Clock_Alarm::SM::Clock::Ticking::OK} */
        case OK_SIG: {
            status_ = Q_TRAN(&Clock_Alarm_Alarm_Setting);
            break;
        }
        /*.${HSMs::Clock_Alarm::SM::Clock::Ticking::TICK} */
        case TICK_SIG: {
             Clock_Alarm_display_curr_time(me,TICKING_CURR_TIME_ROW,TICKING_CURR_TIME_COL);
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&Clock_Alarm_Clock);
            break;
        }
    }
    return status_;
}
/*.${HSMs::Clock_Alarm::SM::Clock::Settings} ...............................*/
static QState Clock_Alarm_Settings(Clock_Alarm * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*.${HSMs::Clock_Alarm::SM::Clock::Settings::ABRT} */
        case ABRT_SIG: {
            status_ = Q_TRAN(&Clock_Alarm_Ticking);
            break;
        }
        /*.${HSMs::Clock_Alarm::SM::Clock::Settings::OK} */
        case OK_SIG: {
            status_ = Q_TRAN(&Clock_Alarm_Ticking);
            break;
        }
        default: {
            status_ = Q_SUPER(&Clock_Alarm_Clock);
            break;
        }
    }
    return status_;
}
/*.${HSMs::Clock_Alarm::SM::Clock::Settings::Clock_Setting} ................*/
static QState Clock_Alarm_Clock_Setting(Clock_Alarm * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        default: {
            status_ = Q_SUPER(&Clock_Alarm_Settings);
            break;
        }
    }
    return status_;
}
/*.${HSMs::Clock_Alarm::SM::Clock::Settings::Alarm_Setting} ................*/
static QState Clock_Alarm_Alarm_Setting(Clock_Alarm * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        default: {
            status_ = Q_SUPER(&Clock_Alarm_Settings);
            break;
        }
    }
    return status_;
}
/*.${HSMs::Clock_Alarm::SM::Alarm_Notify} ..................................*/
static QState Clock_Alarm_Alarm_Notify(Clock_Alarm * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        /*.${HSMs::Clock_Alarm::SM::Alarm_Notify::OK} */
        case OK_SIG: {
            status_ = Q_TRAN_HIST(me->hist_Clock);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/*.$enddef${HSMs::Clock_Alarm} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/


ISR(TIMER1_COMPA_vect){
    Clock_Alarm_update_curr_time();
}

/*
 * Description : Decodes AM/PM information from given time in 24H format
 * param1: Integer time in 24H format
 * return : A string value("AM" or "PM")
 */
String get_am_or_pm(uint32_t time24h){
    String ampm;
    uint8_t h = GET_HOUR(time24h);
    if(h == 0U){
        ampm = "AM";
    }else if( h > 12U){
        ampm = "PM";
    }else if (h == 12U)
        ampm = "PM";
    else
        ampm = "AM";
    return ampm;
}

/*
 * Description: Writes a message to the LCD at given row and column number
 * param1 : Message to write in 'String' format
 * param2 : row number of the LCD
 * param2 : column number of the LCD
 */
void  display_write(String str_, uint8_t r, uint8_t c){
    lcd_set_cursor(c,r);
    lcd_print_string(str_);
}

/*
 * Description: converts an 'integer' time to 'String' time
 * param1 : time represented in terms of number of seconds
 * return : time as 'String' value in the format HH:MM:SS
 */
String integertime_to_string(uint32_t time_){
    uint8_t h,m,s;
    char buf[10]; //00:00:00+null
    h = GET_HOUR(time_); /* Extract how many hours the 'time_' represent */
    m = GET_MIN(time_);  /* Extract how many minutes the 'time_' represent */
    s = GET_SEC(time_);	 /* Extract how many seconds the 'time_' represent */
    sprintf(buf,"%02d:%02d:%02d",h,m,s);
    return (String)buf;
}

/*
 * Description: Converts given integer time in 12H format to integer time 24H format
 * param1 : Integer time in 12H format
 * param2 : time format of type time_format_t
 * return : Integer time in 24H format
 */
uint32_t convert_12hformat_to_24h(uint32_t time12h, time_format_t ampm){
    uint8_t hour;
    uint32_t time24h;
    hour = GET_HOUR(time12h);
    if(ampm == FORMAT_AM){
        time24h = (hour == 12)? (time12h-(12UL * 3600UL)) : time12h;
    }else{
        time24h = (hour == 12)? time12h : (time12h +(12UL * 3600UL));
    }
    return time24h;
}

/*
 * Description: Converts given integer time in 24H format to integer time 12H format
 * param1 : Integer time in 24H format
 * return : Integer time in 12H format
 */
uint32_t convert_24hformat_to_12h(uint32_t time24h){
    uint8_t hour;
    uint32_t time12h;
    hour = GET_HOUR(time24h);

    if(hour == 0)
        time12h = time24h + (12UL * 3600UL);
    else{
        if((hour < 12UL) || (hour == 12UL))
            return time24h;
        else
            time12h = time24h - (12UL * 3600UL);
    }
    return time12h;
}


/*
 * Description : Displays current time depending upon the time mode
 * param1: 'me' pointer
 * param2 : row number of the LCD
 * param3: column number of the LCD
 */
void display_clock_setting_time(Clock_Alarm *me,uint8_t row, uint8_t col){
	String time_as_string;

	time_as_string = integertime_to_string(me->temp_time); //hh:mm:ss

	/*concatenate  am/pm information */
	if(me->temp_format != FORMAT_24H){
		time_as_string.concat(' ');
		if(me->temp_format == FORMAT_AM)
			time_as_string.concat("AM");
		else
			time_as_string.concat("PM");
	}

	display_write(time_as_string,row,col);
}



 /*.${HSMs::Clock_Alarm::SM::Clock::Ticking::SET} */
me->temp_time = Clock_Alarm_get_curr_time()/10;
if(me->time_mode == MODE_12H){
	if(get_am_or_pm(me->temp_time).equals("AM")){
		me->temp_format = FORMAT_AM;
	}
	else{
		me->temp_format = FORMAT_PM;
	}
	me->temp_time = convert_24hformat_to_12h(me->temp_time);
}
else
	me->temp_format = FORMAT_24H;


/* ${HSMs::Clock_Alarm::SM::Clock::Settings::Clock_Setting} */
Clock_Alarm_display_clock_setting_time(me,0,2);
display_cursor_on_blinkon();

/* ${HSMs::Clock_Alarm::SM::Clock::Settings::Clock_Setting::cs_hour_d1} */
display_set_cursor(CS_ROW,CS_HOUR_D1_COL);
me->temp_digit = DIGIT1(GET_HOUR(me->temp_time));

/*${HSMs::Clock_Alarm::SM::Clock::Settings::Clock_Setting::cs_hour_d1::SET} */
++me->temp_digit;
me->temp_digit %= 3;
me->temp_time -=  DIGIT1(GET_HOUR(me->temp_time)) * 10UL * 3600UL;
me->temp_time += (me->temp_digit * 10UL ) * 3600UL;
Clock_Alarm_display_clock_setting_time(me,0,2);
display_set_cursor(CS_ROW,CS_HOUR_D1_COL);


void display_cursor_on_blinkon(){
    lcd_cursor_show();
    lcd_cursor_blink();
}

void display_cursor_off_blinkoff(){
    lcd_cursor_off();
    lcd_cursor_blinkoff();
}


void display_set_cursor(uint8_t r, uint8_t c){
     lcd_set_cursor(c,r);
}

bool is_time_set_error(uint32_t time_,time_format_t format){
    uint8_t h = GET_HOUR(time_);
    return ((h > 23) || ((h > 12 || (h == 0))&& format != FORMAT_24H) );
}

${HSMs::Clock_Alarm::SM::Clock::Settings::Clock_Setting::cs_format}
display_set_cursor(CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_FMT_COL);
String msg[3] = {"24H","AM ","PM "};
display_write(msg[me->temp_format],CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_FMT_COL);
display_set_cursor(CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_FMT_COL);

${HSMs::Clock_Alarm::SM::Clock::Settings::Clock_Setting::cs_format::SET}
String msg;
if(me->temp_format == FORMAT_24H){
    me->temp_format = FORMAT_AM;
    msg = "AM ";
}
else if(me->temp_format == FORMAT_AM){
    me->temp_format = FORMAT_PM;
    msg = "PM ";
}
else if (me->temp_format == FORMAT_PM){
    me->temp_format = FORMAT_24H;
    msg = "24H";
}
display_write(msg,CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_FMT_COL);
display_set_cursor(CLOCK_SETTING_TIME_ROW,CLOCK_SETTING_TIME_FMT_COL);


void display_erase_block(uint8_t row,uint8_t col_start,uint8_t col_stop)
{
    uint8_t len = col_stop - col_start;
    do{
        lcd_set_cursor(col_start++,row);
        lcd_print_char(' ');
    }while(len--);
}

/* ${HSMs::Clock_Alarm::SM::Clock::Settings::OK} */
	uint8_t save_sreg;

	if(me->temp_format != FORMAT_24H){
		me->temp_time = convert_12hformat_to_24h( me->temp_time,(time_format_t)me->temp_format);
		me->time_mode = MODE_12H;
	}else{
		me->time_mode = MODE_24H;
	}

	me->temp_time *= 10UL;
	save_sreg = SREG;
	cli();
	TCCR1B &= ~(0x7U); //Stop the TIMER1
	TCNT1 = 0U;
	Clock_Alarm_curr_time = me->temp_time;
	TCCR1B |= 0x4U;
	SREG = save_sreg;