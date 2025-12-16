#ifndef INPUTPARSER_H
#define INPUTPARSER_H

#include <string>
#include <vector>
#include <map>
#include "models/Order.h"
#include "models/Warehouse.h"
#include "models/Vehicle.h"
#include "models/Event.h"

class InputParser {
public:
    InputParser();
    
    bool parse(const std::string& filename);
    
    // Getters for parsed data
    int getNumWarehouses() const { return m_numWarehouses; }
    int getNumItems() const { return m_numItems; }
    int getNumVehicles() const { return m_numVehicles; }
    
    const std::vector<std::vector<int>>& getTravelTimes() const { return m_travelTimes; }
    const std::map<int, Warehouse>& getWarehouses() const { return m_warehouses; }
    const std::map<int, Vehicle>& getVehicles() const { return m_vehicles; }
    const std::vector<EventPtr>& getEvents() const { return m_events; }
    
    std::string getError() const { return m_error; }
    
private:
    bool parseVehicles(std::ifstream& file);
    bool parseWarehouses(std::ifstream& file);
    bool parseEvents(std::ifstream& file);
    
    int m_numWarehouses;
    int m_numItems;
    int m_numVehicles;
    
    std::vector<std::vector<int>> m_travelTimes;
    std::map<int, Warehouse> m_warehouses;
    std::map<int, Vehicle> m_vehicles;
    std::vector<EventPtr> m_events;
    
    std::string m_error;
};

#endif // INPUTPARSER_H
