#pragma once

#include <dpp/dpp.h>
#include <dpp/collector.h>
#include <dpp/snowflake.h>

extern dpp::cluster g_bot;

using UserMessageCollector_t =
  dpp::collector<dpp::message_create_t, dpp::snowflake>;

struct UserMessageCollector : UserMessageCollector_t
{
  dpp::snowflake m_userId;
  dpp::snowflake m_channelId;

  UserMessageCollector(
    dpp::cluster* cl,
    std::uint64_t t,
    dpp::snowflake userId,
    dpp::snowflake channelId)
    : UserMessageCollector_t(cl, t, cl->on_message_create)
    , m_userId(userId)
    , m_channelId(channelId)
  {
  }

  virtual void completed(const std::vector<dpp::snowflake>& list) override
  {
    g_bot.message_delete_bulk(list, m_channelId);
  }

  virtual const dpp::snowflake* filter(
    const dpp::message_create_t& element) override
  {
    if (element.msg.author.id == m_userId)
      return &element.msg.id;

    return nullptr;
  }
};
