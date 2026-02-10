#pragma once
#include <vector>
#include "../model/Seat.h"

class SeatService {
private:
    std::vector<std::vector<Seat>> seats;
    int numRows;
    int numCols;

public:
    SeatService(int rows, int cols);
    int getNumRows() const;
    int getNumCols() const;

    void toggleReservation(int row, int col);

    bool buySeats(int n);

    SeatStatus getStatus(int row, int col) const;

    std::vector<Seat*> getBoughtSeats();

    std::vector<Seat*> getFreeSeats();

    void markBought(Seat* seat);

    int countFree() const;

    Seat* SeatService::getSeat(int row, int col);

private:
    bool isValid(int row, int col) const;
};
