// Copyright 2025 vende11s
#include "ParseSites.hpp"

#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
#include <iostream>

#include <cpr/cpr.h>

#include "OfferQuery.hpp"
#include "utils.hpp"

using namespace OfferQuery;
using utils::LOG;

static std::string getSite(std::string url) {
    cpr::Response r = cpr::Get(cpr::Url{ url });
    return r.text;
}

// lokalnie_offer_id get href as id
// mlc-itembox__image__wrapper - img
// itemOffered - title
// ml-offer-price__dollars price
static Offer parseOneOfferAllegroLokalnie(std::string& raw_html) {
    Offer offer;
    int pos = raw_html.find("lokalnie_offer_id");
    if (pos == std::string::npos) {
        offer.valid = false;
        return offer; // No offer found
    }
    raw_html.erase(0, pos);
    pos = raw_html.find("href=\"");

    for (size_t i = pos + 6; i < raw_html.size(); i++) {
        if (raw_html[i] == ' ') {
            break;
        }

        offer.link += raw_html[i];
    }

    offer.link = "https://allegrolokalnie.pl" + offer.link;
    offer.link.pop_back();

    pos = raw_html.find("https://a.allegroimg.com/");
    for (size_t i = pos; i < raw_html.size(); i++) {
        if (raw_html[i] == ' ') {
            break;
        }

        offer.imageUrl += raw_html[i];
    }
    offer.imageUrl.pop_back();

    pos = raw_html.find("itemOffered");
    for (size_t i = pos + 13; i < raw_html.size(); i++) {
        if (raw_html.substr(i, 5) == "</h3>") {
            break;
        }

        offer.title += raw_html[i];
    }

    pos = raw_html.find("ml-offer-price__dollars");
    std::string priceStr = "";
    for (size_t i = pos + 25; i < raw_html.size(); i++) {
        if (raw_html.substr(i, 7) == "</span>") {
            break;
        }

        if (raw_html[i] == ' ') {
            continue; // skip spaces
        }

        priceStr += raw_html[i];
    }
	offer.price = std::stoi(priceStr);
    raw_html.erase(0, pos + 5);

    return offer;
}
namespace parse {
std::vector<Offer> parseAllegroLokalnie(std::string url) {
    std::vector<Offer> offers;
    std::string raw_html = getSite(url);

    // getting the number of sites
    int pos = raw_html.find("ml-pagination__count");
    if(pos == std::string::npos) {
        LOG("shiiiiiiit allegro captcha/sth wrong with the site syntax");
        return offers; // No offers found
	}
    std::string sitesCountStr;
    for (int i = pos + 24; i < raw_html.size(); i++) {
        if (raw_html[i] == '<') {
            break;
        }
        sitesCountStr += raw_html[i];
    }
    int sitesCount = std::stoi(sitesCountStr);


    for (int i = 1; i <= sitesCount; i++) {
        std::string pageUrl = url + "&page=" + std::to_string(i);

        if (i != 1) { // we already got the first page 
            raw_html = getSite(pageUrl);
        }

        while (true) {
            Offer offer = parseOneOfferAllegroLokalnie(raw_html);
            if (!offer.valid) {
                break; // No more offers found
            }
            offers.push_back(offer);
        }
    }

    return offers;
}

// l-card - start of the offer
// css-1tqlkj0 - link
// css-gl6djm - photo
// css-1g61gc2 - title
// css-uj7mm0 - price
static Offer parseOneOfferOlx(std::string& raw_html) {
    Offer offer;

    int pos = raw_html.find("data-testid=\"l-card\" data-visually-ready-trigger-element=\"true\"");
    if (pos == std::string::npos) {
        offer.valid = false;
        return offer; // No offer found
    }
    raw_html.erase(0, pos); // clean useless html data

    pos = raw_html.find("class=\"css-1tqlkj0\"");

    for (size_t i = pos + 26; i < raw_html.size(); i++) {
        if (raw_html[i] == '"') {
            break;
        }

        offer.link += raw_html[i];
    }
    offer.link = "https://olx.pl" + offer.link;
    pos = raw_html.find("https://ireland.apollo.olxcdn.com");
    for (size_t i = pos; i < raw_html.size(); i++) {
        if (raw_html[i] == ' ') {
            break;
        }

        offer.imageUrl += raw_html[i];
    }
    if(offer.imageUrl.empty()) {
		offer.imageUrl = "https://play-lh.googleusercontent.com/IZbR5N9NRi4JZmiBkGsp7pUQikm8cQMZtnC2RN1e7xhU3u3-cObSYUSquVoqgeuRQw=w240-h480-rw "; // default image if not found
	}
    offer.imageUrl.pop_back();

    pos = raw_html.find("<h4 class=\"css-1g61gc2\">");
    for (size_t i = pos + 24; i < raw_html.size(); i++) {
        if (raw_html.substr(i, 5) == "</h4>") {
            break;
        }

        offer.title += raw_html[i];
    }

    pos = raw_html.find("class=\"css-uj7mm0\">");
    std::string priceStr = "";
    for (size_t i = pos + 19; i < raw_html.size(); i++) {
        if (raw_html.substr(i, 6) == "<style" || raw_html.substr(i, 5) == "<span") {
            break;
        }

        if (raw_html[i] == ' ') {
            continue; // skip spaces
        }

        priceStr += raw_html[i];
    }

    priceStr.pop_back();
    priceStr.pop_back();
    priceStr.pop_back(); // remove zl suffix

    offer.price = std::stoi(priceStr);
    raw_html.erase(0, pos + 10);

    return offer;
}


std::vector<Offer> parseOlx(std::string url) {
    std::vector<Offer> offers;
    std::string raw_html;

	int sitesCount = 3; // 3 should be enough since we are looking at the latest offers 
    // and its rly hard to find how much pages there are

    for (int i = 1; i <= sitesCount; i++) {
        std::string pageUrl = url + "&page=" + std::to_string(i);
        raw_html = getSite(pageUrl);

        while (true) {
            Offer offer = parseOneOfferOlx(raw_html);
            if (!offer.valid) {
                break; // No more offers found
            }
            offers.push_back(offer);
        }
    }

    return offers;
}

bool filterOffer(std::string title, std::vector<std::string>& positivePrompts, std::vector<std::string>& negativePrompts) {
    std::transform(title.begin(), title.end(), title.begin(),
        [](unsigned char c) { return std::tolower(c); });

    for(auto& prompt : positivePrompts) {
        if (title.find(prompt) == std::string::npos) {
            return false; // Positive prompt not found
        }
	}

    for (auto& prompt : negativePrompts) {
        if (title.find(prompt) != std::string::npos) {
            return false; // Negative prompt found
        }
    }

    return true;
}
}