#include <iostream>
#include <chrono>

#include "matrix.hpp"
#include "matrix_utils.hpp"
#include "painter.hpp"

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

int main(int argc, char* argv[]) {
    TimerGuard tg{"total time:"};

    std::string drawing_image_path = argv[1];
    std::string scribbles_image_path = argv[2];

    Painter painter(drawing_image_path.data());
    if (painter.empty()) {
        std::cout << "Failed to load the drawing image." << std::endl;
        return 1;
    }

    auto scribbles = imread(scribbles_image_path.data());
    if (scribbles.empty()) {
        std::cout << "Failed to load the scribble image." << std::endl;
        return 1;
    }

    painter.paint(scribbles);

    painter.imwrite("result.png");

    return 0;
}

