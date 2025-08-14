#include "TelegramHandling.hpp"

#include <thread>
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
		!configUpdate - rereads config.json file so if there was any changes, they will be applied
	)";
	return helpText;
}

static std::string up(std::string) {
	return "FastBuyer is up and running!";
}

// potentially scallable command handling
static std::vector <std::pair<std::string, std::string(*)(std::string)>> commands{
	{"!help", help},
	{"!up", up},
	{"!configUpdate", help}
};

namespace communication {
	using json = nlohmann::json;

	void HandleTelegram(tba::TelegramBotApi bot) {
		int lastMessageId = 0;
		while (true) {
			std::this_thread::sleep_for(std::chrono::seconds(2));
			json messageJson(json::parse(bot.getLastMessage()));

			// guards against empty, malformed or already handled messages
			if (!(messageJson.contains("ok") && messageJson["ok"]))
				continue;
			if (messageJson.contains("result") && messageJson["result"].empty())
				continue;
			if(lastMessageId == messageJson["result"][0]["update_id"])
				continue;
			lastMessageId = messageJson["result"][0]["update_id"];
			std::string chat_id = to_string(messageJson["result"][0]["channel_post"]["chat"]["id"]);
			std::string text = messageJson["result"][0]["channel_post"]["text"];
			
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
	}
}