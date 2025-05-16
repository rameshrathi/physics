#pragma once

/**
 * Enhanced TradingView UI using ImGui with improved performance and UX
 * Using Apple's built-in OpenGL framework instead of GLAD on macOS.
 * Layout optimized for chart and orderbook display with responsive design.
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

using PriceLevel = std::pair<double, double>;

template<typename T>
std::string formatWithCommas(T value, int precision = 2) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    std::string numStr = ss.str();
    const int insertPos = numStr.find('.') != std::string::npos ? numStr.find('.') : numStr.length();
    for (int i = insertPos - 3; i > 0; i -= 3)
        numStr.insert(i, ",");
    return numStr;
}

class MarketView {
public:
    enum class ThemeType { Dark, Light, Trading };

    MarketView(const Config& cfg, const CalculationEngine& calc)
        : config(cfg), calc(calc), window(nullptr), theme(ThemeType::Dark),
          fontSmall(nullptr), fontMedium(nullptr), fontLarge(nullptr), fontMono(nullptr),
          lastFrameTime(0.0), frameCount(0), frameTimer(0.0), fps(0.0f),
          windowWidth(1280), windowHeight(720), initialized(false) {}

    void setup() {
        if (!glfwInit()) throw std::runtime_error("Failed to initialize GLFW");
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_SAMPLES, 4);

        window = glfwCreateWindow(windowWidth, windowHeight, "Professional Trading Simulator", nullptr, nullptr);
        if (!window) {
            glfwTerminate(); throw std::runtime_error("Failed to create GLFW window");
        }
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);
        glfwSetWindowUserPointer(window, this);
        glfwSetWindowSizeCallback(window, [](GLFWwindow* w, int w_, int h_) {
            auto self = static_cast<MarketView*>(glfwGetWindowUserPointer(w));
            if (self && self->initialized) self->handleResize(w_, h_);
        });

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

        io.FontDefault = io.Fonts->AddFontDefault();
        const char* path = "fonts/Roboto-Regular.ttf";
        if (FILE* f = fopen(path, "r")) {
            float baseSize = 18.0f;
            fclose(f);
            fontSmall = io.Fonts->AddFontFromFileTTF(path, baseSize);
            fontMedium = io.Fonts->AddFontFromFileTTF(path, baseSize * 1.25f);
            fontLarge = io.Fonts->AddFontFromFileTTF("fonts/Roboto-Bold.ttf", baseSize * 1.5f);
            fontMono = io.Fonts->AddFontFromFileTTF("fonts/RobotoMono-Regular.ttf", baseSize * 1.1f);
        } else {
            fontSmall = fontMedium = fontLarge = fontMono = io.FontDefault;
            io.FontGlobalScale = 1.5f;
        }
        io.FontDefault = fontMedium;

        applyTheme(theme);
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330 core");
        lastFrameTime = glfwGetTime();
        initialized = true;
    }

    void applyTheme(ThemeType t) {
        theme = t;
        ImGuiIO& io = ImGui::GetIO();
        auto& style = ImGui::GetStyle();
        ImGui::StyleColorsDark();
        style.WindowRounding = (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) ? 0.0f : 5.0f;
        style.FrameRounding = 4.0f;
        style.ScrollbarSize = 18.0f;
        style.ItemSpacing = ImVec2(10, 8);
        style.FramePadding = ImVec2(8, 6);
    }

    void handleResize(int w, int h) {
        windowWidth = w; windowHeight = h;
    }

    void updatePerformanceMetrics() {
        double now = glfwGetTime(), dt = now - lastFrameTime;
        lastFrameTime = now;
        frameCount++; frameTimer += dt;
        if (frameTimer >= 1.0) { fps = frameCount/frameTimer; frameCount = 0; frameTimer = 0; }
    }

    void render() {
        if (!initialized || !window) return;
        ImGuiIO& io = ImGui::GetIO();  // Ensure io is available
        updatePerformanceMetrics();
        if (!initialized || !window) return;
        updatePerformanceMetrics();
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame(); ImGui_ImplGlfw_NewFrame(); ImGui::NewFrame();

        auto viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
        ImGui::Begin("MainWindow", nullptr,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        float chartH = windowHeight*0.3f;
        float bookH = windowHeight*0.7f - ImGui::GetFrameHeight();
        renderChartPanel(ImVec2(windowWidth, chartH));
        ImGui::SetCursorPosY(chartH);
        renderOrderBookPanel(ImVec2(windowWidth, bookH));

        ImGui::End(); ImGui::PopStyleVar();
        renderStatusBar();

        ImGui::Render();
        int dw,dh; glfwGetFramebufferSize(window,&dw,&dh);
        glViewport(0,0,dw,dh); glClearColor(0.08f,0.08f,0.10f,1.0f); glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            auto backup = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows(); ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup);
        }
        glfwSwapBuffers(window);
    }

    void renderStatusBar() {
        float h = ImGui::GetFrameHeight()*1.5f;
        if (ImGui::BeginViewportSideBar("##StatusBar", nullptr, ImGuiDir_Down, h,
            ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_MenuBar)) {
            if (ImGui::BeginMenuBar()) {
                ImGui::PushFont(fontMedium);
                ImGui::Text("Exchange: %s | Asset: %s", config.exchange.c_str(), config.spotAsset.c_str());
                ImGui::SameLine(ImGui::GetWindowWidth()-120);
                ImGui::Text("%.1f FPS", fps);
                ImGui::PopFont(); ImGui::EndMenuBar();
            }
            ImGui::End();
        }
    }

    void renderChartPanel(const ImVec2& size) {
        ImGuiIO& io = ImGui::GetIO();
        auto& style = ImGui::GetStyle();  // For border color reference

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2(10,10));
        ImGui::BeginChild("ChartPanel",size,false,ImGuiWindowFlags_NoScrollbar);
        ImGui::PushFont(fontLarge);
        ImGui::Text("%s/USD Price History",config.spotAsset.c_str());
        ImGui::PopFont(); ImGui::Separator();
        static float hist[100]; static bool init=false;
        if(!init){ float base=calc.getBids().empty()?10000:calc.getBids()[0].first; hist[0]=base;
            for(int i=1;i<100;i++){ hist[i]=hist[i-1]+(((float)rand()/RAND_MAX-0.48f)*base*0.01f);} init=true; }
        float minP=hist[0],maxP=hist[0]; for(int i=1;i<100;i++){minP=std::min(minP,hist[i]);maxP=std::max(maxP,hist[i]);}
        float pad=(maxP-minP)*0.05f; minP-=pad; maxP+=pad;
        ImVec2 avail=ImGui::GetContentRegionAvail(); ImVec2 canvas(avail.x,avail.y-ImGui::GetTextLineHeightWithSpacing());
        ImVec2 pos=ImGui::GetCursorScreenPos(); auto draw=ImGui::GetWindowDrawList();
        draw->AddRect(pos, ImVec2(pos.x + canvas.x, pos.y + canvas.y), ImColor(style.Colors[ImGuiCol_Border]));
        const int div=4; for(int i=0;i<=div;i++){float y=pos.y+i*canvas.y/div; char buf[32]; sprintf(buf,"$%.2f",maxP - i*(maxP-minP)/div);
            draw->AddLine(ImVec2(pos.x,y),ImVec2(pos.x+canvas.x,y),ImColor(0.5f,0.5f,0.5f,0.25f));
            draw->AddText(ImGui::GetFont(),ImGui::GetFontSize(),ImVec2(pos.x+5,y-10),ImColor(0.8f,0.8f,0.8f,1.0f),buf);
        }
        for(int i=0;i<99;i++){ float t1=(hist[i]-minP)/(maxP-minP),t2=(hist[i+1]-minP)/(maxP-minP);
            ImVec2 p1(pos.x+i*canvas.x/99,pos.y+canvas.y - t1*canvas.y);
            ImVec2 p2(pos.x+(i+1)*canvas.x/99,pos.y+canvas.y - t2*canvas.y);
            draw->AddLine(p1,p2, hist[i+1]>=hist[i]?ImColor(0.0f,0.8f,0.21f):ImColor(0.92f,0.23f,0.23f),2.5f);
        }
        ImGui::InvisibleButton("canvas",canvas);
        ImGui::PopStyleVar(); ImGui::EndChild();
    }

    void renderOrderBookPanel(const ImVec2& size) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2(10,10));
        ImGui::BeginChild("OrderBookPanel",size,false,ImGuiWindowFlags_NoScrollbar);
        ImGui::Text("Market Depth: %s/USD",config.spotAsset.c_str()); ImGui::Separator();
        auto bids=calc.getBids(),asks=calc.getAsks(); double maxV=0; for(auto &b: bids) maxV=std::max(maxV,b.second); for(auto&a:asks) maxV=std::max(maxV,a.second);
        ImGui::Columns(4,nullptr,false);
        ImGui::SetColumnWidth(0,size.x*0.15f);ImGui::Text("Volume");ImGui::NextColumn();ImGui::Text("Bid");ImGui::NextColumn();ImGui::Text("Ask");ImGui::NextColumn();ImGui::Text("Volume");ImGui::NextColumn();ImGui::Separator();
        for(size_t i=0;i<std::min(std::max(bids.size(),asks.size()),(size_t)15);++i){
            if(i<bids.size()){
                ImGui::Text("%s",formatWithCommas(bids[i].second,4).c_str());ImGui::NextColumn();
                float frac=bids[i].second/maxV;auto cur=ImGui::GetCursorScreenPos();ImVec2 bar(size.x*0.35f*frac,ImGui::GetFrameHeight()*1.2f);
                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(cur.x+size.x*0.35f-bar.x,cur.y),ImVec2(cur.x+size.x*0.35f,cur.y+bar.y),ImColor(0.0f,0.8f,0.21f,0.7f));
                ImGui::Text("$%s",formatWithCommas(bids[i].first,2).c_str());ImGui::NextColumn();
            } else { ImGui::NextColumn();ImGui::NextColumn(); }
            if(i<asks.size()){
                auto cur=ImGui::GetCursorScreenPos(); float frac=asks[i].second/maxV; ImVec2 bar(size.x*0.35f*frac,ImGui::GetFrameHeight()*1.2f);
                ImGui::GetWindowDrawList()->AddRectFilled(cur,ImVec2(cur.x+bar.x,cur.y+bar.y),ImColor(0.92f,0.23f,0.23f,0.7f));
                ImGui::Text("$%s",formatWithCommas(asks[i].first,2).c_str());ImGui::NextColumn();
                ImGui::Text("%s",formatWithCommas(asks[i].second,4).c_str());ImGui::NextColumn();
            } else { ImGui::NextColumn();ImGui::NextColumn(); }
        }
        ImGui::Columns(1);
        if(!bids.empty() && !asks.empty()){
            double spread=asks[0].first-bids[0].first,mid=(asks[0].first+bids[0].first)/2;
            ImGui::TextColored(ImVec4(1,0.8f,0,1),"Spread: $%s (%.2f%%)",formatWithCommas(spread,2).c_str(),spread/mid*100);
        }
        ImGui::PopStyleVar(); ImGui::EndChild();
    }

    bool shouldClose() const { return window && glfwWindowShouldClose(window); }
    void cleanup() {
        if(!initialized) return;
        ImGui_ImplOpenGL3_Shutdown(); ImGui_ImplGlfw_Shutdown(); ImGui::DestroyContext();
        if(window) glfwDestroyWindow(window); glfwTerminate(); initialized=false;
    }

private:
    const Config& config;
    const CalculationEngine& calc;
    GLFWwindow* window;
    ThemeType theme;
    ImFont *fontSmall, *fontMedium, *fontLarge, *fontMono;
    double lastFrameTime;
    int frameCount;
    double frameTimer;
    float fps;
    int windowWidth, windowHeight;
    bool initialized;
};
