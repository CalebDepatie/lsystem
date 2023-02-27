#pragma once

#include <string_view>
#include <string>
#include <unordered_map>
#include <functional>
#include "core/dyn_array.ipp"

enum actions {
  empty,
  push,
  pop,
  forward,
  move,
  right_turn,
  left_turn,
  turn_in,
  turn_out,
  turn_y_in,
  turn_y_out,
  split,
  half_speed,
  double_speed,
};

struct lsystem {
  int         MAX_ITER;
  int         ANGLE_STEP;
  std::string DRAW_FORWARD; // Move and draw
  std::string MOVE_FORWARD; // Move without drawing
  std::string VARIABLE;     // Variables without command, gets expanded
  std::string AXIOM;
  int         SEED;
  std::unordered_map<char, std::function<std::string()>> RULES;
};

auto generateLSystem(std::string_view filePath) -> lsystem;
auto lexString(lsystem p) -> velox::dyn_array<actions>;
