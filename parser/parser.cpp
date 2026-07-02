#include <ios>
#include <iostream>
#include <algorithm>
#include "parser.h"

/*
std::ostream& operator<<(std::ostream& cout, Query::Type query_type)
{
  std::string_view query_name{};
  switch(query_type)
  {
    case Query::Type::q_select: query_name = "select";
    default: query_name = "null";
  }

  cout << query_name;
  return cout;
}
*/

// Trim leading whitespace
void ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// Trim trailing whitespace
void rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// Trim both ends
std::string_view trim(std::string_view s) {
    // Trim leading
    auto start = std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    });
    s.remove_prefix(std::distance(s.begin(), start));

    // Trim trailing
    auto end = std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    });
    s.remove_suffix(std::distance(s.rbegin(), end));

    return s;
}

Parser::Parser(std::string_view user_input) : m_user_input(user_input)
{
  parse_user_query();
  std::cout << "user query finished" << "\n";
}

void Parser::parse_user_query()
{
  switch (identify_query_type())
  {
    case Query::q_select: 
      select_parser();
      std::cout << "select parser finished";
      break;
//    case Query::q_insert: return insert_parser();
//    case Query::q_delete: return delete_parser();
//    case Query::q_create: return create_parser();
//    case Query::q_drop: return drop_parser();
    default: throw std::invalid_argument("Could not match query type");
  }
  
}

const Query::Type Parser::identify_query_type()
{
    auto word = first_word(); 

    if (word == "SELECT") return Query::q_select;
    if (word == "INSERT") return Query::q_insert;
    if (word == "DELETE") return Query::q_delete;
    if (word == "CREATE") return Query::q_create;
    if (word == "DROP")   return Query::q_drop;

    throw std::invalid_argument("Unknown query type: " + std::string(word));
}

const std::string_view Parser::first_word()
{
  auto end {m_user_input.find(' ')};
  return m_user_input.substr(0, end);
}


void Parser::select_parser()
  /*
   * Parses statements of the form
   * SELECT star/(comma seperated column names)
   * FROM table name
   * WHERE condition;
   */
{
  int cursor {0};
  Query::SelectParsing cur_parsing_area {Query::in_select};
  m_user_input = trim(m_user_input);

  while (cursor != std::string_view::npos)
  {
    cursor = m_user_input.find_first_not_of(" ", cursor);

    auto end {m_user_input.find_first_of(" ", cursor)};
    auto token {m_user_input.substr(cursor, end - cursor)};
    cursor = end;

    std::cout << "this is the main loop end - " << end << '\n';
    std::cout << "this is the main loop token - " << token << "token before\n";

    if (token == "SELECT") 
    {
      cur_parsing_area = Query::in_select;
      end = m_user_input.find("FROM") - 1;
      token = m_user_input.substr(cursor + 1, end - cursor);
      std::cout << "detected select\n";
    }
    else if (token == "FROM")
    {
      cur_parsing_area = Query::in_from;
      end = m_user_input.find("WHERE") - 1;
      token = m_user_input.substr(cursor + 1, end - cursor);
      std::cout << "detected from\n";
    }
    else if (token == "WHERE") 
    {
      cur_parsing_area = Query::in_condition;
      end = m_user_input.find(";");
      token = m_user_input.substr(cursor + 1, end - cursor);
      std::cout << "detected where\n";
    }
    else if (token == ";") 
    {
      std::cout << "breaking main loop\n";
      break;
    }

    std::cout << "this is the end after branch " << end << '\n';
    std::cout << "this is the token " << token << '\n';
    trim(token); 
    populate_user_instructions(token, cur_parsing_area);
    
    cursor = end;

  };
  std::cout << "finish loop";
}



template <typename T>
void Parser::populate_user_instructions(std::basic_string_view<T> token, Query::SelectParsing cur_parsing_area)
{
  std::cout << "Entered populate user instructions \n";
  if (cur_parsing_area == Query::in_select)
    user_instructions.select_column_names = parse_in_select(token);

  else if (cur_parsing_area == Query::in_from)
    user_instructions.from_table_name = trim(token);

  else if (cur_parsing_area == Query::in_condition)
    user_instructions.conditions = parse_in_conditions(token);
}



template <typename T>
std::vector<std::string_view> Parser::parse_in_select(std::basic_string_view<T> token)
{
  std::vector<std::string_view> column_names(20);
  int cursor {0};
  std::string_view::size_type prev_end {std::string_view::npos};
  while (cursor != std::string_view::npos)
  {
    auto end {token.find_first_of(",")};
    if (end == prev_end) end = std::string_view::npos;
    auto column {token.substr(cursor, end - cursor)};
    column_names.push_back(static_cast<std::string_view>(trim(column)));

    if (end == std::string_view::npos) break;
    cursor = end + 1;
    prev_end = end;
  };
  return column_names;
}



template <typename T>
Query::Instructions::Conditions Parser::parse_in_conditions(std::basic_string_view<T> token)
{
  // assume form of col1 cond col2
  Query::Instructions::Conditions statement_inequality_data{};
  std::cout << "Entered parse conditions\n";

  // start with non leading whitespace
  token = token.substr(token.find_first_not_of(" "), token.length());
  
  // get the first column name before the whitespace
  auto end {token.find_first_of(" ")};
  statement_inequality_data.first_table_name = token.substr(0, end);
  std::cout << "first table name - " << token.substr(0, end) << '\n';

  token = token.substr(end + 1, token.length());
  std::cout << "Token after shortening " << token << '\n';
  
  // get the inequality 
  end = token.find_first_of(" ");
  statement_inequality_data.inequality = inequality_to_enum(token.substr(0, end));
  std::cout << "inequality name - " << token.substr(0, end) << '\n';

  token = token.substr(end + 1, token.length());
  std::cout << "Token after shortening " << token << '\n';

  // get the second column name
  end = std::string_view::npos;
  statement_inequality_data.second_table_name = token.substr(0, end);
  std::cout << "second table name - " << token.substr(0, end) << '\n';
  
  return statement_inequality_data;
}

Query::Instructions Parser::get_user_instructions()
{
  return user_instructions;
}

Query::Inequalities Parser::inequality_to_enum(std::string_view inequality_substring)
{
  if (inequality_substring == "<=") return Query::less_than_equal;
  if (inequality_substring == "<") return Query::less_than;
  if (inequality_substring == ">=") return Query::greater_than_equal;
  if (inequality_substring == ">") return Query::greater_than;
  if (inequality_substring == "==") return Query::equal;
  if (inequality_substring == "!=") return Query::not_equal;
  std::cout << "did not match any condition" << '\n';
  throw std::io_errc(); 
  return Query::equal;
}

std::ostream& operator<<(std::ostream& cout, const Query::Instructions& instructions)
{
  cout << "Our instructions consists of the following \n";
  cout << "First the table names - ";
  for (const auto& column_name: instructions.select_column_names)
  {
    cout << column_name << " ";
  }

  cout << "\nThis is from the table - " << instructions.from_table_name << '\n';
  
  cout << "We also have the condition to uphold - " << "\ntable one - " << instructions.conditions.first_table_name << "\ntable two - " << instructions.conditions.second_table_name << "\n condition " << instructions.conditions.inequality << "\n";

  return cout;
}
