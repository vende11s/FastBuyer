#include "TelegramBotApi.hpp"
#include "OfferQuery.hpp"

bool sendOffer(const Offer& offer, tba::TelegramBotApi& bot) {
	std::string caption = "<b>" +offer.title + "</b>\n" +
		"Cena: " + std::to_string(offer.price) + " zł\n" +
		"Link: " + offer.link;

	return bot.sendPhotoUrl(offer.imageUrl, caption, "HTML");
}