#include "EventManager.h"

EventManager::EventManager() : m_totalEventsProcessed(0) {}

void EventManager::addEvent(EventPtr event) {
    m_eventQueue.push(event);
}

EventPtr EventManager::getNextEvent() {
    if (m_eventQueue.empty()) return nullptr;
    EventPtr event = m_eventQueue.top();
    m_eventQueue.pop();
    m_totalEventsProcessed++;
    return event;
}

bool EventManager::hasEventsAt(int timestamp) const {
    if (m_eventQueue.empty()) return false;
    return m_eventQueue.top()->getTimestamp() == timestamp;
}

int EventManager::getNextEventTime() const {
    if (m_eventQueue.empty()) return -1;
    return m_eventQueue.top()->getTimestamp();
}

std::vector<EventPtr> EventManager::getEventsAt(int timestamp) {
    std::vector<EventPtr> events;
    while (!m_eventQueue.empty() && m_eventQueue.top()->getTimestamp() == timestamp) {
        events.push_back(m_eventQueue.top());
        m_eventQueue.pop();
        m_totalEventsProcessed++;
    }
    return events;
}

std::vector<EventPtr> EventManager::peekUpcoming(int count) const {
    std::vector<EventPtr> upcoming;
    // Create a copy of the queue to peek
    auto tempQueue = m_eventQueue;
    while (!tempQueue.empty() && upcoming.size() < static_cast<size_t>(count)) {
        upcoming.push_back(tempQueue.top());
        tempQueue.pop();
    }
    return upcoming;
}

void EventManager::clear() {
    while (!m_eventQueue.empty()) m_eventQueue.pop();
}
