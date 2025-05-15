//
// Created by ramesh on 15/05/25.
//

#pragma once

#include <string>
#include <imgui.h>

#include "CalculationEngine.h"

// UI Module
class UIModule {
public:
    UIModule(const Config& cfg, const CalculationEngine& calc) : config(cfg), calc(calc) {}

    void render() {
        ImGui::Begin("Trade Simulator");
        ImGui::Text("Input Parameters");
        ImGui::InputText("Exchange", const_cast<char*>(config.exchange.c_str()), 10);
        ImGui::InputText("Spot Asset", const_cast<char*>(config.spotAsset.c_str()), 20);
        ImGui::Text("Order Type: %s", config.orderType.c_str());
        ImGui::InputDouble("Quantity (USD)", &config.quantity);
        ImGui::InputDouble("Volatility", &config.volatility);
        ImGui::InputInt("Fee Tier", &config.feeTier);

        ImGui::Text("Output Parameters");
        ImGui::Text("Slippage: %.4f", calc.getSlippage());
        ImGui::Text("Fees: %.4f", calc.getFees());
        ImGui::Text("Market Impact: %.4f", calc.getMarketImpact());
        ImGui::Text("Net Cost: %.4f", calc.getNetCost());
        ImGui::Text("Maker/Taker Prop: %.4f", calc.getMakerTaker());
        ImGui::Text("Latency: %.2f us", calc.getLatency());
        ImGui::End();
    }
private:
    const Config& config;
    const CalculationEngine & calc;
};

