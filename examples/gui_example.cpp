#include <iostream>
#include <array>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "matrix.hpp"
#include "matrix_utils.hpp"

// Global variables
/*
GLFWwindow* window;
unsigned int drawing_id;
Matrix<unsigned char> drawing;
*/

class Painter {
public:
    Painter() { }

    void init(const char* filename) {
        drawing_ = imread(filename); // double copy
        scribbles_.reset(drawing_.height(), drawing_.width(), 4, 0);

        scribbles_make_border();
        
        update_drawing();
        update_scribbles();
    }
    ~Painter() = default;

    auto drawing_id() -> unsigned int {
        return drawing_id_; 
    }
    auto scribbles_id() -> unsigned int {
        return scribbles_id_; 
    }

    auto width() {return drawing_.width();}
    auto height() {return drawing_.height();}
    auto drawing_pt() {return drawing_.pt();}

    void draw_circle(int x, int y, int diameter, std::array<int, 3> color) {
        int radius = diameter / 2;
      
        auto w = scribbles_.width();
        auto h = scribbles_.height();
        auto* pt = scribbles_.pt();
        for (int i = x - radius; i <= x + radius; ++i) {
            for (int j = y - radius; j <= y + radius; ++j) {
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

    void update_scribbles() {
        update_texture(scribbles_, scribbles_id_);
    }
    void update_drawing() {
        update_texture(drawing_, drawing_id_);
    }

private:
    void update_texture(Matrix<unsigned char>& m, unsigned int& texture_id) {
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        if (m.channels() == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m.width(), m.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, m.pt());
        }
        else {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m.width(), m.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m.pt());
        }
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
    Matrix<unsigned char> drawing_;
    Matrix<unsigned char> scribbles_;
    unsigned int drawing_id_;
    unsigned int scribbles_id_;
};

class GUI {
public:
    GUI() = delete;
    GUI(const char* filename) {
        if (!glfwInit()) {
            std::cerr << "glfw wasn't initialized\n";
            return;
        }
        window_ = glfwCreateWindow(800, 600, "Image Display", NULL, NULL);
        if (!window_) {
            glfwTerminate();
            std::cerr << "glfw window initialized\n";
            return;
        }
        glfwMakeContextCurrent(window_);
        ImGui_init();

        painter_.init(filename);
        is_ready_ = true;
    }

    ~GUI() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window_);
        glfwTerminate();
    }

    bool run() {
        if (!is_ready_)
            return false;

        while (!glfwWindowShouldClose(window_)) {
            glfwPollEvents();
            render_frame();
        }
        return is_ready_;
    }

    void render_frame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos({0, 0});
        ImGui::Begin("Tools", NULL, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
        float col;
        ImGui::ColorPicker3("color picker", &col);
        static bool show_scribbles = true;
        ImGui::Checkbox("Show scribbles", &show_scribbles);
        ImGui::End();

        ImGui::ShowDemoWindow();

        ImGui::Begin("Image", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse);
        ImGui::Image((void*)(intptr_t)painter_.drawing_id(), ImVec2(painter_.width(), painter_.height()));

        ImVec2 mousePos = ImGui::GetMousePos();
        ImVec2 imagePos = ImGui::GetItemRectMin(); // Top-left corner of the image within the ImGui window
        ImVec2 rmpos = ImVec2(mousePos.x - imagePos.x, mousePos.y - imagePos.y);

        // Check if the cursor is within the bounds of the image
        if (rmpos.x >= 0 && rmpos.y >= 0 &&
            rmpos.x < painter_.width() && rmpos.y < painter_.height()) 
        {
            /*
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                painter_.draw_circle(rmpos.x, rmpos.y, 16, {255, 0, 0});
                painter_.update_scribbles();
            } */
            std::cout << "x:" << rmpos.x << " y:" << rmpos.y << '\n'; 
        }
        ImGui::End();

        ImGui::Begin("Scribbles", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse);
        ImGui::Image((void*)(intptr_t)painter_.scribbles_id(), ImVec2(painter_.width(), painter_.height()));
        ImGui::End();

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
    Painter painter_ {};    
    bool is_ready_ {};
};

/*
void ImGuiInit() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
}

void render_frame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos({0, 0});
    ImGui::Begin("Tools", NULL, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
    float col;
    ImGui::ColorPicker3("color picker", &col);
    static bool show_scribbles = true;
    ImGui::Checkbox("Show scribbles", &show_scribbles);
    ImGui::End();

    ImGui::Begin("Image", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse);
    ImGui::Image((void*)(intptr_t)drawing_id, ImVec2(drawing.width(), drawing.height()));

    ImVec2 mousePos = ImGui::GetMousePos();
    ImVec2 imagePos = ImGui::GetItemRectMin(); // Top-left corner of the image within the ImGui window
    ImVec2 rmpos = ImVec2(mousePos.x - imagePos.x, mousePos.y - imagePos.y);

    // Check if the cursor is within the bounds of the image
    if (rmpos.x >= 0 && rmpos.y >= 0 &&
        rmpos.x < drawing.width() && rmpos.y < drawing.height()) 
    {
        std::cout << "x:" << rmpos.x << " y:" << rmpos.y << '\n'; 
    }
    ImGui::End();

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap buffers
    glfwSwapBuffers(window);
}
*/

int main(int argc, char* argv[]) {
    /*
    if (!glfwInit()) {
        return -1;
    }
    window = glfwCreateWindow(800, 600, "Image Display", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    drawing = imread(argv[1]);
    // Generate OpenGL texture for the drawing
    glGenTextures(1, &drawing_id);
    glBindTexture(GL_TEXTURE_2D, drawing_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, drawing.width(), drawing.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, drawing.pt());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    ImGuiInit();

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        render_frame();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    */
    GUI gui{argv[1]};
    gui.run();   

    return 0;
}
