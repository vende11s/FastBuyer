// Copyright 2025 vende11s
#include <string>

#include "OfferQuery.hpp"

#include "TelegramBotApi.hpp"

namespace config {
	struct Config {
		int refresh_seconds = 600; // default just in case
		int refresh_seconds_randomization = 120; // default just in case
		std::string telegram_token;
		std::string chat_id; // global chat id
		std::vector<OfferQuery::Query> queries;

		void read();
	};
}