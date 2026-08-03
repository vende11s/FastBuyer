// Copyright 2025 vende11s
#include "ConfigReader.hpp"

#include <fstream>

#include <nlohmann/json.hpp>
#include "TelegramBotApi.hpp"

#include "OfferQuery.hpp"
#include "utils.hpp"

namespace config{
	using OfferQuery::Query;
	void Config::read() {
		std::ifstream configStream("config.json");
		if (!configStream) {
			utils::LOG("Could not open config.json, shutting down...");
			throw std::runtime_error("could not open config.json");
		}
		nlohmann::json jsonConfig;
		configStream >> jsonConfig;
		configStream.close();

		refresh_seconds = jsonConfig["config"]["refresh_seconds"];
		refresh_seconds_randomization = jsonConfig["config"]["refresh_seconds_randomization"];
		telegram_token = jsonConfig["config"]["telegram_token"];

		bool is_global_chat_id = false;
		if (jsonConfig["config"].contains("chat_id")) {
			chat_id = jsonConfig["config"]["chat_id"];
			is_global_chat_id = true;
		}

		for (size_t i = 0; i < jsonConfig["queries"].size(); i++) {
			Query query;

			try {
				query.title = jsonConfig["queries"][i]["title"];
				query.type = jsonConfig["queries"][i]["type"]; // "allegro" or "olx"
				query.url = jsonConfig["queries"][i]["url"];
				query.pricePoint = jsonConfig["queries"][i]["price_point"];
				query.positivePrompts = jsonConfig["queries"][i]["positive_prompts"];
				query.negativePrompts = jsonConfig["queries"][i]["negative_prompts"];

				if (jsonConfig["queries"][i].contains("chat_id")) {
					query.chat_id = jsonConfig["queries"][i]["chat_id"];
				}
				else {
					if (!is_global_chat_id) {
						utils::LOG("Chat ID is not specified for query: " + query.title + ", there is no global chat_id to use, shutting down...");
						throw std::runtime_error("No chat_id specified for query: " + query.title);
					}
					query.chat_id = chat_id; // use default chat id from config
				}
			}
			catch (...) {
				utils::LOG("Failed to read query: " + std::to_string(i) + ", shutting down...");
				throw std::runtime_error("Failed to read query: " + std::to_string(i));
			}
			queries.push_back(query);
		}
	}
}