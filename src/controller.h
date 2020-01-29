#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "snake.h"

class Controller {
 public:
  virtual void HandleInput(bool &running, Snake &snake) const = 0;

 protected:
   void ChangeDirection(Snake &snake, Snake::Direction input,
                       Snake::Direction opposite) const;
};

class PlayerController:public Controller {
public:
  void HandleInput (bool& running, Snake& snake) const override;
};

class RobotController:public Controller{
 public:
  void HandleInput (bool& running, Snake& snake) const override;   
};



#endif