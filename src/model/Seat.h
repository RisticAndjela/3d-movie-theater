#pragma once
enum SeatStatus { Free, Reserved, Bought };

struct Seat {
    int row;
    int col;
    SeatStatus status;

    Seat(int r, int c) : row(r), col(c), status(SeatStatus::Free) {}

};