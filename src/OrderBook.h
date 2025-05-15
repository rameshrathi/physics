//
// Created by ramesh on 15/05/25.
//

#pragma once

#include <vector>
#include <boost/json.hpp>
#include <boost/json/value.hpp>

#include "WSClient.h"

namespace  json = boost::json;

namespace stock {
    // Example stock ticker
    struct AmountAndPrice {
        double amount{};
        double price{};
        AmountAndPrice() = default;
        AmountAndPrice(const json::string & str1, const json::string & str2)
            : amount(0), price(0)
            {}
    };

    // Orderbook Processor
    class Orderbook {
    public:
        void updateOrderbook(Orderbook && orderbook) {
            std::unique_lock lock(mutex_);
            bids.clear();
            asks.clear();
            symbol = orderbook.symbol;
            timestamp = orderbook.timestamp;
            bids = orderbook.bids;
            asks = orderbook.asks;
            orderbook.clear();
        }

        std::string getSymbol() const { return symbol; }
        std::string getTimestamp() const { return timestamp; }

        std::vector<AmountAndPrice> getBids() const { return bids; }
        std::vector<AmountAndPrice> getAsks() const { return asks; }

        Orderbook()
            : Orderbook("", "", {}, {})
            {}

        Orderbook(std::string symbol, std::string timestamp, std::vector<stock::AmountAndPrice> bids, std::vector<stock::AmountAndPrice> asks)
            : symbol(std::move(symbol)), timestamp(std::move(timestamp)), bids(std::move(bids)), asks(std::move(asks))
            {}

    private:
        std::string symbol;
        std::string timestamp;
        std::vector<AmountAndPrice> bids;
        std::vector<AmountAndPrice> asks;
        std::mutex mutex_;

        void clear() {
            symbol.clear();
            timestamp.clear();
            bids.clear();
            asks.clear();
        }
    };

    // Example stock ticker parser
    struct OrderbookParser final : IParser<Orderbook> {
        Orderbook operator() (const std::string& msg) const override {
            // Data members
            std::vector<AmountAndPrice> bids;
            std::vector<AmountAndPrice> asks;
            std::string symbol;
            std::string timestamp;

            json::value parsed = json::parse(msg);
            symbol = parsed.at("symbol").as_string();
            timestamp = parsed.at("timestamp").as_string();
            json::array asks_data = parsed.at("asks").as_array();
            for (auto& obj : asks_data) {
                if (obj.is_array()) {
                    auto arr = obj.as_array();
                    asks.emplace_back(AmountAndPrice{arr[0].as_string(), arr[1].as_string()});
                }
            }
            json::array bids_data = parsed.at("bids").as_array();
            for (json::value& obj : bids_data) {
                json::array & arr = obj.as_array();
                bids.emplace_back(AmountAndPrice{ arr[0].as_string(), arr[1].as_string() });
            }
            return Orderbook{symbol, timestamp, bids, asks};
        }
    };

} // stock

