#pragma once

/**
 * OrderView UI using ImGui
 * Splits the window into two horizontal panels (50:50)
 * Left panel: order input fields (Exchange, Spot Asset, Order Type, Quantity, Volatility, Fee Tier)
 * Right panel: output parameters display (Expected Slippage, Fees, Market Impact, Net Cost, Maker/Taker Proportion, Internal Latency)
 * UI scaled by 30% to increase font and widget sizes.
 */
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <stdexcept>
#include <cstdio>
#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

class OrderView {
public:
    OrderView() = default;
    ~OrderView() { cleanup(); }

    // Initialize GLFW and ImGui
    void setup() {
        if (!glfwInit()) throw std::runtime_error("Failed to initialize GLFW");
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        window = glfwCreateWindow(1024, 1024, "OrderView", nullptr, nullptr);
        if (!window) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
        io.FontGlobalScale = 1.3f; // Increase all UI elements by 30%

        // Load roboto font
        const char* robotoPath = "./fonts/Roboto-Regular.ttf";
        ImFont* fontRegular = io.Fonts->AddFontFromFileTTF(robotoPath, 18.0f * io.FontGlobalScale);
        // const char* robotoBoldPath = "./fonts/Roboto-Bold.ttf";

        // ImFont* fontBold = io.Fonts->AddFontFromFileTTF(robotoBoldPath, 18.0f * io.FontGlobalScale);
        if (fontRegular)
            io.FontDefault = fontRegular;
        else
            io.FontDefault = io.Fonts->Fonts[0];

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330 core");
    }

    // Main rendering loop
    void renderLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            render();

            ImGui::Render();
            int w, h;
            glfwGetFramebufferSize(window, &w, &h);
            glViewport(0, 0, w, h);
            glClearColor(0.11f, 0.12f, 0.14f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                GLFWwindow* backup = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup);
            }

            glfwSwapBuffers(window);
        }
    }

    // Cleanup ImGui and GLFW
    void cleanup() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        if (window) glfwDestroyWindow(window);
        glfwTerminate();
    }

private:
    GLFWwindow* window = nullptr;

    // Input fields
    char exchange[64] = "OKX";
    char spotAsset[64] = "";
    char orderType[64] = "market";
    double quantity = 100.0;
    double volatility = 0.1;
    int feeTier = 0;

    // Output values (placeholders for future calculation logic)
    struct Output {
        float expectedSlippage = 0.0f;
        float expectedFees = 0.0f;
        float expectedImpact = 0.0f;
        float netCost = 0.0f;
        float makerTaker = 0.0f;
        float latency = 0.0f;
    } output;

    // Renders the two 50:50 panels
    void render() {
        ImGuiViewport* vp = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(vp->WorkPos);
        ImGui::SetNextWindowSize(vp->WorkSize);
        ImGui::Begin("OrderView Main", nullptr,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        // Two-column layout
        ImGui::Columns(2, nullptr, false);
        float halfW = vp->WorkSize.x * 0.5f;
        ImGui::SetColumnWidth(0, halfW);

        // --- Left Panel: Inputs ---
        ImGui::BeginChild("LeftPanel", ImVec2(halfW, 0), true);
        ImGui::Text("Order Input");
        ImGui::Separator();
        ImGui::InputText("Exchange", exchange, sizeof(exchange));
        ImGui::InputText("Spot Asset", spotAsset, sizeof(spotAsset));
        ImGui::InputText("Order Type", orderType, sizeof(orderType));
        ImGui::InputDouble("Quantity (USD)", &quantity, 1.0, 10.0, "%.2f");
        ImGui::InputDouble("Volatility", &volatility, 0.01, 0.1, "%.2f");
        ImGui::InputInt("Fee Tier", &feeTier);
        ImGui::EndChild();

        ImGui::NextColumn();

        // --- Right Panel: Outputs ---
        ImGui::BeginChild("RightPanel", ImVec2(halfW, 0), true);
        ImGui::Text("Output Parameters");
        ImGui::Separator();
        ImGui::Text("Expected Slippage: %.2f", output.expectedSlippage);
        ImGui::Text("Expected Fees: %.2f", output.expectedFees);
        ImGui::Text("Expected Market Impact: %.2f", output.expectedImpact);
        ImGui::Text("Net Cost: %.2f", output.netCost);
        ImGui::Text("Maker/Taker Proportion: %.2f", output.makerTaker);
        ImGui::Text("Internal Latency: %.2f ms", output.latency);
        ImGui::EndChild();

        ImGui::Columns(1);
        ImGui::End();
    }
};
