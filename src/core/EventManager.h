#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <queue>
#include <vector>
#include "models/Event.h"

class EventManager {
public:
    EventManager();
    
    // Event queue operations
    void addEvent(EventPtr event);
    EventPtr getNextEvent();
    bool hasEventsAt(int timestamp) const;
    bool hasEvents() const { return !m_eventQueue.empty(); }
    int getNextEventTime() const;
    
    // Get all events at a specific timestamp
    std::vector<EventPtr> getEventsAt(int timestamp);
    
    // Peek upcoming events without removing
    std::vector<EventPtr> peekUpcoming(int count) const;
    
    // Clear all events
    void clear();
    
    // Statistics
    int getTotalEvents() const { return m_totalEventsProcessed; }
    
private:
    std::priority_queue<EventPtr, std::vector<EventPtr>, EventComparator> m_eventQueue;
    int m_totalEventsProcessed;
};

#endif // EVENTMANAGER_H
