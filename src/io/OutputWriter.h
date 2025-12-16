#ifndef OUTPUTWRITER_H
#define OUTPUTWRITER_H

#include <string>
#include <vector>
#include <map>
#include "models/Order.h"

class OutputWriter {
public:
    OutputWriter();
    
    // Generic statistics struct that can accept any compatible type
    struct Statistics {
        int totalOrders = 0;
        int deliveredOrders = 0;
        int canceledOrders = 0;
        int vipOrders = 0;
        int stdOrders = 0;
        double totalValue = 0;
        double avgWaitTime = 0;
        double avgTransitTime = 0;
        double onTimeRate = 0;
    };
    
    // Template method to accept any statistics-like struct
    template<typename T>
    bool write(const std::string& filename,
               const std::map<int, Order>& orders,
               const std::vector<int>& deliveredOrders,
               const T& stats) {
        Statistics s;
        s.totalOrders = stats.totalOrders;
        s.deliveredOrders = stats.deliveredOrders;
        s.canceledOrders = stats.canceledOrders;
        s.vipOrders = stats.vipOrders;
        s.stdOrders = stats.stdOrders;
        s.totalValue = stats.totalValue;
        s.avgWaitTime = stats.avgWaitTime;
        s.avgTransitTime = stats.avgTransitTime;
        s.onTimeRate = stats.onTimeRate;
        return writeImpl(filename, orders, deliveredOrders, s);
    }
    
    std::string getError() const { return m_error; }
    
private:
    bool writeImpl(const std::string& filename,
                   const std::map<int, Order>& orders,
                   const std::vector<int>& deliveredOrders,
                   const Statistics& stats);
    std::string m_error;
};

#endif // OUTPUTWRITER_H
