#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "../model/Person.h"
#include "../service/SeatService.h"
#include "../figures3D/Seat3D.h"

class PersonManager {
public:
    PersonManager(SeatService& service, Seat3D& seat3d);

    void spawnPeople();
    void update(float dt);
    void startExit();
    bool allSeated() const;
    bool allGone() const;
    void renderSeats(unsigned int shaderProgram);
    void renderScene(unsigned int shaderProgram);
    glm::vec3 seatToWorld(int row, int col) const;

private:
    SeatService& seatService;
    Seat3D& seat3D; // referenca

    std::vector<Person> persons;
    glm::vec3 entrancePos{ 0.0f, 0.0f, 0.0f };
    float speed = 2.0f;

    bool moveTowards(glm::vec3& pos, const glm::vec3& target, float dt);
};
