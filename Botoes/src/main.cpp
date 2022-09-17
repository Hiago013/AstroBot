#include <Arduino.h>
#include <ControlMotor.h>
#include <PID.h>
#include <ButtonMaster.h>


//----------------------------------------------------Definindos Conexoes Ponte H<->Motor----------------------------------------------------
#define pin_motor_left_1 8 // BIN 2
#define pin_motor_left_2 7 // BIN 1

#define pin_motor_right_1 4 // AIN 1
#define pin_motor_right_2 5 // AIN 2

#define pin_speed_motor_left 6 // PWMA
#define pin_speed_motor_right 9  // PWMB
#define STBY 2 // STBY

#define defaultPWM 100 // PWM default

//----------------------------------------------------Definindos Conexoes arduino <-> botoes----------------------------------------------------
const int button_play = A0;
const int button_start = A1;
const int button_forward = 10;
const int button_turn_right = A2;
const int button_turn_left = 11;
//----------------------------------------------------Buzzer----------------------------------------------------
/*
  Frequência das notas:
  Dó - 262 Hz
  Ré - 294 Hz
  Mi - 330 Hz
  Fá - 349 Hz
  Sol - 392 Hz
  Lá - 440 Hz
  Si - 494 Hz
  #Dó - 528 Hz
  */
#define buzzer 13
#define DO 262
#define RE 294
#define MI 330
#define FA 249
#define SOL 392
#define LA 440
#define SI 494
#define _DO 528



//----------------------------------------------------Definindo Conexoes com dos botoes----------------------------------------------------

Buttons set_buttons = {button_play, button_start, button_forward, button_turn_right, button_turn_left};
ButtonMaster buttons(set_buttons);

//----------------------------------------------------Definindo Conexoes com a Ponte H----------------------------------------------------


PonteH set_linkage = {pin_motor_left_1, pin_motor_left_2, pin_motor_right_1, pin_motor_right_2,
                  STBY, pin_speed_motor_left, pin_speed_motor_right}; // Ponte H pinagens

ControlMotor linkage(set_linkage); // Ponte H motor

//---------------------------------------------------Instanciando objetos ----------------------------------------------------



// ----------------------------------------------------Variaveis Globais----------------------------------------------------

int sequence[10] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
int time = 500; // 500 milisegundos
// ----------------------------------------------------Protótipo de funções----------------------------------------------------

void wait(unsigned long time);
void run();


void setup() {
  linkage.Off();
  linkage.setPWM(defaultPWM, defaultPWM);
  pinMode(buzzer, OUTPUT);
}

void loop() {

  if(buttons.playPressed()){
    tone(buzzer, LA, time);
    wait(time);
    for(int i = 0; i <= 9; i++){
      sequence[i] = -1;
    }
    int count = 0;
    while((!buttons.startPressed()) && (count < 9)){
      if(buttons.forwardPressed()) {sequence[count] = 0; count++; wait(time); tone(buzzer, DO, time);}
      else if(buttons.turnRightPressed()){sequence[count] = 1; count++; wait(time);tone(buzzer, RE, time);}
      else if(buttons.turnLeftPressed()){sequence[count] = 2; count++; wait(time);tone(buzzer, MI, time);}
      
    }
    tone(buzzer, _DO, time);
    run();
  }


}


void wait(unsigned long time){

  unsigned long time_now = millis();
  while(time_now + time > millis()){

  }
}

void run(){
  for(auto task : sequence){
    switch (task)
    {
    case 0:
      linkage.goForward();
      wait(1000);
      linkage.Off();
      wait(100);
      break;
    case 1:
      linkage.TurnRight();
      wait(1000);
      linkage.Off();
      wait(100);
      break;
    case 2:
      linkage.TurnLeft();
      wait(1000);
      linkage.Off();
      wait(100);
      break;    
    default:
      linkage.Off();
      break;
    }
  }
}
