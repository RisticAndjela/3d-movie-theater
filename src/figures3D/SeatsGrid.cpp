#include <vector>
#include "Seat3D.h"

std::vector<Seat3D> generateSeatsGrid(int rows, int cols, float spacingX, float spacingZ, float stepHeight) {
    std::vector<Seat3D> seats;
    float startX = -((cols - 1) * spacingX) / 2.0f;

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            glm::vec3 pos(startX + c * spacingX, r * stepHeight, -r * spacingZ);
            seats.emplace_back(pos);
        }
    }
    return seats;
}
