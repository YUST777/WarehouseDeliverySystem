#include "InputParser.h"
#include <fstream>
#include <sstream>

InputParser::InputParser() 
    : m_numWarehouses(0), m_numItems(0), m_numVehicles(0) {}

bool InputParser::parse(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        m_error = "Could not open file: " + filename;
        return false;
    }
    
    std::string line;
    
    // Skip empty lines and read first line: W N V
    while (std::getline(file, line) && line.empty());
    std::stringstream ss(line);
    ss >> m_numWarehouses >> m_numItems >> m_numVehicles;
    
    // Read travel-time matrix (W x W)
    m_travelTimes.resize(m_numWarehouses + 1, std::vector<int>(m_numWarehouses + 1, 0));
    for (int i = 1; i <= m_numWarehouses; ++i) {
        std::getline(file, line);
        std::stringstream rowSS(line);
        for (int j = 1; j <= m_numWarehouses; ++j) {
            rowSS >> m_travelTimes[i][j];
        }
    }
    
    // Parse vehicles
    if (!parseVehicles(file)) return false;
    
    // Parse warehouses
    if (!parseWarehouses(file)) return false;
    
    // Parse events
    if (!parseEvents(file)) return false;
    
    file.close();
    return true;
}

bool InputParser::parseVehicles(std::ifstream& file) {
    std::string line;
    
    for (int i = 0; i < m_numVehicles; ++i) {
        std::getline(file, line);
        while (line.empty()) std::getline(file, line);
        
        std::stringstream ss(line);
        int id, speed, capacity, refFlag, homeWid;
        std::string type;
        
        ss >> id >> type >> speed >> capacity >> refFlag >> homeWid;
        
        VehicleType vtype = (type == "Refrigerated" || refFlag == 1) 
                           ? VehicleType::Refrigerated : VehicleType::Standard;
        
        m_vehicles[id] = Vehicle(id, vtype, speed, capacity, homeWid);
    }
    
    return true;
}

bool InputParser::parseWarehouses(std::ifstream& file) {
    std::string line;
    
    for (int i = 0; i < m_numWarehouses; ++i) {
        std::getline(file, line);
        while (line.empty()) std::getline(file, line);
        
        int wid;
        std::stringstream(line) >> wid;
        
        m_warehouses[wid] = Warehouse(wid, wid);  // Location node = warehouse ID
        
        // Read inventory line
        std::getline(file, line);
        std::stringstream invSS(line);
        for (int j = 1; j <= m_numItems; ++j) {
            int qty;
            invSS >> qty;
            m_warehouses[wid].setInventory(j, qty);
        }
    }
    
    return true;
}

bool InputParser::parseEvents(std::ifstream& file) {
    std::string line;
    
    // Read number of events
    std::getline(file, line);
    while (line.empty()) std::getline(file, line);
    
    int numEvents;
    std::stringstream(line) >> numEvents;
    
    for (int i = 0; i < numEvents; ++i) {
        std::getline(file, line);
        while (line.empty()) std::getline(file, line);
        
        std::stringstream ss(line);
        char eventType;
        int timestamp;
        ss >> eventType >> timestamp;
        
        if (eventType == 'R') {
            // Order arrival: R TS OrderID DestWID DueBy PriorityClass K
            int orderId, destWid, dueBy, k;
            std::string prioClass;
            ss >> orderId >> destWid >> dueBy >> prioClass >> k;
            
            bool isVip = (prioClass == "VIP");
            std::vector<std::pair<int, int>> demand;
            
            for (int j = 0; j < k; ++j) {
                std::getline(file, line);
                std::stringstream itemSS(line);
                int itemId, qty;
                itemSS >> itemId >> qty;
                demand.push_back({itemId, qty});
            }
            
            m_events.push_back(std::make_shared<OrderArrivalEvent>(
                timestamp, orderId, destWid, dueBy, isVip, demand));
                
        } else if (eventType == 'S') {
            // Restock: S TS WID K followed by K lines
            int wid, k;
            ss >> wid >> k;
            
            std::vector<std::pair<int, int>> items;
            for (int j = 0; j < k; ++j) {
                std::getline(file, line);
                std::stringstream itemSS(line);
                int itemId, qty;
                itemSS >> itemId >> qty;
                items.push_back({itemId, qty});
            }
            
            m_events.push_back(std::make_shared<RestockEvent>(timestamp, wid, items));
            
        } else if (eventType == 'C') {
            // Cancel: C TS OrderID
            int orderId;
            ss >> orderId;
            m_events.push_back(std::make_shared<CancelEvent>(timestamp, orderId));
            
        } else if (eventType == 'M') {
            // Maintenance: M TS VID Duration
            int vid, duration;
            ss >> vid >> duration;
            m_events.push_back(std::make_shared<MaintenanceEvent>(timestamp, vid, duration));
            
        } else if (eventType == 'U') {
            // Reroute: U TS i j NewTime
            int nodeA, nodeB, newTime;
            ss >> nodeA >> nodeB >> newTime;
            m_events.push_back(std::make_shared<RerouteEvent>(timestamp, nodeA, nodeB, newTime));
        }
    }
    
    return true;
}
