#pragma once

#include <string>
#include <vector>

namespace Query
{
  enum Type 
  {
    q_select,
    q_insert,
    q_delete,
    q_create,
    q_drop,
    number_of_queries,
  };

  struct Instructions
  {
    std::vector<std::string_view> select_column_names{};
    std::string_view from_table_name{};

    struct Conditions 
    {
      std::string_view first_table_name {};
      std::string_view inequality{"null"};
      std::string_view second_table_name {};
    };

    Conditions conditions{};
  };

  enum SelectParsing
  {
    in_select,
    in_from,
    in_condition,
  };
}


class Parser
{
  private:
    std::string_view m_user_input{}; 
    const Query::Type identify_query_type();
    const std::string_view first_word();
    Query::Instructions user_instructions;

    void parse_user_query(Query::Type query_type);
    void select_parser();
    
    template <typename T>
    void populate_user_instructions(std::basic_string_view<T> token, Query::SelectParsing cur_parsing_area);
    template <typename T>
    std::vector<std::string_view> parse_in_select(std::basic_string_view<T> token);
    template <typename T>
    Query::Instructions::Conditions parse_in_conditions(std::basic_string_view<T> token);


  public:
    Parser(std::string_view user_input);
};

