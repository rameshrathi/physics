#pragma once

/**
 * Enhanced TradingView UI using ImGui with improved performance and UX
 * Using Apple's built-in OpenGL framework instead of GLAD on macOS.
 */

#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <iomanip>
#include <sstream>

#include <OpenGL/gl3.h>            // Apple's OpenGL core profile headers
#include <GLFW/glfw3.h>            // Window & input
#include <imgui.h>                 // Core ImGui
#include <imgui_internal.h>        // For advanced ImGui features
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "CalculationEngine.h"

// Price level type for orderbook display
using PriceLevel = std::pair<double, double>;

// Function to format numbers with commas for better readability
template<typename T>
std::string formatWithCommas(T value, int precision = 2) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    std::string numStr = ss.str();

    const int insertPosition = numStr.find('.') != std::string::npos
                        ? numStr.find('.')
                        : numStr.length();

    for (int i = insertPosition - 3; i > 0; i -= 3) {
        numStr.insert(i, ",");
    }
    return numStr;
}

class TradingView {
public:
    TradingView(const Config& cfg, const CalculationEngine& calc)
        : config(cfg), calc(calc), window(nullptr),
          theme(ThemeType::Dark),
          lastFrameTime(0.0),
          frameCount(0),
          frameTimer(0.0),
          fps(0.0f),
          windowWidth(1280),
          windowHeight(720) {}

    // Initialize GLFW and ImGui (no loader step)
    void setup() {
        if (!glfwInit()) throw std::runtime_error("Failed to initialize GLFW");

        // Request OpenGL 3.3 core profile
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        // Enable MSAA for smoother edges
        glfwWindowHint(GLFW_SAMPLES, 4);

        // Create a window with default size
        window = glfwCreateWindow(windowWidth, windowHeight, "Professional Trading Simulator", nullptr, nullptr);
        if (!window) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Enable vsync

        // window resize callback
        glfwSetWindowUserPointer(window, this);
        glfwSetWindowSizeCallback(window, [](GLFWwindow* w, int width, int height) {
            auto view = static_cast<TradingView*>(glfwGetWindowUserPointer(w));
            view->handleResize(width, height);
        });

        // Initialize ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        // Enable docking and viewports for better UX
        // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        // Load fonts
        // io.Fonts->AddFontFromFileTTF("fonts/Roboto-Regular.ttf", 16.0f);
        //
        // // Add additional fonts with specific ranges for different sizes
        // fontSmall = io.Fonts->AddFontFromFileTTF("fonts/Roboto-Regular.ttf", 14.0f);
        // fontMedium = io.Fonts->AddFontFromFileTTF("fonts/Roboto-Medium.ttf", 18.0f);
        // fontLarge = io.Fonts->AddFontFromFileTTF("fonts/Roboto-Bold.ttf", 22.0f);
        // fontMono = io.Fonts->AddFontFromFileTTF("fonts/RobotoMono-Regular.ttf", 15.0f);

        // Try to load custom fonts if available, otherwise use default
        ImFont* defaultFont = io.Fonts->AddFontDefault();
        fontSmall = defaultFont;
        fontMedium = defaultFont;
        fontLarge = defaultFont;
        fontMono = defaultFont;

        // Optionally try loading custom fonts
        const char* fontPath = "fonts/Roboto-Regular.ttf";
        if (FILE* file = fopen(fontPath, "r")) {
            fclose(file);
            fontSmall = io.Fonts->AddFontFromFileTTF(fontPath, 14.0f);
            fontMedium = io.Fonts->AddFontFromFileTTF(fontPath, 18.0f);
            fontLarge = io.Fonts->AddFontFromFileTTF("fonts/Roboto-Bold.ttf", 22.0f);
            fontMono = io.Fonts->AddFontFromFileTTF("fonts/RobotoMono-Regular.ttf", 15.0f);
        }

        // default style and theme
        applyTheme(theme);

        // Setup platform/renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330 core");

        // Setup window icon (if available)
        // initWindowIcon();

        // Initialize frame time measurement
        lastFrameTime = glfwGetTime();
    }

