#include "game.h"
#include <iostream>
#include <thread>
#include "SDL.h"
#include <functional>
#include <vector>


Game::Game(std::size_t grid_width, std::size_t grid_height)
    : engine(dev()),
      random_w(0, static_cast<int>(grid_width)),
      random_h(0, static_cast<int>(grid_height)) 
      {
  this->grid_height = grid_height;
  this->grid_width = grid_width;
  addSnake();
  PlaceFood();
}

bool Game::isFoodCeil(int x, int y) {
    for (auto food: foods){
      if (food.x == x && food.y == y) return true;
    }
    return false;
}

void Game::RunClient(Controller*  controller, Snake& snake,  Renderer* renderer,
               std::size_t target_frame_duration, bool mainThread) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;

  while (running) {
    frame_start = SDL_GetTicks();


    if (!snake.alive) {

      if (mainThread) return ;

      std::cout << "player died!" << std::endl;

      for (auto point : snake.body){
        foods.push_back(point);
      }

      for (auto it=snakes.begin(); it!=snakes.end(); ++it){
        if(!it->alive){
          it->reset();
          std::cout << "snake: " << it-snakes.begin() << " died" << std::endl;
          break;
        }
      }
      
    }

    // Input, Update, Render - the main game loop.
    controller->HandleInput(running, snake);
    Update(snake);

    if (renderer!= nullptr) renderer->Render(snakes, foods);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      if (renderer!= nullptr) renderer->UpdateWindowTitle(score, frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

void Game::Run(std::vector<Controller*> controllers, Renderer* renderer,
           std::size_t target_frame_duration){
    this->controllers = controllers;

    for (int i =1 ; i < snakes.size(); ++ i){
      std::thread robot([this, controllers, target_frame_duration, i]{
        RobotController rController;
        this->RunClient(&rController, snakes[i], nullptr, target_frame_duration);
      });
      robot.detach();
    }

    this->RunClient(controllers[0], snakes[0], renderer, target_frame_duration, true);

}

void Game::PlaceFood() {
  int x, y;
  while (true) {
    x = random_w(engine) %31;
    y = random_h(engine) %31;
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (foods.size () >= 50)  return;
    
    bool isSnakeCeil = false;
    for (auto snake: snakes){
      if (snake.SnakeCell(x, y)) {
        isSnakeCeil = true;
        break;
      }
    }

    if (!isSnakeCeil && !isFoodCeil(x, y)) {
      SDL_Point food{x,y};

      foods.push_back(food);
    }
  }
}

void Game::Update(Snake &snake) {
  if (!snake.alive) return;

  snake.Update();
  if (!isAlive(snake)){
    snake.alive = false;
  }

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  // Check if there's food over here
  for (auto food=foods.begin(); food!=foods.end(); ++food){
    if (food->x == new_x && food->y == new_y) {
      score++;
      // Grow snake and increase speed.
      snake.GrowBody();

      if (snake.speed < 0.2) snake.speed += 0.02;

      foods.erase(food);
      break;
    }
  }

  PlaceFood();

}

void Game::operator()(Controller* controller, Snake& snake, Renderer* renderer,
           std::size_t target_frame_duration){
    RunClient(controller, snake, renderer,target_frame_duration);
}

bool Game::isAlive(Snake& snake){
    SDL_Point current_cell{
      static_cast<int>(snake.head_x),
      static_cast<int>(snake.head_y)
      }; 

  for (auto const player : snakes){
      for (auto const &item : player.body) {
        if (current_cell.x == item.x && current_cell.y == item.y) {
          std::cout << "die" << std::endl;
          snake.alive = false;
          return snake.alive;
        }
      }
  }

  return snake.alive;
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snakes[0].size; }

void Game::addSnake(){
  while(snakes.size() < 3){
    snakes.push_back(Snake(grid_width, grid_height));
  }
}