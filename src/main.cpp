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

int main() {
    TimerGuard tg{"total time:"};

    Painter painter("skull/drawing.png");
    auto scribbles = imread("skull/scribbles.png");

    painter.paint(scribbles);

    painter.imwrite("result.png");

    return 0;
}

