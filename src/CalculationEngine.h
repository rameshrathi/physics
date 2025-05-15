//
// Created by ramesh on 15/05/25.
//

// #pragma once


// // Calculation Engine
// class CalculationEngine {
// public:
//     CalculationEngine(const Config& cfg) : config(cfg) {}
//
//     void processTick(const OrderbookProcessor& ob) {
//         auto start = std::chrono::high_resolution_clock::now();
//         slippage = computeSlippage(ob);
//         fees = computeFees();
//         marketImpact = computeMarketImpact(ob);
//         netCost = slippage + fees + marketImpact;
//         makerTakerProp = computeMakerTaker(ob);
//         auto end = std::chrono::high_resolution_clock::now();
//         latency = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
//     }
//
//     double getSlippage() const { return slippage; }
//     double getFees() const { return fees; }
//     double getMarketImpact() const { return marketImpact; }
//     double getNetCost() const { return netCost; }
//     double getMaker() { return makerTakerProp; }
//
//     double computeSlippage(const OrderbookProcessor& ob) {
//         // Simplified linear regression for slippage
//         double depth = ob.getBids()[0].second + ob.getAsks()[0].second;
//         return config.quantity / depth * 0.01; // Example model
//     }
//
//     double computeFees() {
//         // Rule-based fee model (e.g., 0.1% taker fee)
//         return config.quantity * 0.001;
//     }
//
//     double computeMarketImpact(const OrderbookProcessor& ob) {
//         // Simplified Almgren-Chriss model
//         double spread = ob.getAsks()[0].first - ob.getBids()[0].first;
//         return config.quantity * spread * config.volatility;
//     }
//
//     double computeMakerTaker(const OrderbookProcessor& ob) {
//         // Simplified logistic regression
//         double spread = ob.getAsks()[0].first - ob.getBids()[0].first;
//         return 1.0 / (1.0 + std::exp(-spread)); // Sigmoid function
//     }
//
// private:
//     Config config;
//     double slippage = 0.0, fees = 0.0, marketImpact = 0.0, netCost = 0.0, makerTakerProp = 0.0;
//     double latency = 0.0; // Microseconds
// };
//
