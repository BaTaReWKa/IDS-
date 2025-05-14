#pragma once
#include <string>
#include <chrono>


class Sensor
{
private:
    std::string id;
    std::string place;
    std::chrono::system_clock::time_point lastUpdate;
    double lastDistance; // Последнее измеренное расстояние (в см или мм — зависит от применения)

public:
    Sensor(const std::string& id, const std::string& place)
        : id(id), place(place), lastDistance(-1.0) {
    }

    std::string getId() const { return id; }
    std::string getPlace() const { return place; }

    void setDistance(double d) { 
        lastDistance = d;
        lastUpdate = std::chrono::system_clock::now();
    }

    bool isTimedOut(int seconds = 5) const {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdate).count() > seconds;
    }

    void setLastDistance(double distance) { lastDistance = distance; }
    double getLastDistance() const { return lastDistance; }
};
