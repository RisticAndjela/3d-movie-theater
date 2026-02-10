#include <vector>
#include "Seat3D.h"

std::vector<Seat3D> generateSeatsGrid(int rows, int cols, float spacingX, float spacingZ) {
    std::vector<Seat3D> seats;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            glm::vec3 pos(c * spacingX, r * 0.5f, -r * spacingZ); // -r*spacingZ da ide unazad
            seats.emplace_back(pos);
        }
    }
    return seats;
}
