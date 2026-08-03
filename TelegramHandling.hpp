// Copyright 2025 vende11s
#include <atomic>

#include "TelegramBotApi.hpp"

#include "OfferQuery.hpp"

namespace communication {
extern std::atomic<bool> doReadConfig;
void HandleTelegram(tba::TelegramBotApi bot);
void sendOffer(const OfferQuery::Offer& offer, tba::TelegramBotApi& bot, std::string& chat_id);
}