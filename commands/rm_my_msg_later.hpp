#pragma once

#include <unordered_map>
#include <functional>
#include <iostream>
#include <dpp/dpp.h>
#include <utility>
#include "Bot/Command.hpp"
#include "UserMessageCollector.hpp"

BOT_COMMAND(rm_my_msg_later)
{
  static constexpr std::uint32_t k_intents = dpp::i_message_content;

  dpp::cluster* m_bot;
  std::unordered_map<dpp::snowflake, UserMessageCollector>
    rmMyMsgLaterUserMessages;

  rm_my_msg_later(dpp::cluster & bot)
    : m_bot(&bot)
  {
  }

  dpp::slashcommand getSlashCommand() const
  {
    return dpp::slashcommand(
      "rm_my_msg_later",
      "Удалить ваши сообщения в голосовом канале после выхода",
      m_bot->me.id);
  }

  void init_handler(const dpp::ready_t&)
  {
    m_bot->on_voice_client_disconnect(
      std::bind_front(&rm_my_msg_later::on_voice_client_disconnect, this));
  }

  void join_issuer_channel(const dpp::slashcommand_t& event)
  {
    dpp::guild* g = dpp::find_guild(event.command.guild_id);
    auto current_vc = event.from->get_voice(event.command.guild_id);
    bool join_vc = true;

    if (current_vc)
    {
      auto users_vc =
        g->voice_members.find(event.command.get_issuing_user().id);

      if (
        users_vc != g->voice_members.end() &&
        current_vc->channel_id == users_vc->second.channel_id)
      {
        join_vc = false;
      }
      else
      {
        event.from->disconnect_voice(event.command.guild_id);

        join_vc = true;
      }
    }

    if (join_vc)
    {
      std::cout << "Joining new VC!\n";
      if (!g->connect_member_voice(event.command.get_issuing_user().id))
      {
        event.reply(dpp::message("Вы не находитесь ни в каком канале!")
                      .set_flags(dpp::m_ephemeral));
        return;
      }
    }
  }

  void handler(const dpp::slashcommand_t& event)
  {
    if (!event.command.channel.is_voice_channel())
    {
      event.reply(dpp::message("Вы не можете использовать эту команду здесь")
                    .set_flags(dpp::m_ephemeral));
      return;
    }

    dpp::snowflake userId = event.command.get_issuing_user().id;

    rmMyMsgLaterUserMessages.emplace(std::piecewise_construct,
      std::forward_as_tuple(userId),
      std::forward_as_tuple(m_bot, 3600, userId, event.command.channel_id));

    join_issuer_channel(event);

    dpp::message reply("Ваши сообщения будут удалены после того как вы "
                       "отключитесь от голосового канала");

    reply.set_flags(dpp::m_ephemeral);

    event.reply(reply);
  }

  void on_voice_client_disconnect(const dpp::voice_client_disconnect_t& event)
  {
    std::cout << "User disconnected\n";
    auto it = rmMyMsgLaterUserMessages.find(event.user_id);
    if (it != rmMyMsgLaterUserMessages.end())
    {
      it->second.cancel();
      rmMyMsgLaterUserMessages.erase(it);
    }
  }
};
