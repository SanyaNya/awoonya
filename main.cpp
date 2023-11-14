#include <dpp/dpp.h>
#include <dpp/unicode_emoji.h>

int main()
{
    dpp::cluster bot(std::getenv("BOT_TOKEN"));

    bot.on_log(dpp::utility::cout_logger());

    bot.on_slashcommand([](const dpp::slashcommand_t& event)
    {
        std::cout << "Command: " << event.command.get_command_name() << "\n";
        if(event.command.get_command_name() == "call_houkz")
        {
            dpp::message msg(event.command.channel_id, "");
	            
            msg.add_component(
                dpp::component().add_component(
                    dpp::component()
                        .set_label("Звонок Хоукзу")
                        .set_type(dpp::cot_button)
                        .set_emoji(dpp::unicode_emoji::smile)
                        .set_style(dpp::cos_danger)
                        .set_id("call_houkz_button_id")
                )
            );
    
            event.reply(msg);
        }
    });

    bot.on_button_click([&bot](const dpp::button_click_t& event)
    {
        std::cout << "button: " << event.custom_id << "\n";
        if(event.custom_id == "call_houkz_button_id")
        {
            event.reply(dpp::message("Уведомляем Хоукза...").set_flags(dpp::m_ephemeral));

            const auto guild_id = event.command.get_guild().id;
            const auto member_map = bot.guild_search_members_sync(guild_id, "Dim'ya", 1);
            const auto houkz_id = member_map.begin()->first;

            bot.direct_message_create(
                houkz_id, 
                dpp::message("Хоукз, " + event.command.get_issuing_user().global_name + " хочет с вами связаться"), 
                [](const dpp::confirmation_callback_t& cb){});
        }
	});

    bot.on_ready([&bot](const dpp::ready_t& event)
    {
        std::cout << "Bot is ready!\n";
        if(dpp::run_once<struct register_commands_tag>())
        {
            std::cout << "Registering commands...\n";
            bot.global_command_create(dpp::slashcommand("call_houkz", "Лучше звоните Хоукзу", bot.me.id));
        }
    });

    bot.start(dpp::st_wait);
}