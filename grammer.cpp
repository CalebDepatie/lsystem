#include "grammer.hpp"
#include <string>
#include <sstream>
#include <regex>
#include <iostream>
#include <fstream>
#include <tuple>
#include <unordered_set>
#include <algorithm>
#include <ranges>

// lexing and parsing are the same due to the token syntax
auto lexString(lsystem p) -> std::vector<actions> {
  // expansion
  std::string expanded_string = std::move(p.AXIOM.data());
  srand(p.SEED);

  for (int i = 0; i < p.MAX_ITER; i++) {
    std::string newString = "";

    for (int ci = 0; ci < expanded_string.size(); ci++) {
      if (p.RULES.contains(expanded_string[ci])) {
        auto func = p.RULES[expanded_string[ci]];
        const std::string rule = func();
        newString += rule;

      } else {
        newString += expanded_string[ci];
      }
    }
    expanded_string = std::move(newString);

  }

  auto action_list = std::vector<actions>();

  for (int i = 0; i<expanded_string.size(); i++) {
    char c = expanded_string[i];
    actions action = empty;

    switch (c) {
      case '[': {
        action = push;
        break;
      }
      case ']': {
        action = pop;
        break;
      }
      case '+': {
        action = left_turn;
        break;
      }
      case '-': {
        action = right_turn;
        break;
      }
      case '>': {
        action = turn_in;
        break;
      }
      case '<': {
        action = turn_out;
        break;
      }
      case '.': {
        action = turn_y_in;
        break;
      }
      case ',': {
        action = turn_y_out;
        break;
      }
      case '=': {
        action = split;
        break;
      }
      case '/': {
        action = half_speed;
        break;
      }
      case '*': {
        action = double_speed;
        break;
      }
    }

    // check modifiers
    if (p.DRAW_FORWARD.find(c) != std::string::npos) {
      action = forward;

    } else if (p.MOVE_FORWARD.find(c) != std::string::npos) {
      action = move;
    }

    if (action == empty)
      continue;

    action_list.emplace_back(action);
  }

  return action_list;
}

auto generateLSystem(std::string_view filePath) -> lsystem {
  lsystem newProgram;

  std::ifstream definition_file(filePath.data());

  if (!definition_file.is_open()) {
    std::cerr << "Could not open provided definition file at " << filePath << std::endl;
    exit(0);
  }

  while(!definition_file.eof()) {
    std::string cur_line;
    std::getline(definition_file, cur_line);

    if (cur_line == "")
      break;

    //split string to words
    std::vector<std::string> words;
    std::istringstream iss(cur_line);
    std::string cur_word;
    while (std::getline(iss, cur_word, ' ')) {
      words.emplace_back(cur_word);
    }

    if (words[0] == "iterations:") {
      newProgram.MAX_ITER = std::stoi(words[1].data());

    } else if (words[0] == "draw_forward:") {
      for (int i=1; i<words.size(); i++) {
        newProgram.DRAW_FORWARD += words[i];
      }

    } else if (words[0] == "move_forward:") {
      for (int i=1; i<words.size(); i++) {
        newProgram.MOVE_FORWARD += words[i];
      }

    } else if (words[0] == "variable:") {
      for (int i=1; i<words.size(); i++) {
        newProgram.VARIABLE += words[i];
      }

    } else if (words[0] == "turn_angle:") {
      newProgram.ANGLE_STEP = std::stoi(words[1].data());

    } else if (words[0] == "seed:") {
      newProgram.SEED = std::stoi(words[1].data());

    } else if (words[0] == "axiom:") {
      newProgram.AXIOM = words[1];

    } else if (words[0] == "rules:") {
      std::vector<std::tuple<std::string, std::string>> temp_rules;
      std::unordered_set<char> keys;

      // Collect the keys and their rules
      for (int i=1; i<words.size(); i+=2) {
        char key = words[i][0];
        keys.insert(key);
        auto value = words[i+1];

        temp_rules.emplace_back(std::make_tuple(words[i], value));
      }

      // seperate into functions and add to the rule list
      // todo: This code is just awful. Ranges to clean up the logic?
      for (char k : keys) {
        std::pair<char, std::function<std::string()>> rule;

        auto predicate = [&k](std::tuple<std::string, std::string> el) {
          return std::get<0>(el)[0] == k;
        };

        std::vector<std::tuple<std::string, std::string>> rules;
        for (const auto& t_r : temp_rules) {
          if (predicate(t_r))
            rules.push_back(t_r);
        }

        if (rules.size() == 1) {
          // deterministic
          const auto [key, value] = rules[0];

          auto func = [value]() { return value; };

          rule = std::pair<char, std::function<std::string()>>(key[0], func);
        } else {
          //stochiastic : assume two rules
          // get %'s
          auto getPercent = [](std::string s) {
            // erase <key>(
            s.erase(0, 2);
            // erase )
            s.erase(s.size()-1, 1);
            return std::stoi(s);
          };

          int percent_1 = getPercent(std::get<0>(rules[0]));
          int percent_2 = getPercent(std::get<0>(rules[1]));

          auto value_1 = std::get<1>(rules[0]);
          auto value_2 = std::get<1>(rules[1]);

          auto func = [percent_1, percent_2, value_1, value_2]() {
            int r = rand() % 100;

            // std::cout << r << std::endl;
            // std::cout << percent_1 << "" << value_1 << std::endl;
            // std::cout << percent_2 << "" << value_2 << std::endl << std::endl;

            if (r < percent_1) {
              return value_1;
            } else if (r < (percent_1+percent_2)) {
              return value_2;
            }
          };

          rule = std::pair<char, std::function<std::string()>>(std::get<0>(rules[0])[0], func);
        }

        newProgram.RULES.insert(rule);
      }
    }
  }

  definition_file.close();

  // std::cout << "Iter: " << newProgram.MAX_ITER << std::endl;
  // std::cout << "Angle: " << newProgram.ANGLE_STEP << std::endl;
  // std::cout << "Axiom: " << newProgram.AXIOM << std::endl;
  // std::cout << "Draw Forward: " << newProgram.DRAW_FORWARD << std::endl;
  // std::cout << "Move Forward: " << newProgram.MOVE_FORWARD << std::endl;
  // std::cout << "Variable: " << newProgram.VARIABLE << std::endl;
  // std::cout << "Rules:" << std::endl;
  // for (const auto& pair : newProgram.RULES) {
  //   std::cout << pair.first << ": " << pair.second << std::endl;
  // }

  return newProgram;
}
