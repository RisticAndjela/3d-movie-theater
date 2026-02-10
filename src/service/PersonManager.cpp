#include "PersonManager.h"
#include <cstdlib>
#include <ctime>
#include "../model/Person.h"

PersonManager::PersonManager(SeatService& service, Seat3D& seat3d)
    : seatService(service), seat3D(seat3d) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

glm::vec3 PersonManager::seatToWorld(int row, int col) const {
    float spacingX = 1.0f;
    float spacingZ = 1.0f;
    float stepHeight = 0.3f;
    int cols = 10;

    float startX = -((cols - 1) * spacingX) / 2.0f;

    return {
        startX + col * spacingX,
        row * stepHeight + 0.5f,
        -row * spacingZ - 5.0f
    };
}

bool PersonManager::moveTowards(glm::vec3& pos, const glm::vec3& target, float dt) {
    glm::vec3 dir = target - pos;
    float dist = glm::length(dir);
    if (dist < 0.01f) {
        pos = target;
        return true;
    }
    pos += glm::normalize(dir) * speed * dt;
    return false;
}

void PersonManager::spawnPeople() {
    persons.clear();

    auto bought = seatService.getBoughtSeats();
    auto free = seatService.getFreeSeats();

    int padobranci = free.empty() ? 0 : std::rand() % (free.size() + 1);

    // ljudi za kupljena mesta
    for (Seat* s : bought) {
        persons.push_back({
            entrancePos,
            entrancePos,
            s,
            PersonState::EnteringToRow
            });
    }

    // padobranci
    for (int i = 0; i < padobranci; ++i) {
        Seat* s = free.back();
        free.pop_back();
        seatService.markBought(s);

        persons.push_back({
            entrancePos,
            entrancePos,
            s,
            PersonState::EnteringToRow
            });
    }
}

void PersonManager::update(float dt) {
    for (auto& p : persons) {
        glm::vec3 seatPos = seatToWorld(
            p.targetSeat->row,
            p.targetSeat->col
        );

        switch (p.state) {
        case PersonState::EnteringToRow: {
            glm::vec3 rowPos(p.entrancePos.x, seatPos.y, seatPos.z);
            if (moveTowards(p.position, rowPos, dt))
                p.state = PersonState::MovingToSeat;
            break;
        }

        case PersonState::MovingToSeat:
            if (moveTowards(p.position, seatPos, dt))
                p.state = PersonState::Seated;
            break;

        case PersonState::LeavingRow: {
            glm::vec3 back(p.entrancePos.x, seatPos.y, p.entrancePos.z);
            if (moveTowards(p.position, back, dt))
                p.state = PersonState::LeavingDown;
            break;
        }

        case PersonState::LeavingDown:
            if (moveTowards(p.position, p.entrancePos, dt))
                p.state = PersonState::Gone;
            break;

        default:
            break;
        }
    }
}

void PersonManager::startExit() {
    for (auto& p : persons)
        if (p.state == PersonState::Seated)
            p.state = PersonState::LeavingRow;
}

bool PersonManager::allSeated() const {
    for (const auto& p : persons)
        if (p.state != PersonState::Seated)
            return false;
    return true;
}

bool PersonManager::allGone() const {
    for (const auto& p : persons)
        if (p.state != PersonState::Gone)
            return false;
    return true;
}

void PersonManager::renderSeats(unsigned int shaderProgram) {
    for (int r = 0; r < seatService.getNumRows(); ++r) {
        for (int c = 0; c < seatService.getNumCols(); ++c) {
            Seat* s = seatService.getSeat(r, c);
            glm::vec3 color = seat3D.getSeatColor(s);
            glm::vec3 pos = seatToWorld(r, c);
            seat3D.renderCube(shaderProgram, pos, color, true, seat3D.size);
        }
    }
}
void PersonManager::renderScene(unsigned int shaderProgram) {
    renderSeats(shaderProgram); // prvo sedista

    for (auto& p : persons) {
        glm::vec3 color(0.2f, 0.8f, 0.2f); // ljudi
        seat3D.renderPerson(shaderProgram, p.position, color);
    }
}
