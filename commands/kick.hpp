#pragma once

#include "Bot/Command.hpp"
#include <dpp/permissions.h>

BOT_COMMAND(kick)
{
  static constexpr std::uint32_t k_intents = dpp::i_privileged_intents;

  static constexpr dpp::snowflake k_my_id{ 320850915177529345uz };

  dpp::cluster* m_bot;

  kick(dpp::cluster & bot)
    : m_bot(&bot)
  {
  }

  dpp::slashcommand getSlashCommand() const
  {
    return dpp::slashcommand("kick", "Kick member", m_bot->me.id)
      .set_default_permissions(dpp::p_kick_members)
      .add_option(
        dpp::command_option(dpp::co_user, "user", "User to kick", true));
  }

  void init_handler(const dpp::ready_t&) {}

  void handler(const dpp::slashcommand_t& event)
  {
    if (!event.command.app_permissions.can(dpp::p_kick_members))
    {
      event.reply(
        dpp::message("У меня нет прав на кик").set_flags(dpp::m_ephemeral));
      return;
    }

    dpp::snowflake userId = event.command.get_issuing_user().id;

    if (userId != k_my_id)
    {
      event.reply(dpp::message("ERROR").set_flags(dpp::m_ephemeral));
      return;
    }

    m_bot->guild_member_kick(
      event.command.guild_id,
      std::get<dpp::snowflake>(event.get_parameter("user")));

    event.reply(dpp::message("Kicked").set_flags(dpp::m_ephemeral));
  }
};
