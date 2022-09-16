#include <Arduino.h>
#include <ControlMotor.h>
#include <PID.h>
#include <MPU6050_light.h>
#include <ButtonMaster.h>

//----------------------------------------------------Definindos Conexoes Ponte H<->Motor----------------------------------------------------
#define pin_motor_left_1 5 // AIN 1
#define pin_motor_left_2 4 // AIN 2

#define pin_motor_right_1 7 // BIN 1
#define pin_motor_right_2 8 // BIN 2

#define pin_speed_motor_left 6 // PWMA
#define pin_speed_motor_right 9  // PWMB
#define STBY 2 // STBY

#define defaultPWM 100 // PWM default

//----------------------------------------------------Definindos Conexoes arduino <-> botoes----------------------------------------------------

const int button_play = A0;
const int button_funcao = A1;
const int button_forward = A2;
const int button_turn_right = 10;
const int button_turn_left = 11;

//----------------------------------------------------Definindo Conexoes com dos botoes----------------------------------------------------

Buttons set_buttons = {button_play, button_funcao, button_forward, button_turn_right, button_turn_left};
ButtonMaster buttons(set_buttons);

//----------------------------------------------------Definindo Conexoes com a Ponte H----------------------------------------------------


PonteH set_linkage = {pin_motor_left_1, pin_motor_left_2, pin_motor_right_1, pin_motor_right_2,
                  STBY, pin_speed_motor_left, pin_speed_motor_right}; // Ponte H pinagens

ControlMotor linkage(set_linkage); // Ponte H motor

//---------------------------------------------------Instanciando objetos ----------------------------------------------------
PID pid(3, 2, 2); // PID(Kp, Ki, Kd)

MPU6050 mpu(Wire); // MPU6050 sensor

// ----------------------------------------------------Variaveis Globais----------------------------------------------------
unsigned long timer = 0; // Variavel para armazenar o tempo
float angles[3]; // Variavel para armazenar os angulos
float auxForward, auxRight, auxLeft; // Variavel para armazenar o angulo

// ----------------------------------------------------Protótipo de funções----------------------------------------------------
void get_angle(); // Função para leitura do yaw
float pid2pwm(float pid, int max); // Função para transformar a saída do controlador de graus para pwm
void wait(unsigned long time);
void run_sequence(ControlMotor &motor, ButtonMaster &buttons, int sequence[10]);

void setup() {
  Wire.begin();
  byte status = mpu.begin(1, 0);
  while(status != 0){ }
  mpu.calcOffsets(true, true);
  mpu.setFilterGyroCoef(0.98);
  linkage.Off();
  linkage.setPWM(defaultPWM, defaultPWM);
  delay(3000); // Delay para por o robo em posição
  get_angle();
  auxForward = angles[2];
}

void loop() {
  // put your main code here, to run repeatedly:
  get_angle(); // leitura do angulo yaw, pitch e roll
  if((abs(angles[0]) > 30) || (abs(angles[1]) > 30)){
    linkage.Off();
    linkage.setPWM(0, 0);
    while ((abs(angles[0]) > 30) || (abs(angles[1]) > 30)) {get_angle(); delay(20);}
    get_angle();                                      // Leitura do angulo yaw, pitch e roll
    auxForward = angles[2];                           // Armazenando o angulo forward
  }

  else{
    if(buttons.playPressed()){
      wait(1000);
      int count = 0;
      int sequence[10] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
      while((!buttons.playPressed()) && count < 9){
        if(buttons.forwardPressed()){sequence[count] = 0; count++;}
        else if(buttons.turnRightPressed()){sequence[count] = 1; count++;}
        else if(buttons.turnLeftPressed()){sequence[count] = 2; count++;}
        wait(50);
      }
    buttons.setSequence(sequence);
    run_sequence(linkage, buttons, sequence);
    linkage.Off();
    }    
  }
}

void get_angle(){ // Função para leitura do roll, pitch e yaw
  mpu.update();
  if((millis() - timer) > 10){
    angles[0] = mpu.getAngleX();
    angles[1] = mpu.getAngleY();
    angles[2] = mpu.getAngleZ();
    timer = millis();
  }
}

float pid2pwm(float pid, int max){ // Função para transformar a saída do controlador de graus para pwm
  if(pid < 0){
    pid = map(pid, 0, -360, 0, 200);
    if(pid >= max) pid = max;
  }
  else
  {
    pid = map(pid, 0, 360, 0, 200);
    if(pid >= max) pid = max;
  }

  return pid;
}

void wait(unsigned long time){

  unsigned long time_now = millis();
  while(time_now + time > millis()){

  }
}

void run_sequence(ControlMotor &motor, ButtonMaster &buttons, int sequence[10]){

    for(int i = 0; i < 10; i++){
        int value = sequence[i];
        switch (value)
        {
        case 0:
        {
            get_angle();
            auxForward = angles[2];
            unsigned long time = 3000;
            unsigned long time_now = millis();
            while(time_now + time > millis()){
              float out = pid.calculate(angles[2], auxForward); // Calcula o PID
              if(out < 0) {out = pid2pwm(out, 10); motor.setPWM(defaultPWM + out, defaultPWM - out);} // Se o PID for negativo, o motor gira mais para a esquerda
              else {out = pid2pwm(out, 10); motor.setPWM(defaultPWM - out, defaultPWM + out);} // Se o PID for positivo, o motor gira mais para a direita
              motor.goForward();
              get_angle();
              wait(10);
            }
            motor.Off();
            break;
        }
        case 1:
        {
            get_angle();
            auxRight = angles[2];
            while (auxRight - 90 - angles[2] < 0){
              motor.TurnRight();
              get_angle();
              wait(10);
            }
            motor.Off();
            break;
        }
        case 2:
        {
            get_angle();
            auxLeft = angles[2];
            while (auxLeft + 90 - angles[2] > 0){
              motor.TurnLeft();
              get_angle();
              wait(10);
            }
            motor.Off();
            break;
        }
        default:
            motor.Off();
            break;
        }
    }
}
