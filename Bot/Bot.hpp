#pragma once

#include <functional>
#include <utility>
#include <string_view>
#include <iostream>
#include <dpp/dpp.h>
#include "misc/apply_index.hpp"
#include "Command.hpp"

namespace bot
{

namespace detail
{
constexpr std::uint32_t getAllIntents() noexcept
{
  return apply_index<BOT_COMMAND_COUNT()>(
    [](auto... Is)
    {
      return (
        dpp::intents::i_default_intents | ... | GetCommand_t<Is>::k_intents);
    });
}

inline auto getAllCommands(dpp::cluster& bot)
{
  return apply_index<BOT_COMMAND_COUNT()>(
    [&](auto... Is) { return std::tuple{ GetCommand_t<Is>(bot)... }; });
}

using AllCommands = decltype(getAllCommands(std::declval<dpp::cluster&>()));

inline auto getCommandsMap(AllCommands& cmds)
{
  std::cout << "Command count: " << BOT_COMMAND_COUNT() << "\n";
  return apply_index<BOT_COMMAND_COUNT()>(
    [&](auto... Is)
    {
      using S = std::string_view;
      using F = std::function<void(const dpp::slashcommand_t&)>;

      auto name = [](auto I)
      {
        std::cout << "Command: " << GetCommandName_v<I> << "\n";
        return GetCommandName_v<I>;
      };
      auto handler = [&](auto I)
      {
        return [&](const dpp::slashcommand_t& event)
        { return std::get<I>(cmds).handler(event); };
      };

      return std::unordered_map<S, F>{ { name(Is), handler(Is) }... };
    });
}
} // namespace detail

class Bot
{
public:
  Bot(const char* token)
    : m_bot(token, detail::getAllIntents())
    , m_commands(detail::getAllCommands(m_bot))
    , m_commandHandlers(detail::getCommandsMap(m_commands))
  {
    m_bot.on_log(dpp::utility::cout_logger());
    m_bot.on_ready(std::bind_front(&Bot::on_ready, this));
    m_bot.on_slashcommand(std::bind_front(&Bot::on_slashcommand, this));
  }

  void start() { m_bot.start(dpp::st_wait); }

private:
  dpp::cluster m_bot;
  detail::AllCommands m_commands;
  const std::unordered_map<
    std::string_view,
    std::function<void(const dpp::slashcommand_t&)>>
    m_commandHandlers;

  void on_ready(const dpp::ready_t& event)
  {
    if (dpp::run_once<struct register_commands_tag>())
    {
      std::apply(
        [&](auto&... cmds) { ((cmds.init_handler(event)), ...); }, m_commands);

      m_bot.global_bulk_command_create_sync(std::apply(
        [](const auto&... cmds)
        { return std::vector<dpp::slashcommand>{ cmds.getSlashCommand()... }; },
        m_commands));
    }

    std::cout << "Bot ready\n";
  }

  void on_slashcommand(const dpp::slashcommand_t& event)
  {
    std::cout << "Slashcommand: " << event.command.get_command_name() << ": ";

    const auto it = m_commandHandlers.find(event.command.get_command_name());
    if (it != m_commandHandlers.end())
    {
      std::cout << "Handling\n";
      it->second(event);
    }
    else
      std::cout << "Not found\n";
  }
};

} // namespace awoo
