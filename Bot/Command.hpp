#pragma once

#include <type_traits>
#include <dpp/dpp.h>
#include "misc/ct_counter.hpp"

#define BOT_COMMAND(NAME)                                                      \
  struct NAME;                                                                 \
                                                                               \
  template<>                                                                   \
  constexpr const char* getCommandName(                                        \
    std::integral_constant<std::size_t, CT_COUNTER_READ(CommandTag)>)          \
  {                                                                            \
    return #NAME;                                                              \
  }                                                                            \
                                                                               \
  template<>                                                                   \
  struct GetCommand<CT_COUNTER_READ(CommandTag)> : std::type_identity<NAME>    \
  {                                                                            \
  };                                                                           \
                                                                               \
  CT_COUNTER_INC(CommandTag);                                                  \
                                                                               \
  struct NAME

#define BOT_COMMAND_COUNT() CT_COUNTER_READ(CommandTag)

struct CommandTag
{
};

template<std::size_t Id>
struct GetCommand;

template<std::size_t Id>
using GetCommand_t = typename GetCommand<Id>::type;

template<std::size_t Id>
constexpr const char* getCommandName(std::integral_constant<std::size_t, Id>);

template<std::size_t Id>
constexpr const char* GetCommandName_v =
  getCommandName(std::integral_constant<std::size_t, Id>{});
