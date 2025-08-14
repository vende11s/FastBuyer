// Copyright 2025 vende11s
#pragma once
#include <string>
#include <vector>
#include <map>

namespace OfferQuery {
struct Offer {
    bool valid = true;
    int price = 0;
    std::string title;
    std::string link;
    std::string imageUrl;
};

struct Query {
    std::string title;
    std::string url;
    std::string type; // "allegro" or "olx"
	std::string chat_id;
    int pricePoint = 0;
    std::vector<std::string> negativePrompts;
    std::vector<std::string> positivePrompts;
};
}