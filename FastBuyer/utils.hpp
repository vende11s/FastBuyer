#pragma once
#include <chrono>
#include <thread>
#include <random>
#include <cmath>
#include <ctime>

#include "TelegramBotApi.hpp"
#include "OfferQuery.hpp"

// Cross-platform thread-safe localtime
static std::tm get_local_tm(std::time_t t) {
	std::tm tm{};
#ifdef _WIN32
	localtime_s(&tm, &t);  // Windows
#else
	localtime_r(&t, &tm);  // Linux/macOS
#endif
	return tm;
}

// Return current time as HH:MM:SS
static std::string current_time_hms() {
	auto now = std::chrono::system_clock::now();
	std::time_t now_time = std::chrono::system_clock::to_time_t(now);
	std::tm tm = get_local_tm(now_time);

	char buf[9]; // HH:MM:SS
	std::strftime(buf, sizeof(buf), "%H:%M:%S", &tm);
	return std::string(buf);
}

inline void LOG(std::string message) {
	std::string data = "[" + current_time_hms() + "] " + message + "\n";

	std::clog << data;
	std::ofstream log("data.log", std::ios_base::app);
	log << data;
}

inline void wait(int seconds, std::mt19937& engine, std::uniform_int_distribution<>& distrib, bool dont_random = 0) {
	int random = distrib(engine);
	if(distrib(engine) % 2 == 0) {
		random = -random; // randomize direction
	}
	random += seconds; // add randomization to the wait time
	random = std::abs(random); // ensure positive random value

	if (dont_random) {
		random = seconds; // ignore randomization if requested
	}

	std::this_thread::sleep_for(std::chrono::seconds(random));
	LOG("Waited for " + std::to_string(random) + " seconds");
}

inline bool sendOffer(const Offer& offer, tba::TelegramBotApi& bot) {
	std::string i_hate_cpp23_utf_support(offer.title.begin(), offer.title.end());
	std::string caption = "<b>" + i_hate_cpp23_utf_support + "</b>\n" +
		"Cena: " + std::to_string(offer.price) + "zl\n" +
		"Link: " + offer.link;

	return bot.sendPhotoUrl(offer.imageUrl, caption, "HTML");
}

inline void drawLogo() {
	std::cout << R"(
 _____         _   ____                        
|  ___|_ _ ___| |_| __ ) _   _ _   _  ___ _ __ 
| |_ / _` / __| __|  _ \| | | | | | |/ _ \ '__|
|  _| (_| \__ \ |_| |_) | |_| | |_| |  __/ |   
|_|  \__,_|___/\__|____/ \__,_|\__, |\___|_|   
                               |___/v1.0.0)"<<std::endl;
}