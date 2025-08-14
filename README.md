# FastBuyer
simple scrapping tool for fast buying on Allegro Lokalnie and Olx
<br>sends data into telegram channel using telegram bot api
### Requirements
can compile on linux and windows<br>
required libs:
```
nlohmann-json
cpr
```
### Usage
put all the data in config.json file (u got example one in repo)<br>
use lowercase only for positivePrompt and negativePrompt<br>
program scrapps all pages for allegro lokalnie and only first 3 pages for olx (scrapping olx is way harder)<br>

### TODO
- [ ] add scrapping all pages for olx
- [ ] multithreaded command for asking if FastBuyer is up
- [ ] chatID specified for any offer
- [ ] analysis of market (what offer was sold when and for how much
- [ ] command to ask about offer popularity (how many sold in last 24h, 7 days, 30 days))
- [ ] help command