#ifndef ButtonMaster_h
#define ButtonMaster_h

#include "ControlMotor.h"

struct Buttons
{
int button_play; 
int button_function; 

int button_forward; 
int button_turn_right;
int button_turn_left; 

};

class ButtonMaster{
  private:
  int button_play, button_function;
  int button_forward, button_turn_right, button_turn_left;
  int sequence[10] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

  public:
  ButtonMaster(Buttons _buttons); //constructor
  void setSequence(int new_sequence[10]);
  bool forwardPressed();
  bool turnRightPressed();
  bool turnLeftPressed();
  bool playPressed();
  void run(ControlMotor &motor);
 // void setPWM(int _pwm_left, int _pwm_right);
  //void goForward();
  //void TurnLeft();
  //void TurnRight();
  //void Off();

};


#endif