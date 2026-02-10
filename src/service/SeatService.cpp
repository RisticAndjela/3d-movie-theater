#include "SeatService.h"

    SeatService::SeatService(int rows, int cols)
        : numRows(rows), numCols(cols) {
        seats.resize(numRows);
        for (int r = 0; r < numRows; ++r)
            for (int c = 0; c < numCols; ++c)
                seats[r].emplace_back(r, c);
    }
	int SeatService::getNumRows() const { return numRows; }
	int SeatService::getNumCols() const { return numCols; }

    void SeatService::toggleReservation(int row, int col) {
        if (!isValid(row, col)) return;

        Seat& seat = seats[row][col];
        if (seat.status == SeatStatus::Free)
            seat.status = SeatStatus::Reserved;
        else if (seat.status == SeatStatus::Reserved)
            seat.status = SeatStatus::Free;
    }

    bool SeatService::buySeats(int n) {
        if (n <= 0 || n > numCols) return false;

        // od poslednjeg reda ka prvom
        for (int r = numRows - 1; r >= 0; --r) {

            // desno ka levo (desni kraj bloka)
            for (int j = numCols - 1; j >= 0; --j) {
                int start = j - (n - 1);
                if (start < 0) continue;

                bool canBuy = true;
                for (int c = start; c <= j; ++c) {
                    if (seats[r][c].status != SeatStatus::Free) {
                        canBuy = false;
                        break;
                    }
                }

                if (canBuy) {
                    for (int c = start; c <= j; ++c)
                        seats[r][c].status = SeatStatus::Bought;
                    return true;
                }
            }
        }
        return false;
    }

    SeatStatus SeatService::getStatus(int row, int col) const {
        if (!isValid(row, col)) return SeatStatus::Free;
        return seats[row][col].status;
    }

    std::vector<Seat*> SeatService::getBoughtSeats() {
        std::vector<Seat*> result;
        for (auto& row : seats)
            for (auto& s : row)
                if (s.status == SeatStatus::Bought)
                    result.push_back(&s);
        return result;
    }

    std::vector<Seat*> SeatService::getFreeSeats() {
        std::vector<Seat*> result;
        for (auto& row : seats)
            for (auto& s : row)
                if (s.status == SeatStatus::Free)
                    result.push_back(&s);
        return result;
    }

    void SeatService::markBought(Seat* seat) {
        if (seat) seat->status = SeatStatus::Bought;
    }

    int SeatService::countFree() const {
        int c = 0;
        for (const auto& r : seats)
            for (const auto& s : r)
                if (s.status == SeatStatus::Free) ++c;
        return c;
    }

    Seat* SeatService::getSeat(int row, int col) {
        if (!isValid(row, col)) return nullptr;
        return &seats[row][col];
    }

    bool SeatService::isValid(int row, int col) const {
        return row >= 0 && row < numRows && col >= 0 && col < numCols;
    }
