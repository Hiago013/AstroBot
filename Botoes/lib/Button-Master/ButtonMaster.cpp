#include <Arduino.h>
#include "ButtonMaster.h"

ButtonMaster::ButtonMaster(Buttons _buttons){
    button_play = _buttons.button_play;
    button_function = _buttons.button_function;
    button_forward = _buttons.button_forward;
    button_turn_right = _buttons.button_turn_right;
    button_turn_left = _buttons.button_turn_left;

    pinMode(button_play, INPUT_PULLUP);
    pinMode(button_function, INPUT_PULLUP);
    pinMode(button_forward, INPUT_PULLUP);
    pinMode(button_turn_right, INPUT_PULLUP);
    pinMode(button_turn_left, INPUT_PULLUP);
}

void ButtonMaster::setSequence(int new_sequence[10]){
for(int i = 0; i < 9; i++){
    sequence[i] = new_sequence[i];
}
}

bool ButtonMaster::forwardPressed(){
return !digitalRead(button_forward);
}

bool ButtonMaster::turnRightPressed(){
return !digitalRead(button_turn_right);
}

bool ButtonMaster::turnLeftPressed(){
return !digitalRead(button_turn_left);
}

bool ButtonMaster::playPressed(){
return !digitalRead(button_play);
}

void ButtonMaster::run(ControlMotor &motor){
    for(auto value: sequence){
        switch (value)
        {
        case 0:
            motor.goForward();
            break;
        case 1:
            motor.TurnRight();
            break;
        case 2:
            motor.TurnLeft();
            break;
        default:
            motor.Off();
            break;
        }
    }
}