#include <iostream>

#include "../parser/parser.h"

int main (int argc, char *argv[]) {

  std::cout << "Start of execution \n";
// main loop recieving user input 
  // std::string user_input {};
  // std::cout << "enter your SQL query here: ";
  std::string user_input {"SELECT col_1, col_2 FROM tablename WHERE condition;"};
  // std::getline(std::cin >> std::ws, user_input);

  Parser parse(user_input);


  return 0;
}
