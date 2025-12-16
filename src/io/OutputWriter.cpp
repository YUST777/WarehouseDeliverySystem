#include "OutputWriter.h"
#include <fstream>
#include <algorithm>

OutputWriter::OutputWriter() {}

bool OutputWriter::writeImpl(const std::string& filename,
                             const std::map<int, Order>& orders,
                             const std::vector<int>& deliveredOrders,
                             const Statistics& stats) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        m_error = "Could not open file for writing: " + filename;
        return false;
    }
    
    // Sort delivered orders by finish time
    std::vector<int> sorted = deliveredOrders;
    std::sort(sorted.begin(), sorted.end(), [&orders](int a, int b) {
        return orders.at(a).getFinishTime() < orders.at(b).getFinishTime();
    });
    
    // Write each delivered order
    // Format: FT OrderID RT WT TransitTime AssignedWID AssignedVID Filled Value
    for (int oid : sorted) {
        const Order& o = orders.at(oid);
        int waitTime = o.getAssignTime() - o.getRequestTime();
        int transitTime = o.getFinishTime() - o.getDispatchTime();
        
        out << o.getFinishTime() << " "
            << o.getId() << " "
            << o.getRequestTime() << " "
            << waitTime << " "
            << transitTime << " "
            << o.getAssignedWarehouse() << " "
            << o.getAssignedVehicle() << " "
            << "Yes " 
            << o.getValue() << "\n";
    }
    
    out << "\n";
    
    // Write statistics
    out << "Total Orders: " << stats.totalOrders << "\n";
    out << "VIP Orders: " << stats.vipOrders << "\n";
    out << "Standard Orders: " << stats.stdOrders << "\n";
    out << "Delivered: " << stats.deliveredOrders << "\n";
    out << "Canceled: " << stats.canceledOrders << "\n";
    out << "Total Value: " << stats.totalValue << "\n";
    out << "Avg Wait Time: " << stats.avgWaitTime << "\n";
    out << "Avg Transit Time: " << stats.avgTransitTime << "\n";
    out << "On-Time Rate: " << stats.onTimeRate << "%\n";
    
    out.close();
    return true;
}
