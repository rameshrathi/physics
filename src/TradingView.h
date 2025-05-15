#pragma once

/**
 * Using Apple's built-in OpenGL framework instead of GLAD on macOS.
 * No explicit loader needed: just include <OpenGL/gl3.h> and link to OpenGL.framework.
 */

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

#include <OpenGL/gl3.h>            // Apple's OpenGL core profile headers
#include <GLFW/glfw3.h>            // Window & input
#include <imgui.h>                 // Core ImGui
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "CalculationEngine.h"

// Price level type for orderbook display
using PriceLevel = std::pair<double, double>;

// UI Module encapsulates all ImGui and GLFW setup/teardown
class TradingView {
public:
    TradingView(const Config& cfg, const CalculationEngine& calc)
        : config(cfg), calc(calc), window(nullptr) {}

    // Initialize GLFW and ImGui (no loader step)
    void setup() {
        if (!glfwInit()) throw std::runtime_error("Failed to initialize GLFW");
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(1280, 720, "Trade Simulator", nullptr, nullptr);
        if (!window) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        // No GLAD; use system OpenGL.framework

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330 core");
    }

    // Render a single frame
    void render() {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Trade Simulator");

        // Input pane
        if (ImGui::CollapsingHeader("Input Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
            static char exch[64];  static char asset[64];  static char type[64];
            strncpy(exch, config.exchange.c_str(), sizeof(exch));
            strncpy(asset, config.spotAsset.c_str(), sizeof(asset));
            strncpy(type, config.orderType.c_str(), sizeof(type));
            ImGui::InputText("Exchange", exch, sizeof(exch));
            ImGui::InputText("Spot Asset", asset, sizeof(asset));
            ImGui::InputText("Order Type", type, sizeof(type));
            ImGui::InputDouble("Quantity (USD)", &const_cast<Config&>(config).quantity);
            ImGui::InputDouble("Volatility", &const_cast<Config&>(config).volatility);
            ImGui::InputInt("Fee Tier", &const_cast<Config&>(config).feeTier);
        }

        ImGui::Separator();

        // Orderbook pane
        if (ImGui::CollapsingHeader("Order Book (Bids / Asks)", ImGuiTreeNodeFlags_DefaultOpen)) {
            const auto bids = calc.getBids();   // vector<PriceLevel>
            const auto asks = calc.getAsks();   // vector<PriceLevel>

            ImGui::BeginChild("OrderBookChild", ImVec2(0, 300), true);
            ImGui::Columns(2, "orderbook_cols");
            ImGui::Separator();
            ImGui::Text("Bids"); ImGui::NextColumn();
            ImGui::Text("Asks"); ImGui::NextColumn();
            ImGui::Separator();
            size_t maxRows = std::max(bids.size(), asks.size());
            for (size_t i = 0; i < maxRows; ++i) {
                if (i < bids.size()) ImGui::Text("%.2f : %.4f", bids[i].first, bids[i].second);
                else ImGui::Text(" ");
                ImGui::NextColumn();

                if (i < asks.size()) ImGui::Text("%.2f : %.4f", asks[i].first, asks[i].second);
                else ImGui::Text(" ");
                ImGui::NextColumn();
            }
            ImGui::Columns(1);
            ImGui::EndChild();
        }

        ImGui::Separator();

        // Output pane
        if (ImGui::CollapsingHeader("Output Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Slippage: %.4f", calc.getSlippage());
            ImGui::Text("Fees: %.4f", calc.getFees());
            ImGui::Text("Market Impact: %.4f", calc.getMarketImpact());
            ImGui::Text("Net Cost: %.4f", calc.getNetCost());
        }

        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Check if the window should close
    bool shouldClose() const {
        return window && glfwWindowShouldClose(window);
    }

    // Cleanup ImGui and GLFW
    void teardown() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        if (window) {
            glfwDestroyWindow(window);
            window = nullptr;
        }
        glfwTerminate();
    }

private:
    const Config& config;
    const CalculationEngine& calc;
    GLFWwindow* window;
};

// Example main usage:
// int main() {
//     Config cfg;
//     CalculationEngine calc(cfg);
//     UIModule ui(cfg, calc);
//     ui.setup();
//     while (!ui.shouldClose()) ui.render();
//     ui.teardown();
//     return 0;
// }
