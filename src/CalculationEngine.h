//
// Created by ramesh on 15/05/25.
//
#pragma once

#include <string>
#include <chrono>
#include <utility>

#include "OrderBook.h"

struct Config {
    std::string exchange = "OKX";
    std::string spotAsset = "BTC-USDT-SWAP";
    std::string orderType = "market";
    double quantity = 100.0;
    double volatility = 0.02;
    int feeTier = 0;
};

// Calculation Engine
class CalculationEngine {
public:
    explicit CalculationEngine(Config  cfg) : config(std::move(cfg)) {}

    void processTick(const stock::Orderbook& ob) {
        book = ob;
        auto start = std::chrono::high_resolution_clock::now();
        slippage = computeSlippage(ob);
        fees = computeFees();
        marketImpact = computeMarketImpact(ob);
        netCost = slippage + fees + marketImpact;
        makerTakerProp = computeMakerTaker();
        auto end = std::chrono::high_resolution_clock::now();
        // latency = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    }

    [[nodiscard]] double getSlippage() const { return slippage; }
    [[nodiscard]] double getFees() const { return fees; }
    [[nodiscard]] double getMarketImpact() const { return marketImpact; }
    [[nodiscard]] double getNetCost() const { return netCost; }
    [[nodiscard]] double getMaker() const { return makerTakerProp; }

    [[nodiscard]]
    double computeSlippage(const stock::Orderbook& ob) const {
        // Simplified linear regression for slippage
        double depth = ob.getBids()[0].second + ob.getAsks()[0].second;
        return config.quantity / depth * 0.01; // Example model
    }

    [[nodiscard]]
    double computeFees() const {
        // Rule-based fee model (e.g., 0.1% taker fee)
        return config.quantity * 0.001;
    }

    [[nodiscard]]
    double computeMarketImpact(const stock::Orderbook& ob) const {
        // Simplified Almgren-Chriss model
        double spread = ob.getAsks()[0].first - ob.getBids()[0].first;
        return config.quantity * spread * config.volatility;
    }

    [[nodiscard]]
    double computeMakerTaker() const {
        // Simplified logistic regression
        double spread = book.getAsks()[0].first - book.getBids()[0].first;
        return 1.0 / (1.0 + std::exp(-spread)); // Sigmoid function
    }

    [[nodiscard]]
    const std::vector<stock::PriceLevel> & getBids() const { return book.getBids(); }

    [[nodiscard]]
    const std::vector<stock::PriceLevel> & getAsks() const { return book.getAsks(); }

private:
    Config config;
    stock::Orderbook book;
    double slippage = 0.0, fees = 0.0, marketImpact = 0.0, netCost = 0.0, makerTakerProp = 0.0;
    double latency = 0.0; // Microseconds
};