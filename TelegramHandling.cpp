// Copyright 2025 vende11s
#include "TelegramHandling.hpp"

#include <thread>
#include <atomic>
#include <chrono>
#include <string>

#include <nlohmann/json.hpp>
#include "TelegramBotApi.hpp"

#include "utils.hpp"

static std::string help(std::string) {
	std::string helpText =
		R"(Available commands:
		!help - show this help message

		!up - just checks if FastBuyer is alive

		!configUpdate - rereads config.json, any changes will be applied before next query (except for telegram_token, its read only at the startup)
	)";
	return helpText;
}

static std::string up(std::string) {
	return "FastBuyer is up and running!";
}

static std::string configUpdate(std::string) {
	communication::doReadConfig.store(true);
	return "config will be changed before next query.";
}

// potentially scallable command handling
static std::vector <std::pair<std::string, std::string(*)(std::string)>> commands{
	{"!help", help},
	{"!up", up},
	{"!configUpdate", configUpdate}
};

namespace communication {
using json = nlohmann::json;
std::atomic<bool> doReadConfig(false);

void HandleTelegram(tba::TelegramBotApi bot) {
	int lastMessageId = 0;
	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(2));

		// The whole iteration is wrapped so that a bad Telegram response
		// (empty/malformed JSON, missing fields, non-text updates) never
		// escapes the thread function and terminates the whole process.
		try {
			std::string rawMessage = bot.getLastMessage();
			if (rawMessage.empty())
				continue; // network error, nothing to parse

			json messageJson = json::parse(rawMessage, nullptr, false);
			if (messageJson.is_discarded())
				continue; // malformed JSON

			// guards against empty, malformed or already handled messages
			if (!(messageJson.contains("ok") && messageJson["ok"]))
				continue;
			if (!messageJson.contains("result") || messageJson["result"].empty())
				continue;

			const auto& update = messageJson["result"][0];
			if (!update.contains("update_id"))
				continue;
			if (lastMessageId == update["update_id"])
				continue;

			// we only handle channel posts that carry a text body
			if (!update.contains("channel_post"))
				continue;
			const auto& post = update["channel_post"];
			if (!post.contains("date") || !post.contains("text") || !post.contains("chat"))
				continue;

			if (post["date"] < std::chrono::system_clock::to_time_t(std::chrono::system_clock::now() - std::chrono::seconds(120))) {
				utils::LOG("Ignoring old message, update_id: " + std::to_string(update["update_id"].get<int>()));
				lastMessageId = update["update_id"];
				continue; // ignore messages older than 120 seconds
			}
			lastMessageId = update["update_id"];
			std::string chat_id = to_string(post["chat"]["id"]);
			std::string text = post["text"];

			utils::LOG("Received message: " + text + " from chat_id: " + chat_id);

			bool commandFound = false;
			for(auto& command : commands) {
				if(text == command.first) { // command found
					commandFound = true;
					std::string response = command.second(text);
					if(!response.empty()) {
						bot.sendText(response, chat_id);
					}
					break; // no need to check other commands
				}
			}
			if(!commandFound) {
				// if no command found
				bot.sendText("Wym by " + text + "\nhere's what i've got:\n" + help(""), chat_id);
			}
		}
		catch (const std::exception& e) {
			utils::LOG("Telegram handler error (ignored): " + std::string(e.what()));
		}
	}
}

void sendOffer(const OfferQuery::Offer& offer, tba::TelegramBotApi& bot, std::string& chat_id) {
	std::string i_hate_cpp23_utf_support(offer.title.begin(), offer.title.end());
	std::string caption = "<b>" + i_hate_cpp23_utf_support + "</b>\n" +
		"Cena: " + std::to_string(offer.price) + "zl\n" +
		"Link: " + offer.link;

	if (!bot.sendPhotoUrl(offer.imageUrl, caption, "HTML", chat_id)) {
		throw std::runtime_error("Failed to send offer photo: " + offer.imageUrl);
	}
}
}