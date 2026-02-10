#pragma once
#include <glm/ext/vector_float3.hpp>
#include "Seat.h"

enum class PersonState {
    EnteringToRow,     // penje se stepenicama do reda
    MovingToSeat,     // ide levo-desno do kolone
    Seated,           // u kocki sedista
    LeavingRow,       // ustaje i ide nazad do stepenica
    LeavingDown,      // silazi ka izlazu
    Gone
};

struct Person {
    glm::vec3 position;
    glm::vec3 entrancePos;
    Seat* targetSeat;        
    PersonState state;
};
