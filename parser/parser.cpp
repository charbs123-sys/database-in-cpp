#include <iostream>
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

Parser::Parser(std::string_view user_input) : m_user_input(user_input)
{
  const Query::Type query_type {identify_query_type()};
  std::cout << "This is the query type " << query_type << "\n";
  parse_user_query(query_type);
}

const Query::Type Parser::identify_query_type()
{
    auto word = first_word(); // replace query_ with your string member

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

void Parser::parse_user_query(Query::Type query_type)
{
  switch (query_type)
  {
    case Query::q_select: 
      select_parser();
      break;
//    case Query::q_insert: return insert_parser();
//    case Query::q_delete: return delete_parser();
//    case Query::q_create: return create_parser();
//    case Query::q_drop: return drop_parser();
    default: std::cout << "No matching queries.\n";
  }
  
  throw std::invalid_argument("Could not match query type");
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

  while (cursor != std::string_view::npos)
  {
    cursor = m_user_input.find_first_not_of(" ", cursor);

    auto end {m_user_input.find_first_of(" ", cursor)};
    auto token {m_user_input.substr(cursor, end - cursor)};
    cursor = end;

    if (token == "SELECT") 
    {
      cur_parsing_area = Query::in_select;
      end = m_user_input.find("FROM");
      token = m_user_input.substr(cursor, end - cursor);
      std::cout << "detected select\n";
    }
    else if (token == "FROM")
    {
      cur_parsing_area = Query::in_from;
      end = m_user_input.find("WHERE");
      token = m_user_input.substr(cursor, end - cursor);
      std::cout << "detected from\n";
    }
    else if (token == "WHERE") 
    {
      cur_parsing_area = Query::in_condition;
      end = m_user_input.find(";");
      token = m_user_input.substr(cursor, end - cursor);
      std::cout << "detected where\n";
    }
    else if (token == ";") break;

    std::cout << "this is the token " << token << '\n';
    populate_user_instructions(token, cur_parsing_area);
    
    cursor = end;

  }
}



template <typename T>
void Parser::populate_user_instructions(std::basic_string_view<T> token, Query::SelectParsing cur_parsing_area)
{
  if (cur_parsing_area == Query::in_select)
    user_instructions.select_column_names = parse_in_select(token);

  else if (cur_parsing_area == Query::in_from)
    user_instructions.from_table_name = token;

  else if (cur_parsing_area == Query::in_condition)
    user_instructions.conditions = parse_in_conditions(token);
}



template <typename T>
std::vector<std::string_view> parse_in_select(std::basic_string_view<T> token)
{
  std::vector<std::string_view> column_names(20);
  int cursor {0};
  while (cursor != std::string_view::npos)
  {
    auto end {token.find_first_of(",")};
    auto column {token.substr(cursor, end - cursor)};
    column_names.push_back(static_cast<std::string_view>(column));
    cursor = end;
  };
}



template <typename T>
Query::Instructions::Conditions parse_in_conditions(std::basic_string_view<T> token)
{
  std::vector<std::string_view> column_names(20);
  int cursor {0};
  while (cursor != std::string_view::npos)
  {
    auto end {token.find_first_of(",")};
    auto column {token.substr(cursor, end - cursor)};
    column_names.push_back(static_cast<std::string_view>(column));
    cursor = end;
  };
}


