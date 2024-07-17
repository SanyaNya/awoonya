#include "commands/rm_my_msg_later.hpp"
#include "commands/kick.hpp"
#include "Bot/Bot.hpp"

int main()
{
  try
  {
    bot::Bot bot(std::getenv("BOT_TOKEN"));
    bot.start();
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
  }
}
