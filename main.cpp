#include <limits>
#include <unordered_map>
#include <functional>
#include <memory>
#include <dpp/dpp.h>
#include <dpp/snowflake.h>
#include <dpp/queues.h>
#include <dpp/unicode_emoji.h>
#include "UserMessageCollector.hpp"

dpp::cluster& g_bot()
{
  uint32_t intents = dpp::i_default_intents | dpp::i_message_content;
  const char* token = std::getenv("BOT_TOKEN");
  static dpp::cluster bot(token, intents);
  return bot;
}

std::unordered_map<dpp::snowflake, std::unique_ptr<UserMessageCollector>>
  rmMyMsgLaterUserMessages;

// Call when some command(e.g. /call_houkz xd) was executed
void on_slashcommand(const dpp::slashcommand_t& event)
{
  std::cout << "Command: " << event.command.get_command_name() << "\n";
  if (event.command.get_command_name() == "rm_my_msg_later")
  {
    if (!event.command.channel.is_voice_channel())
    {
      event.reply(dpp::message("Вы не можете использовать эту команду здесь")
                    .set_flags(dpp::m_ephemeral));
      return;
    }

    dpp::snowflake userId = event.command.get_issuing_user().id;
    auto msg_collector = std::make_unique<UserMessageCollector>(
      &g_bot(), 3600, userId, event.command.channel_id);

    rmMyMsgLaterUserMessages.emplace(userId, std::move(msg_collector));

    // join channel

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
    dpp::message reply("Ваши сообщения будут удалены после того как вы "
                       "отключитесь от голосового канала");

    reply.set_flags(dpp::m_ephemeral);

    event.reply(reply);
  }
}

// Call when bot was done initialization
void on_ready(const dpp::ready_t&)
{
  std::cout << "bot is ready!\n";
  if (dpp::run_once<struct register_commands_tag>())
  {
    std::cout << "Registering commands...\n";
    g_bot().global_command_create(dpp::slashcommand(
      "rm_my_msg_later",
      "Удалить ваши сообщения в голосовом канале после выхода",
      g_bot().me.id));
  }
}

void on_voice_client_disconnect(const dpp::voice_client_disconnect_t& event)
{
  std::cout << "User disconnected\n";
  auto it = rmMyMsgLaterUserMessages.find(event.user_id);
  if (it != rmMyMsgLaterUserMessages.end())
  {
    it->second->cancel();
    rmMyMsgLaterUserMessages.erase(it);
  }
}

int main()
{
  // Set callbacks
  g_bot().on_log(dpp::utility::cout_logger());
  g_bot().on_slashcommand(on_slashcommand);
  g_bot().on_ready(on_ready);
  g_bot().on_voice_client_disconnect(on_voice_client_disconnect);

  try
  {
    g_bot().start(dpp::st_wait);
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
  }
}
