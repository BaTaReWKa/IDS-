#pragma once
#include "Sensor.h"
#include <string>
#include <map>
#include <memory>

class House
{
private:
    std::string address;
    std::map<std::string, std::shared_ptr<Sensor>> sensors;
    double lastDistance = -1.0; // по умолчанию Ч нет данных


public:
    House(const std::string& address)
        : address(address) {
    }

    std::string getAddress() const { return address; }

    void setDistance(double dist) {
        for (auto& pair : sensors) {
            pair.second->setDistance(dist);
        }
    }

    double getDistance() const {
        return lastDistance;
    }

    void addSensor(const std::string& id, const std::string& place)
    {
        if (sensors.find(id) == sensors.end())
        {
            sensors[id] = std::make_shared<Sensor>(id, place);
        }
    }

    bool hasSensor(const std::string& id) const
    {
        return sensors.find(id) != sensors.end();
    }

    std::shared_ptr<Sensor> getSensor(const std::string& id) const
    {
        auto it = sensors.find(id);
        if (it != sensors.end())
            return it->second;
        return nullptr;
    }

    std::map<std::string, std::shared_ptr<Sensor>> getSensors() const
    {
        return sensors;
    }

    void removeSensor(const std::string& id)
    {
        sensors.erase(id);
    }

    void updateDistance(const std::string& id, double distance) {
        if (sensors.count(id)) {
            sensors[id]->setDistance(distance);
            lastDistance = distance; // ќбновл€ем и дл€ дома
        }
    }
};
