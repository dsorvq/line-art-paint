#include <iostream>
#include <array>

#include "matrix.hpp"
#include "matrix_utils.hpp"
#include "graph_utils.hpp"

void print_partition(std::vector<int>& partition, int height, int width) {
    auto size = width * height;
    for (int i = 0; i != size; ++i) {
        if (i % width == 0) {
            std::cout << '\n';
        }
        std::cout << partition[i] << ' ' ;
        if (partition[i] < 10)
            std::cout << ' ';
        if (partition[i] < 100)
            std::cout << ' ';
    }
    std::cout << '\n';
}

#include <chrono>
class TimerGuard {
private:
    std::ostream& out_;
    std::string message_;

    std::chrono::time_point<std::chrono::high_resolution_clock> start;

    public:
    TimerGuard(std::string dead_message = "", std::ostream& out = std::cout) 
        : out_(out)
        , message_(dead_message)
        , start(std::chrono::high_resolution_clock::now())
    { }

    void now(std::string message = "") {
        std::chrono::duration<double> diff =
        std::chrono::high_resolution_clock::now() - start;
        out_ << message << ' ' << diff.count() << '\n';
    }

    ~TimerGuard() {
        std::chrono::duration<double> diff =
        std::chrono::high_resolution_clock::now() - start;
        out_ << message_ << ' ' << diff.count() << '\n';
    }
};

int main() {
    auto img = imread("dino/drawing.png");
    auto gray = to_gray_gamma(img, 1/2.0);

    auto scribbles = imread("dino/scribbles.png");

    int width = gray.width();
    int height = gray.height();
    auto size = gray.size();

    // 23 works best so far
    constexpr int terminal_capacity = 23;

    TimerGuard tg{"total time:"};
    Dinic<int> graph(size + 2);
    tg.now("graph created:");
    add_img_edges(graph, gray);
    tg.now("normal pixels added:");
    add_scribble_edges(graph, scribbles, terminal_capacity, {14, 138, 8});
    tg.now("scribbles added:");
   
    auto flow = graph.max_flow(size, size + 1);
    tg.now("flow calculated:");
    std::cout << "flow=" << flow << '\n';
    auto partition = graph.partition(size);

    // if image has 3 channels
    std::array<float, 3> color = {0, 1, 0};
    auto* img_p = img.pt();
    auto img_size = img.size();
    for (int i = 0; i < img_size; i += 3) {
        if (!partition[i/3])
            continue;
        img_p[i] *= color[0]; 
        img_p[i+1] *= color[1]; 
        img_p[i+2] *= color[2]; 
    }
    imwrite("result.png", img); 

    return 0;
}

