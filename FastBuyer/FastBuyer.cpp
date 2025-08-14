// Copyright 2025 vende11s
// v1.0.0
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <random>

#include <nlohmann/json.hpp>
#include "TelegramBotApi.hpp"

#include "OfferQuery.hpp"
#include "ParseSites.hpp"
#include "utils.hpp"

using utils::LOG;

int main(){
	// setting fastbuyer up start
    utils::drawLogo();
	LOG("setting FastBuyer up...");
    std::ifstream configStream("config.json");
    if (!configStream) {
        std::cerr << "Could not open config.json\n";
        return 1;
    }
    nlohmann::json jsonConfig;
    configStream >> jsonConfig;
	configStream.close();

	int refresh_seconds = jsonConfig["config"]["refresh_seconds"];
    // setting up random number generator for randomization
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, jsonConfig["config"]["refresh_seconds_randomization"]);

    tba::TelegramBotApi bot(jsonConfig["config"]["telegram_token"]);
    bool IS_GLOBAL_CHAT_ID = false;
    if(jsonConfig["config"].contains("chat_id")) {
        bot.chatId = jsonConfig["config"]["chat_id"];
        IS_GLOBAL_CHAT_ID = true;
    }
    
    std::vector<Query> queries;
    std::map<std::string, int> sentOffers; // url & price

    for (size_t i = 0; i < jsonConfig["queries"].size(); i++) {
        Query query;

        try {
            query.title = jsonConfig["queries"][i]["title"];
            query.type = jsonConfig["queries"][i]["type"]; // "allegro" or "olx"
            query.url = jsonConfig["queries"][i]["url"];
            query.pricePoint = jsonConfig["queries"][i]["price_point"];
            query.positivePrompts = jsonConfig["queries"][i]["positive_prompts"];
            query.negativePrompts = jsonConfig["queries"][i]["negative_prompts"];

            if(jsonConfig["queries"][i].contains("chat_id")) {
                query.chat_id = jsonConfig["queries"][i]["chat_id"];
            } else {
                if (!IS_GLOBAL_CHAT_ID) {
					LOG("Chat ID is not specified for query: " + query.title + ", there is no global chat_id to use, shutting down...");
                    return 1;
                }
                query.chat_id = bot.chatId; // use default chat id from config
			}
        }
        catch (...) {
            std::cerr << "Failed to parse query\n";
            return 1;
        }
		queries.push_back(query);
    }
    LOG("FastBuyer is up, scrapping time!");
    // setting fastbuyer up end

    while (true) {
        for (auto& query : queries) {
            std::vector<Offer> offers;
            try {
                if (query.type == "allegro") {
					LOG("Scrapping allegro offers for query: " + query.title);
                    offers = parse::parseAllegroLokalnie(query.url);
                    LOG("Found " + std::to_string(offers.size()) + " allegro offers for query: " + query.title);
                }
                else if (query.type == "olx") {
                    LOG("Scrapping olx offers for query: " + query.title);
                    offers = parse::parseOlx(query.url);
                    LOG("Found " + std::to_string(offers.size()) + " olx offers for query: " + query.title);
                }
            }
            catch (...) {
				LOG("Failed to parse offers for query: " + query.title);
            }

            for (auto& offer : offers) {
                if (!offer.valid)
                    continue;
                if (offer.price > query.pricePoint)
                    continue;
                // if offer was already sent at better or equal price
                if (sentOffers.find(offer.link) != sentOffers.end() && sentOffers[offer.link] <= offer.price)
                    continue;
                if (!parse::filterOffer(offer.title, query.positivePrompts, query.negativePrompts))
                    continue;

				// send offer to Telegram
                if (utils::sendOffer(offer, bot, query.chat_id)) {
					LOG("Sent offer: " + offer.title + " for " + std::to_string(offer.price) + "zl");
					sentOffers[offer.link] = offer.price; // update sent offers with the new price
                }
                else {
					LOG("Failed to send offer: " + offer.title);
                }
            }
        }
        utils::wait(refresh_seconds, gen, distrib);
    }
}