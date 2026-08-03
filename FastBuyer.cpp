// Copyright 2025 vende11s
// v1.2.1
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <atomic>
#include <thread>

#include <nlohmann/json.hpp>
#include "TelegramBotApi.hpp"

#include "OfferQuery.hpp"
#include "ParseSites.hpp"
#include "utils.hpp"
#include "TelegramHandling.hpp"
#include "ConfigReader.hpp"

using utils::LOG;

int main(){
	// setting fastbuyer up start
    utils::drawLogo();
	LOG("setting FastBuyer up...");

    // Read configuration from config.json
    config::Config config;
	config.read(); 

    tba::TelegramBotApi bot(config.telegram_token);
	bot.chatId = config.chat_id;

    std::map<std::string, int> sentOffers; // url & price
    LOG("setting telegram handler up...");
	std::thread telegramHandlerThread(communication::HandleTelegram, bot);
    LOG("FastBuyer is up, scrapping time!");
    // setting fastbuyer up end

    while (true) {
        for (auto& query : config.queries) {
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
                try{
                    communication::sendOffer(offer, bot, query.chat_id);
                    LOG("Sent offer: " + offer.title + " for " + std::to_string(offer.price) + "zl");
                    sentOffers[offer.link] = offer.price; // update sent offers with the new price
                }
                catch (...) {
                    LOG("Failed to send offer: " + offer.title);
                }
            }
        }
        utils::wait(config.refresh_seconds, config.refresh_seconds_randomization);

        if(communication::doReadConfig.load()) {
			LOG("Trying to update config...");
            try {
                config.read(); // re-read config

				// Update bot and distribution with new config
                bot.chatId = config.chat_id;

                communication::doReadConfig.store(false);
                LOG("Config updated successfully.");
            }
            catch (const std::exception& e) {
                LOG("Failed to update config: " + std::string(e.what()));
            }
		}
    }
}