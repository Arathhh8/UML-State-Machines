
enum even{
  ON,
  OFF
};

typedef enum{
  LIGHT_OFF,
  LIGHT_DIM,
  LIGHT_MEDIUM,
  LIGHT_FULL
}light_state_t;

light_state_t curr_state = LIGHT_OFF;

#define PIN_LED 9

#define LIGHT_BRIGHT_OFF    0
#define LIGHT_BRIGHT_DIM    25
#define LIGHT_BRIGHT_MED    85
#define LIGHT_BRIGHT_FULL   255


void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}


void light_state_machine(uint8_t event){
  switch(curr_state){
    case LIGHT_OFF:{
      switch(event){
        case ON:{
          light_change_intensity(PIN_LED, LIGHT_BRIGHT_DIM);
          current_state = LIGTH_DIM;
          break;
        }
      }
      break;
    }
    case LIGHT_DIM:{
      switch(event){
        case ON:{
          break;
        }
        case OFF:{
          break;
        }
      }
      break;
    }
    case LIGHT_MEDIUM:{
      switch(event){
        case ON:{
          break;
        }
        case OFF:{
          break;
        }
      }
      break;
    }
    case LIGHT_FULL:{
      switch(event){
        case ON:{
          break;
        }
        case OFF:{
          break;
        }
      }
      break;
    }
  }
}

void light_change_intensity(uint8_t pin, uint8_t intensity){
  analogWrite(pin, intensity);
}