    // Apply a theme to ImGui
    enum class ThemeType { Dark, Light, Trading };

    void applyTheme(ThemeType newTheme) {
        theme = newTheme;
        auto& style = ImGui::GetStyle();

        // Reset style
        ImGui::StyleColorsDark();

        // Common style settings
        style.WindowRounding = 5.0f;
        style.FrameRounding = 4.0f;
        style.GrabRounding = 3.0f;
        style.ScrollbarSize = 14.0f;
        style.ScrollbarRounding = 9.0f;
        style.TabRounding = 4.0f;
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
        style.ItemSpacing = ImVec2(8.0f, 6.0f);
        style.ItemInnerSpacing = ImVec2(6.0f, 6.0f);
        style.FramePadding = ImVec2(6.0f, 4.0f);
        style.WindowPadding = ImVec2(10.0f, 10.0f);

        // Apply specific theme colors
        if (theme == ThemeType::Dark) {
            // Professional dark theme for trading
            style.Colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.11f, 0.14f, 0.94f);
            style.Colors[ImGuiCol_Border] = ImVec4(0.06f, 0.06f, 0.12f, 0.5f);
            style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.21f, 0.54f);
            style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.20f, 0.25f, 0.54f);
            style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.24f, 0.29f, 0.54f);
            style.Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.16f, 1.00f);
            style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.16f, 0.24f, 1.00f);
            style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.20f, 1.00f);
            style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.16f, 0.16f, 0.21f, 0.54f);
            style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.24f, 0.24f, 0.29f, 0.54f);
            style.Colors[ImGuiCol_Header] = ImVec4(0.16f, 0.16f, 0.23f, 0.54f);
            style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.22f, 0.22f, 0.35f, 0.54f);
            style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.26f, 0.39f, 0.54f);
            style.Colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.30f, 0.54f);
            style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.24f, 0.38f, 0.54f);
            style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.28f, 0.28f, 0.42f, 0.54f);
            style.Colors[ImGuiCol_Tab] = ImVec4(0.17f, 0.17f, 0.27f, 0.86f);
            style.Colors[ImGuiCol_TabHovered] = ImVec4(0.24f, 0.24f, 0.38f, 0.80f);
            style.Colors[ImGuiCol_TabActive] = ImVec4(0.28f, 0.28f, 0.42f, 1.00f);
        }
        else if (theme == ThemeType::Light) {
            ImGui::StyleColorsLight();
            style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
            style.Colors[ImGuiCol_TitleBg] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
            style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.78f, 0.78f, 0.80f, 1.00f);
        }
        else if (theme == ThemeType::Trading) {
            // Trading-specific theme
            ImGui::StyleColorsDark();
            style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.12f, 0.16f, 0.94f);
            style.Colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.16f, 0.94f);
            style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.13f, 0.17f, 0.23f, 0.94f);
            style.Colors[ImGuiCol_Button] = ImVec4(0.11f, 0.16f, 0.28f, 1.00f);
            style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.15f, 0.22f, 0.36f, 1.00f);
            style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.19f, 0.26f, 0.40f, 1.00f);
            style.Colors[ImGuiCol_Header] = ImVec4(0.11f, 0.16f, 0.28f, 1.00f);
            style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.15f, 0.22f, 0.36f, 1.00f);
            style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.19f, 0.26f, 0.40f, 1.00f);
            style.Colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.16f, 0.28f, 0.86f);
            style.Colors[ImGuiCol_TabHovered] = ImVec4(0.15f, 0.22f, 0.36f, 0.80f);
            style.Colors[ImGuiCol_TabActive] = ImVec4(0.19f, 0.26f, 0.40f, 1.00f);
        }

        // platform dependent style
        if (ImGui::GetIO().ConfigFlags) { // ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }
    }

    // Window resize handler
    void handleResize(const int width, const int height) {
        windowWidth = width;
        windowHeight = height;
    }

    // For performance tracking
    void updatePerformanceMetrics() {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;

        frameCount++;
        frameTimer += deltaTime;

        // Update fps-counter once per second
        if (frameTimer >= 1.0) {
            fps = static_cast<float>(frameCount) / frameTimer;
            frameCount = 0;
            frameTimer = 0.0;
        }
    }

    // Render a single frame with improved visualization
    void render() {
        updatePerformanceMetrics();
        glfwPollEvents();

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // main dock-space
        setupDockspace();

        // Render different panels
        renderMenuBar();
        renderMainWindow();
        renderStatusBar();

        // Rendering
        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        ImGuiIO& io = ImGui::GetIO();
    /*
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    */
        glfwSwapBuffers(window);
    }
    // Simplify the setupDockspace() function to avoid errors:
    void setupDockspace() {
        // Basic implementation without docking
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);

        // Add this one as well => ImGuiWindowFlags_NoDocking
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar
                                     | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
                                     | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
                                     | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGui::Begin("DockSpace Demo", nullptr, window_flags);
        ImGui::PopStyleVar(3);

        // Comment out the actual dockspace call
        // ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
        // ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
        ImGui::End();
    }
