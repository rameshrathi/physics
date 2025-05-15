//
// Created by ramesh on 15/05/25.
//

#pragma once

#include <vector>
#include <boost/json.hpp>
#include <boost/json/value.hpp>

#include "WSClient.h"

namespace stock {

    namespace  json = boost::json;

    using PriceLevel = std::pair<double, double>;

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

        [[nodiscard]]
        const std::string & getSymbol() const { return symbol; }

        [[nodiscard]]
        const std::string & getTimestamp() const { return timestamp; }

        [[nodiscard]]
        const std::vector<PriceLevel> & getBids() const { return bids; }

        [[nodiscard]]
        const std::vector<PriceLevel> & getAsks() const { return asks; }

        Orderbook()
            : Orderbook("", "", {}, {})
            {}

        Orderbook(std::string symbol, std::string timestamp, std::vector<stock::PriceLevel> bids, std::vector<stock::PriceLevel> asks)
            : symbol(std::move(symbol)), timestamp(std::move(timestamp)), bids(std::move(bids)), asks(std::move(asks))
            {}

        Orderbook(const Orderbook& other) {
            symbol = other.symbol;
            timestamp = other.timestamp;
            bids = other.bids;
            asks = other.asks;
        }

        Orderbook& operator = (const Orderbook& other) {
            symbol = other.symbol;
            timestamp = other.timestamp;
            bids = other.bids;
            asks = other.asks;
            return *this;
        }

    private:
        std::string symbol;
        std::string timestamp;
        std::vector<PriceLevel> bids;
        std::vector<PriceLevel> asks;
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
            std::vector<PriceLevel> bids;
            std::vector<PriceLevel> asks;
            std::string symbol;
            std::string timestamp;

            json::value parsed = json::parse(msg);
            symbol = parsed.at("symbol").as_string();
            timestamp = parsed.at("timestamp").as_string();
            json::array asks_data = parsed.at("asks").as_array();
            for (auto& obj : asks_data) {
                if (obj.is_array()) {
                    auto arr = obj.as_array();
                    asks.emplace_back(200.0, 2.0);
                }
            }
            json::array bids_data = parsed.at("bids").as_array();
            for (json::value& obj : bids_data) {
                json::array & arr = obj.as_array();
                bids.emplace_back(300.0, 3.0);
            }
            return Orderbook{symbol, timestamp, bids, asks};
        }
    };

} // stock

