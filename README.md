<h1 align="center">🛒 FastBuyer</h1>

<p align="center">
  <b>A scraping tool for hunting the best deals on Allegro Lokalnie and OLX.</b>
</p>

## 💡 About The Project

FastBuyer is a lightweight but powerful C++ scraper designed to help you snipe the best offers before anyone else does. It continuously monitors listings on **Allegro Lokalnie** and **OLX**, immediately forwarding matched items straight to your Telegram via a dedicated bot. 

Whether you're looking for personal steals or tracking market prices, FastBuyer keeps you ahead of the competition.

## ✨ Key Features

- **Instant Telegram Alerts:** Get notified about new deals directly on your phone or desktop.
- **Smart Scraping:** Scans all available pages on Allegro Lokalnie and the top 3 pages of OLX.
- **Interactive Bot Integration:** Communicate with your scraper on the go. Type `!help` in your Telegram chat to see available commands.
- **Customizable Targets:** Filter exactly what you want using `positivePrompt` and `negativePrompt` rules.
- **Granular Notifications:** Set a global Telegram `chat_id` or define specific IDs for individual queries.

## 🚀 Getting Started

Depending on your operating system, you can either download a pre-built release or compile the project from source.

### Windows (Quick Start)

The easiest way to run FastBuyer on Windows is by using the pre-compiled executable.

1. Head over to the [Releases](../../releases) page.
2. Download the latest `FastBuyer.exe` and the `config.json` template.
3. Place both files in the same folder.
4. Edit `config.json` with your credentials (see Configuration below).
5. Run `FastBuyer.exe`.

### Linux & macOS (Build from Source)

For Linux and macOS users, FastBuyer needs to be compiled manually. 

**Prerequisites:**
- A compiler with **C++20** support (e.g., GCC 10+, Clang 10+, Apple Clang 13+).
- [nlohmann/json](https://github.com/nlohmann/json) - JSON for Modern C++
- [libcpr/cpr](https://github.com/libcpr/cpr) - C++ Requests

**Build Steps:**

1. Clone the repository to your local machine:
   ```bash
   git clone [https://github.com/vende11s/FastBuyer.git](https://github.com/vende11s/FastBuyer.git)
   cd FastBuyer
   ```
2. Ensure you have the required dependencies linked properly in your build environment.
3. Compile the source code using your preferred build system (e.g., CMake/Make).
4. Create a `config.json` file in the same directory as your compiled executable.

## ⚙️ Configuration

Before running the application, you must set up your scraping preferences and Telegram credentials. 

Use the provided `config.json` as a baseline and fill in your Telegram Bot token, chat IDs, and desired search queries.

> ⚠️ **Important:** When setting up your queries, ensure that all `positivePrompt` and `negativePrompt` values are strictly written in **lowercase**.

## 💻 Usage

Once the configuration is ready, simply launch the application. FastBuyer will begin scraping the platforms based on your JSON setup and push live notifications directly to your Telegram channel or private chat.
