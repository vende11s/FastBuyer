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
can specify to use another than global chat_id for single query, as it is in example config.json<br>
there is also communication with FastBuyer via telegram, just write !help in chat to see available commands<br>

### TODO
- [ ] add sent.json (when launching FastBuyer its always sending things i already saw)
- [ ] add scrapping all pages for olx
- [ ] keeping data for analysis of the market (what offer was sold when and for how much)