/*
    // Setup dockspace for better panel organization
    void setupDockspace() {
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each other.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen) {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGui::Begin("DockSpace Demo", nullptr, window_flags);

        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }
    }
*/
    // Render the menu bar
    void renderMenuBar() {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New Simulation", "Ctrl+N")) {}
                if (ImGui::MenuItem("Open Config", "Ctrl+O")) {}
                if (ImGui::MenuItem("Save Config", "Ctrl+S")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Export Results", "Ctrl+E")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "Alt+F4")) { glfwSetWindowShouldClose(window, true); }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View")) {
                if (ImGui::MenuItem("Dark Theme", nullptr, theme == ThemeType::Dark)) { applyTheme(ThemeType::Dark); }
                if (ImGui::MenuItem("Light Theme", nullptr, theme == ThemeType::Light)) { applyTheme(ThemeType::Light); }
                if (ImGui::MenuItem("Trading Theme", nullptr, theme == ThemeType::Trading)) { applyTheme(ThemeType::Trading); }
                ImGui::Separator();
                if (ImGui::MenuItem("Performance Metrics", nullptr, showPerformanceWindow)) { showPerformanceWindow = !showPerformanceWindow; }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Tools")) {
                if (ImGui::MenuItem("Simulation Settings")) {}
                if (ImGui::MenuItem("Market Data Settings")) {}
                if (ImGui::MenuItem("Advanced Configuration")) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help")) {
                if (ImGui::MenuItem("Documentation")) {}
                if (ImGui::MenuItem("About")) {}
                ImGui::EndMenu();
            }

            // Right-aligned items
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 200);
            ImGui::Text("%.1f FPS", fps);

            ImGui::EndMenuBar();
        }
    }

    // Render status bar at the bottom of the window
    void renderStatusBar() {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
                                       ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoNavFocus;
        float height = ImGui::GetFrameHeight();

        if (ImGui::BeginViewportSideBar("##StatusBar", nullptr, ImGuiDir_Down, height, window_flags)) {
            if (ImGui::BeginMenuBar()) {
                std::string exchangeInfo = "Exchange: " + config.exchange + " | Asset: " + config.spotAsset;
                ImGui::Text("%s", exchangeInfo.c_str());

                // Status indicators - right aligned
                ImGui::SameLine(ImGui::GetWindowWidth() - 250);
                ImGui::Text("Status: ");
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Connected");

                ImGui::SameLine(ImGui::GetWindowWidth() - 100);
                ImGui::Text("%.1f FPS", fps);

                ImGui::EndMenuBar();
            }
            ImGui::End();
        }
    }

    // Main window containing all trading panels
    void renderMainWindow() {
        renderInputPanel();
        renderOrderBookPanel();
        renderOutputPanel();
        renderChartPanel();

        if (showPerformanceWindow) {
            renderPerformanceWindow();
        }
    }

    // Input parameters panel
    void renderInputPanel() {
        ImGui::Begin("Input Parameters");

        // Use larger font for section headers
        ImGui::PushFont(fontMedium);
        ImGui::Text("Trading Configuration");
        ImGui::PopFont();

        ImGui::Separator();

        // Create two columns for better layout
        ImGui::Columns(2, "input_columns", false);

        static char exch[64];
        static char asset[64];
        static char type[64];
        strncpy(exch, config.exchange.c_str(), sizeof(exch));
        strncpy(asset, config.spotAsset.c_str(), sizeof(asset));
        strncpy(type, config.orderType.c_str(), sizeof(type));

        // Left column
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Exchange");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(-1);
        if (ImGui::InputText("##Exchange", exch, sizeof(exch))) {
            const_cast<Config&>(config).exchange = exch;
        }

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Spot Asset");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(-1);
        if (ImGui::InputText("##SpotAsset", asset, sizeof(asset))) {
            const_cast<Config&>(config).spotAsset = asset;
        }

        ImGui::NextColumn();

        // Right column
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Order Type");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(-1);
        if (ImGui::InputText("##OrderType", type, sizeof(type))) {
            const_cast<Config&>(config).orderType = type;
        }

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Fee Tier");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(-1);
        ImGui::InputInt("##FeeTier", &const_cast<Config&>(config).feeTier);

        ImGui::Columns(1);

        ImGui::Separator();

        // Quantity slider with formatted display
        ImGui::PushFont(fontMedium);
        ImGui::Text("Order Parameters");
        ImGui::PopFont();

        double& quantity = const_cast<Config&>(config).quantity;
        double& volatility = const_cast<Config&>(config).volatility;

        ImGui::Text("Quantity (USD)");
        ImGui::SameLine(ImGui::GetWindowWidth() * 0.7f);
        ImGui::Text("$%s", formatWithCommas(quantity).c_str());
        ImGui::SliderScalar("##Quantity", ImGuiDataType_Double, &quantity, &quantityMin, &quantityMax, "");

        ImGui::Text("Volatility");
        ImGui::SameLine(ImGui::GetWindowWidth() * 0.7f);
        ImGui::Text("%.2f%%", volatility * 100.0);
        ImGui::SliderScalar("##Volatility", ImGuiDataType_Double, &volatility, &volatilityMin, &volatilityMax, "");

        ImGui::Separator();

        // Action buttons
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 300) * 0.5f);
        if (ImGui::Button("Execute Order", ImVec2(150, 40))) {
            // Order execution logic
        }

        ImGui::SameLine();

        if (ImGui::Button("Reset Parameters", ImVec2(150, 40))) {
            // Reset parameters logic
        }

        ImGui::End();
    }

    // Order book visualization
    void renderOrderBookPanel() const {
        ImGui::Begin("Order Book");

        ImGui::PushFont(fontMedium);
        ImGui::Text("Market Depth: %s/%s", config.spotAsset.c_str(), "USD");
        ImGui::PopFont();

        ImGui::Separator();

        const std::vector<PriceLevel> & bids = calc.getBids();
        const std::vector<PriceLevel> & asks = calc.getAsks();

        // Determine the maximum volume and price for scaling
        double maxVolume = 0.0;
        double medianPrice = 0.0;

        for (const PriceLevel& bid : bids) maxVolume = std::max(maxVolume, bid.second);
        for (const PriceLevel& ask : asks) maxVolume = std::max(maxVolume, ask.second);

        if (!bids.empty() && !asks.empty()) {
            medianPrice = (bids[0].first + asks[0].first) / 2.0;
        }

        // Create header with column labels
        ImGui::Columns(4, "orderbook_cols");
        ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() * 0.15f);  // Volume
        ImGui::SetColumnWidth(1, ImGui::GetWindowWidth() * 0.35f);  // Bid Bars
        ImGui::SetColumnWidth(2, ImGui::GetWindowWidth() * 0.35f);  // Ask Bars
        ImGui::SetColumnWidth(3, ImGui::GetWindowWidth() * 0.15f);  // Volume

        ImGui::Text("Volume");
        ImGui::NextColumn();
        ImGui::Text("Bid");
        ImGui::NextColumn();
        ImGui::Text("Ask");
        ImGui::NextColumn();
        ImGui::Text("Volume");
        ImGui::NextColumn();

        ImGui::Separator();

        // Set up font for price/volume display
        ImGui::PushFont(fontMono);

        // Display the orderbook with bars representing volume
        size_t maxRows = std::max(bids.size(), asks.size());
        for (size_t i = 0; i < maxRows; ++i) {
            // Bid side (right-aligned)
            if (i < bids.size()) {
                // Volume
                ImGui::Text("%s", formatWithCommas(bids[i].second, 4).c_str());
                ImGui::NextColumn();

                // Calculate bar width based on volume relative to max
                float bidFraction = static_cast<float>(bids[i].second / maxVolume);
                ImVec2 cursor = ImGui::GetCursorScreenPos();
                ImVec2 barSize(ImGui::GetColumnWidth() * bidFraction, ImGui::GetFrameHeight());

                // Draw volume bar
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                draw_list->AddRectFilled(
                    ImVec2(cursor.x + ImGui::GetColumnWidth() - barSize.x, cursor.y),
                    ImVec2(cursor.x + ImGui::GetColumnWidth(), cursor.y + barSize.y),
                    ImColor(0.0f, 0.8f, 0.21f, 0.7f)
                );

                // Price text
                ImGui::Text("$%s", formatWithCommas(bids[i].first, 2).c_str());
                ImGui::NextColumn();
            } else {
                ImGui::Text(""); ImGui::NextColumn();
                ImGui::Text(""); ImGui::NextColumn();
            }

            // Ask side (left-aligned)
            if (i < asks.size()) {
                // Calculate bar width based on volume relative to max
                float askFraction = static_cast<float>(asks[i].second / maxVolume);
                ImVec2 cursor = ImGui::GetCursorScreenPos();
                ImVec2 barSize(ImGui::GetColumnWidth() * askFraction, ImGui::GetFrameHeight());

                // Draw volume bar
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                draw_list->AddRectFilled(
                    ImVec2(cursor.x, cursor.y),
                    ImVec2(cursor.x + barSize.x, cursor.y + barSize.y),
                    ImColor(0.92f, 0.23f, 0.23f, 0.7f)
                );

                // Price text
                ImGui::Text("$%s", formatWithCommas(asks[i].first, 2).c_str());
                ImGui::NextColumn();

                // Volume
                ImGui::Text("%s", formatWithCommas(asks[i].second, 4).c_str());
                ImGui::NextColumn();
            } else {
                ImGui::Text(""); ImGui::NextColumn();
                ImGui::Text(""); ImGui::NextColumn();
            }
        }

        ImGui::PopFont();
        ImGui::Columns(1);

        // Spread information
        if (!bids.empty() && !asks.empty()) {
            ImGui::Separator();
            double spread = asks[0].first - bids[0].first;
            double spreadPercent = (spread / medianPrice) * 100.0;

            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 200) / 2);
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Spread: $%s (%.2f%%)",
                formatWithCommas(spread, 2).c_str(), spreadPercent);
        }

        ImGui::End();
    }

    // Output parameters and results panel
    void renderOutputPanel() {
        ImGui::Begin("Output Parameters");

        ImGui::PushFont(fontMedium);
        ImGui::Text("Trade Execution Analytics");
        ImGui::PopFont();

        ImGui::Separator();

        // Create a table for better formatting
        static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
        if (ImGui::BeginTable("output_table", 2, flags)) {
            ImGui::TableSetupColumn("Metric", ImGuiTableColumnFlags_WidthFixed, ImGui::GetWindowWidth() * 0.6f);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, ImGui::GetWindowWidth() * 0.4f);
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted("Slippage");
            ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(
                calc.getSlippage() > 0.01 ? ImVec4(0.92f, 0.23f, 0.23f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
                "%.4f%%", calc.getSlippage() * 100.0);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted("Fees");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("$%s", formatWithCommas(calc.getFees(), 4).c_str());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted("Market Impact");
            ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(
                calc.getMarketImpact() > 0.02 ? ImVec4(0.92f, 0.23f, 0.23f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
                "%.4f%%", calc.getMarketImpact() * 100.0);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted("Net Cost");
            ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.21f, 1.0f),
                "$%s", formatWithCommas(calc.getNetCost(), 2).c_str());

            ImGui::EndTable();
        }

        ImGui::Separator();

        // Execution quality metric
        float executionQuality = calculateExecutionQuality();
        ImGui::Text("Execution Quality:");

        // Progress bar representing execution quality
        ImGui::ProgressBar(executionQuality, ImVec2(-1, 0), "");

        // Color-coded text based on quality
        ImVec4 qualityColor;
        const char* qualityText;

        if (executionQuality >= 0.8f) {
            qualityColor = ImVec4(0.0f, 0.8f, 0.21f, 1.0f);
            qualityText = "Excellent";
        } else if (executionQuality >= 0.6f) {
            qualityColor = ImVec4(0.56f, 0.83f, 0.26f, 1.0f);
            qualityText = "Good";
        } else if (executionQuality >= 0.4f) {
            qualityColor = ImVec4(1.0f, 0.8f, 0.0f, 1.0f);
            qualityText = "Average";
        } else if (executionQuality >= 0.2f) {
            qualityColor = ImVec4(1.0f, 0.4f, 0.0f, 1.0f);
            qualityText = "Poor";
        } else {
            qualityColor = ImVec4(0.92f, 0.23f, 0.23f, 1.0f);
            qualityText = "Very Poor";
        }

        ImGui::SameLine();
        ImGui::TextColored(qualityColor, "%s (%.0f%%)", qualityText, executionQuality * 100);

        ImGui::End();
    }

    // Simple chart panel showing price history
    void renderChartPanel() {
        ImGui::Begin("Price Chart");

        ImGui::PushFont(fontMedium);
        ImGui::Text("%s/USD Price History", config.spotAsset.c_str());
        ImGui::PopFont();

        ImGui::Separator();

        // Mock price history data - in real application, this would come from historical data
        static float priceHistory[100];
        static bool initialized = false;

        if (!initialized) {
            // Generate sample price data
            const float basePrice = static_cast<float>(calc.getBids().empty() ? 10000.0 : calc.getBids()[0].first);
            priceHistory[0] = basePrice;

            for (int i = 1; i < 100; i++) {
                // Simple random walk with trend
                float change = (((float)rand() / RAND_MAX) - 0.48f) * basePrice * 0.01f;
                priceHistory[i] = priceHistory[i-1] + change;
            }
            initialized = true;
        }

        // Find min/max for scaling
        float minPrice = priceHistory[0];
        float maxPrice = priceHistory[0];

        for (int i = 1; i < 100; i++) {
            minPrice = std::min(minPrice, priceHistory[i]);
            maxPrice = std::max(maxPrice, priceHistory[i]);
        }

        // Add 5% padding to min/max
        float padding = (maxPrice - minPrice) * 0.05f;
        minPrice -= padding;
        maxPrice += padding;

        // Plot area
        ImVec2 plotSize = ImVec2(ImGui::GetContentRegionAvail().x, 250);
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
        ImVec2 canvas_size = plotSize;

        // Draw frame
        draw_list->AddRect(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y),
                         ImColor(ImGui::GetStyle().Colors[ImGuiCol_Border]));

        // Draw grid lines
        const int yDivisions = 5;
        const float yStep = canvas_size.y / yDivisions;

        for (int i = 0; i <= yDivisions; i++) {
            float y = canvas_pos.y + i * yStep;
            float price = maxPrice - i * (maxPrice - minPrice) / yDivisions;

            // Grid line
            draw_list->AddLine(
                ImVec2(canvas_pos.x, y),
                ImVec2(canvas_pos.x + canvas_size.x, y),
                ImColor(0.5f, 0.5f, 0.5f, 0.25f)
            );

            // Price label
            char priceLabel[32];
            snprintf(priceLabel, sizeof(priceLabel), "$%.2f", price);
            draw_list->AddText(
                ImVec2(canvas_pos.x + 5, y - 10),
                ImColor(0.8f, 0.8f, 0.8f, 1.0f),
                priceLabel
            );
        }

        // Plot the price line
        const int xStep = static_cast<int>(canvas_size.x) / 99;
        for (int i = 0; i < 99; i++) {
            float t1 = (priceHistory[i] - minPrice) / (maxPrice - minPrice);
            float t2 = (priceHistory[i+1] - minPrice) / (maxPrice - minPrice);

            float y1 = canvas_pos.y + canvas_size.y - t1 * canvas_size.y;
            float y2 = canvas_pos.y + canvas_size.y - t2 * canvas_size.y;

            float x1 = canvas_pos.x + i * xStep;
            float x2 = canvas_pos.x + (i+1) * xStep;

            // Line from point to point with gradient color based on price change
            ImColor lineColor;
            if (priceHistory[i+1] >= priceHistory[i]) {
                lineColor = ImColor(0.0f, 0.8f, 0.21f, 1.0f); // Green for up
            } else {
                lineColor = ImColor(0.92f, 0.23f, 0.23f, 1.0f); // Red for down
            }

            draw_list->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), lineColor, 2.0f);
        }

        // Reserve space for the plot
        ImGui::InvisibleButton("canvas", canvas_size);

        // Time labels below the chart
        ImGui::Text("Time (last 24 hours)");

        ImGui::End();
    }

    // Performance metrics window
    void renderPerformanceWindow() {
        ImGui::Begin("Performance Metrics", &showPerformanceWindow);

        ImGui::Text("FPS: %.1f", fps);
        ImGui::Text("Frame Time: %.3f ms", 1000.0f / fps);
        ImGui::Text("Window Size: %d x %d", windowWidth, windowHeight);

        // Frame time history graph
        static float frameTimes[100] = {};
        static int frameTimeIndex = 0;

        frameTimes[frameTimeIndex] = 1000.0f / std::max(fps, 0.1f);
        frameTimeIndex = (frameTimeIndex + 1) % IM_ARRAYSIZE(frameTimes);

        float maxFrameTime = 0.0f;
        for (float frameTime : frameTimes) {
            maxFrameTime = std::max(maxFrameTime, frameTime);
        }

        ImGui::PlotLines("Frame Times", frameTimes, IM_ARRAYSIZE(frameTimes),
                        frameTimeIndex, nullptr, 0.0f, maxFrameTime, ImVec2(0, 80));

        ImGui::Separator();

        // Memory usage statistics (mock data for demo)
        static float memoryUsage = 128.0f; // MB
        memoryUsage += (static_cast<float>(rand()) / RAND_MAX) * 0.5f - 0.25f;
        memoryUsage = std::max(128.0f, std::min(256.0f, memoryUsage));

        ImGui::Text("Memory Usage: %.1f MB", memoryUsage);
        ImGui::ProgressBar(memoryUsage / 512.0f, ImVec2(-1, 0), "");

        ImGui::End();
    }

    // Calculate a simple synthetic execution quality metric
    float calculateExecutionQuality() const {
        const float slippagePenalty = static_cast<float>(std::min(calc.getSlippage() * 20.0, 0.5));
        const float feesPenalty = static_cast<float>(std::min(calc.getFees() / config.quantity, 0.2));
        const float impactPenalty = static_cast<float>(std::min(calc.getMarketImpact() * 10.0, 0.3));

        return std::max(0.0f, 1.0f - slippagePenalty - feesPenalty - impactPenalty);
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

    // Theme and UI styling
    ThemeType theme;
    ImFont* fontSmall = nullptr;
    ImFont* fontMedium = nullptr;
    ImFont* fontLarge = nullptr;
    ImFont* fontMono = nullptr;

    // Performance tracking
    double lastFrameTime;
    int frameCount;
    double frameTimer;
    float fps;

    // Window dimensions
    int windowWidth;
    int windowHeight;

    // Display options
    bool showPerformanceWindow = false;

    // Slider ranges
    double quantityMin = 100.0;
    double quantityMax = 100000.0;
    double volatilityMin = 0.01;
    double volatilityMax = 0.5;
};