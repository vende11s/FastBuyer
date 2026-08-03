// Copyright 2025 vende11s
// v1.1.2
#pragma once
#include <iostream>
#include <string>
#include <exception>

#include <cpr/cpr.h>

namespace tba {
class TelegramBotApi {
	constexpr static const char* API_URL = "https://api.telegram.org/bot";
	constexpr static uint16_t MAX_TEXT_LEN = 4096;
	const std::string TOKEN;

	// https://stackoverflow.com/questions/154536/encode-decode-urls-in-c
	std::string url_encode(const std::string& value) {
		std::ostringstream escaped;
		escaped.fill('0');
		escaped << std::hex;

		for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
			std::string::value_type c = (*i);

			if (c < 0 || c > 255)
				continue;

			// Keep alphanumeric and other accepted characters intact
			if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
				escaped << c;
				continue;
			}

			// Any other characters are percent-encoded
			escaped << std::uppercase;
			escaped << '%' << std::setw(2) << int((unsigned char)c);
			escaped << std::nouppercase;
		}

		return escaped.str();
	}

	// type: 1 for local file, 0 for url
	bool sendPhoto(bool type, std::string source, std::string caption, std::string parse_mode, std::string chat_id) {
		if (chat_id.empty() && chatId.empty()) {
			std::cerr << "Chat ID is not specified!" << std::endl;
			return false;
		}

		if (chat_id.empty()) {
			chat_id = chatId;
		}

		cpr::Response r;
		try {
			auto photoPart = type ? cpr::Part{ "photo", cpr::File{source} } : cpr::Part{ "photo", source };

			r = cpr::Post(cpr::Url{ API_URL + TOKEN + "/sendPhoto" },
				cpr::Multipart{ {"chat_id", chat_id},
				photoPart, {"caption", caption}, {"parse_mode", parse_mode}});
		}
		catch (...) {
			std::cerr << "Cpr error sending photo: " << source << std::endl << r.text << std::endl;
			return false;
		}

		return true;
	}

public:
	std::string chatId;

	// sends a text message to the Telegram chat (if the message is too long, it will be split into multiple messages)
	bool sendText(std::string text, std::string chat_id = "") {
		if(chat_id.empty() && chatId.empty()) {
			std::cerr << "Chat ID is not specified!" << std::endl;
			return false;
		}

		if (chat_id.empty()) {
			chat_id = chatId;
		}

		cpr::Response r;
		while (!text.empty()) {
			try {
				r = cpr::Get(cpr::Url{ API_URL + TOKEN + "/sendMessage?chat_id=" + chat_id + "&text=" + url_encode(text.substr(0, MAX_TEXT_LEN)) });
			}
			catch (...) {
				std::cerr << "Cpr error sending message: " << text << std::endl << r.text <<std::endl;
				return false;
			}
			text.erase(0, MAX_TEXT_LEN - 1);
		}
		return true;
	}

	// sends a photo from the given local path to the Telegram chat, allows a caption and parse mode
	// parse_modes options : "Markdown", "MarkdownV2", "HTML"
	// empty parse mode is plain text
	bool sendPhotoFile(std::string photoPath, std::string caption="", std::string parse_mode="", std::string chat_id = "") {
		return sendPhoto(true, photoPath, caption, parse_mode, chat_id);
	}

	// sends a photo from the given url to the Telegram chat, allows a caption and parse mode
	// parse_modes options : "Markdown", "MarkdownV2", "HTML"
	// empty parse mode is plain text
	bool sendPhotoUrl(std::string photoUrl, std::string caption = "", std::string parse_mode = "", std::string chat_id = "") {
		return sendPhoto(false, photoUrl, caption, parse_mode, chat_id);
	}

	// sends a file from the given local path to the Telegram chat
	bool sendFile(const std::string& filePath, std::string chat_id = "") {
		if (chat_id.empty() && chatId.empty()) {
			std::cerr << "Chat ID is not specified!" << std::endl;
			return false;
		}

		if (chat_id.empty()) {
			chat_id = chatId;
		}

		cpr::Response r;
		try {
			r = cpr::Post(cpr::Url{ API_URL + TOKEN + "/sendDocument" },
				cpr::Multipart{ {"chat_id", chat_id},
							   {"document", cpr::File{filePath}} });
		}
		catch (...) {
			std::cerr << "Cpr error sending photo: " << filePath << std::endl << r.text << std::endl;
			return false;
		}

		return true;
	}

	std::string getLastMessage() {
		cpr::Response r;
		try {
			r = cpr::Get(cpr::Url{ API_URL + TOKEN + "/getUpdates?last=1&offset=-1" });
		}
		catch (...) {
			std::cerr << "Cpr error getting last message: " << std::endl << r.text << std::endl;
		}

		return r.text;  // This will return the last message in JSON format
	}

	explicit TelegramBotApi(const std::string& TOKEN)
		: TOKEN(TOKEN) {
	}
};
}  // namespace tba
