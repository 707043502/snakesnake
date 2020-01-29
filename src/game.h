#ifndef GAME_H
#define GAME_H

#include <vector>
#include <random>
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"

class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height);
  void Run(std::vector<Controller*> controllers, Renderer* renderer,
           std::size_t target_frame_duration);
  void operator()(Controller*  controller, Snake& snake, Renderer* renderer,
           std::size_t target_frame_duration);
  void RunClient(Controller*  controller, Snake& snake, Renderer* renderer,
           std::size_t target_frame_duration, bool mainThread=false);
 
  int GetScore() const;
  int GetSize() const;
  bool isFoodCeil(int x, int y);
  bool isAlive(Snake& snake);
  void addSnake();

 private:
  std::vector<Snake> snakes;
  std::vector<SDL_Point> foods;
  std::vector<Controller*> controllers;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;
  int  grid_width;
  int  grid_height;
  int score{0};

  void PlaceFood();
  void Update(Snake& snake);
};

#endif