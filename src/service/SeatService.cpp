#pragma once
#include <vector>
#include <iostream>
#include "../model/Seat.h"

class SeatService {
private:
    std::vector<std::vector<Seat>> seats; // redovi x kolone
    int numRows;
    int numCols;

public:
    SeatService(int rows, int cols) : numRows(rows), numCols(cols) {
        seats.resize(numRows);
        for (int r = 0; r < numRows; ++r) {
            for (int c = 0; c < numCols; ++c) {
                seats[r].emplace_back(r, c);
            }
        }
    }

    // Rezervacija / otkazivanje rezervacije klikom
    void toggleReservation(int row, int col) {
        if (!isValid(row, col)) return;

        Seat& seat = seats[row][col];
        if (seat.status == SeatStatus::Free) seat.status = SeatStatus::Reserved;
        else if (seat.status == SeatStatus::Reserved) seat.status = SeatStatus::Free;

        std::cout << "Seat [" << row << "," << col << "] status: " << toString(seat.status) << "\n";
    }

    // Kupovina N susednih slobodnih sedišta
    bool buySeats(int n) {
        for (int r = numRows - 1; r >= 0; --r) { // kreni od zadnjeg reda
            for (int c = numCols - n; c >= 0; --c) { // poslednjih n kolona
                bool canBuy = true;
                for (int i = 0; i < n; ++i) {
                    if (seats[r][c + i].status != SeatStatus::Free) {
                        canBuy = false;
                        break;
                    }
                }
                if (canBuy) {
                    for (int i = 0; i < n; ++i) {
                        seats[r][c + i].status = SeatStatus::Bought;
                        std::cout << "Seat [" << r << "," << c + i << "] bought.\n";
                    }
                    return true;
                }
            }
        }
        std::cout << "Not enough adjacent free seats to buy " << n << " seats.\n";
        return false;
    }

    // Dohvati status sedišta
    SeatStatus getStatus(int row, int col) const {
        if (!isValid(row, col)) return SeatStatus::Free;
        return seats[row][col].status;
    }

    void printSeats() const {
        for (int r = 0; r < numRows; ++r) {
            for (int c = 0; c < numCols; ++c) {
                char ch = '.';
                switch (seats[r][c].status) {
                case SeatStatus::Free: ch = 'F'; break;
                case SeatStatus::Reserved: ch = 'R'; break;
                case SeatStatus::Bought: ch = 'B'; break;
                }
                std::cout << ch << " ";
            }
            std::cout << "\n";
        }
    }

private:
    bool isValid(int row, int col) const {
        return row >= 0 && row < numRows && col >= 0 && col < numCols;
    }

    std::string toString(SeatStatus status) const {
        switch (status) {
        case SeatStatus::Free: return "Free";
        case SeatStatus::Reserved: return "Reserved";
        case SeatStatus::Bought: return "Bought";
        }
        return "Unknown";
    }
};
