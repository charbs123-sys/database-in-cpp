#include <iostream>

#include "../parser/parser.h"
#include "../engine/engine.h"

int main (int argc, char *argv[]) {

  std::cout << "Start of execution \n";
// main loop recieving user input 
  // std::string user_input {};
  // std::cout << "enter your SQL query here: ";
  std::string user_input {"SELECT col_1, col_2 FROM test_db WHERE col_1 <= col2;"};
  // std::getline(std::cin >> std::ws, user_input);

  Parser parse(user_input);
  Engine execute_instructions(parse.get_user_instructions());


  return 0;
}
