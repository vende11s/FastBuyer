// Copyright 2025 vende11s
#pragma once
#include <string>
#include <vector>

#include "OfferQuery.hpp"

using namespace OfferQuery; 

namespace parse {
std::vector<Offer> parseAllegroLokalnie(std::string url);
std::vector<Offer> parseOlx(std::string url);
bool filterOffer(std::string title, std::vector<std::string>& positivePrompts, std::vector<std::string>& negativePrompts);
}