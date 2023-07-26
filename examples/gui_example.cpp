#include <iostream>
#include <array>
#include <unordered_set>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "matrix.hpp"
#include "dinic.hpp"
#include "graph_utils.hpp"
#include "matrix_utils.hpp"
#include "painter.hpp"

class PainterHandler : public Painter {
public:
    PainterHandler() = delete;
    PainterHandler(const char* filename) : Painter(filename) {
        width_ = drawing().width();
        height_ = drawing().height();

        scribbles_.reset(height_, width_, 4, 0);

        scribbles_make_border();   
    }

    void solve() {
        paint(scribbles_);
        update_drawing_texture();
    }

    void save_image() {
        imwrite("result.png");
    }

    void update_scribbles_texture() {
        update_texture(scribbles_, scribbles_id_);
    }
    void update_drawing_texture() {
        update_texture(drawing(), drawing_id_);
    }

    auto width() -> int {
        return width_;
    }
    auto height() -> int {
        return height_;
    }

    auto drawing_id() -> unsigned int {
        return drawing_id_; 
    }
    auto scribbles_id() -> unsigned int {
        return scribbles_id_; 
    }

    void draw_circle(int x, int y, int diameter, std::array<u_char, 3> color) {
        int radius = diameter / 2;
        
        int w = scribbles_.width();
        int h = scribbles_.height();
        auto* pt = scribbles_.pt();
        for (int i = std::max(x - radius, 0); i <= std::min(x + radius, w - 1); ++i) {
            for (int j = std::max(y - radius, 0); j <= std::min(y + radius, h - 1); ++j) {
                if ((i - x)*(i - x) + (j - y)*(j - y) <= radius*radius) {
                    int index = (j * w + i) * 4;
                   
                    pt[index] = color[0];
                    pt[index + 1] = color[1];
                    pt[index + 2] = color[2];
                    pt[index + 3] = 255;
                }
            }
        }
    }

private:
    void update_texture(const Matrix<unsigned char>& m, unsigned int& texture_id) {
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);

        GLenum format = (m.channels() == 3) ? GL_RGB : GL_RGBA;
        
        glTexImage2D(GL_TEXTURE_2D, 0, format, m.width(), m.height(), 0, format, GL_UNSIGNED_BYTE, m.pt());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    }

    void scribbles_make_border() {
        int h = scribbles_.height();
        int w = scribbles_.width();
        for (int i = 0; i != w; ++i) {
            scribbles_.set4(0, i, {255, 255, 255, 255});
            scribbles_.set4(h-1, i, {255, 255, 255, 255});
        }
        for (int i = 0; i != h; ++i) {
            scribbles_.set4(i, 0, {255, 255, 255, 255});
            scribbles_.set4(i, w-1, {255, 255, 255 ,255});
        } 
    }

private:
    int width_{};
    int height_{};
    unsigned int drawing_id_;

    unsigned int scribbles_id_;
    Matrix<unsigned char> scribbles_;
};

class GUI {
public:
    GUI() = delete;
    GUI(const char* filename) 
        : painter_(filename) 
    {
        if (!glfwInit()) {
            std::cerr << "glfw wasn't initialized\n";
            return;
        }
        window_ = glfwCreateWindow(1200, 1200, "Image Display", NULL, NULL);
        if (!window_) {
            glfwTerminate();
            std::cerr << "glfw window initialized\n";
            return;
        }
        glfwMakeContextCurrent(window_);
        ImGui_init();
        painter_.update_drawing_texture();
        painter_.update_scribbles_texture();
    }

    ~GUI() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window_);
        glfwTerminate();
    }

    bool run() {
        while (!glfwWindowShouldClose(window_)) {
            glfwPollEvents();
            render_frame();
        }
        return true;
    }

    void render_frame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos({float(painter_.width()+5), 0});
        ImGui::Begin("Tools", NULL, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
        static float col[3] = {255, 0, 0};
        ImGui::ColorPicker3("color picker", col);
        static bool show_scribbles = true;
        ImGui::Checkbox("Show scribbles", &show_scribbles);
        if (ImGui::Button("Save Image")) {
            painter_.save_image();
        }
        if (ImGui::Button("Paint!")) {
            painter_.solve();
        }
        ImGui::End();

        //ImGui::ShowDemoWindow();

        ImGui::SetNextWindowPos({0, 0});
        ImGui::SetNextWindowSize({0, 0});
        ImGui::Begin("Image", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse);
        ImGui::Image((void*)(intptr_t)painter_.drawing_id(), ImVec2(painter_.width(), painter_.height()));

        ImVec2 mousePos = ImGui::GetMousePos();
        ImVec2 imagePos = ImGui::GetItemRectMin(); // Top-left corner of the image within the ImGui window
        ImVec2 rmpos = ImVec2(mousePos.x - imagePos.x, mousePos.y - imagePos.y);

        // Check if the cursor is within the bounds of the image
        if (rmpos.x >= 0 && rmpos.y >= 0 &&
            rmpos.x < painter_.width() && rmpos.y < painter_.height()) 
        {
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                painter_.draw_circle(rmpos.x, rmpos.y, 16, 
                        {u_char(col[0] * 255), u_char(col[1] * 255), u_char(col[2] * 255)});
                painter_.update_scribbles_texture();
            }
            //std::cout << "x:" << rmpos.x << " y:" << rmpos.y << '\n'; 
        }
        ImGui::End();

        if (show_scribbles) {
            ImGui::SetNextWindowPos({0, 0});
            ImGui::SetNextWindowSize({0, 0});
            ImGui::Begin("Scribbles", NULL, ImGuiWindowFlags_NoMove |  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse);
            ImGui::Image((void*)(intptr_t)painter_.scribbles_id(), ImVec2(painter_.width(), painter_.height()));
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window_, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(window_);
    }
private:
    void ImGui_init() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(window_, true);
        ImGui_ImplOpenGL3_Init("#version 410");
    }

private:
    GLFWwindow* window_ {};
    PainterHandler painter_;
};

int main(int argc, char* argv[]) {
    GUI gui{argv[1]};
    gui.run();   

    return 0;
}
