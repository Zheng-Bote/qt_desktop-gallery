/**
 * @file rz_myevent.hpp
 * @author ZHENG Robert (robert.hase-zheng.net)
 * @brief Main class for the custom event
 * @version 0.1
 * @date 2025-03-01
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 */
#ifndef RZ_MYEVENT_HPP
#define RZ_MYEVENT_HPP

#include <QEvent>

class MyEvent : public QEvent
{
public:
    static Type myregisteredEventType()
    {
        static Type myType = static_cast<QEvent::Type>(QEvent::registerEventType());
        return myType;
    }

    MyEvent()
        : QEvent(myregisteredEventType())
    {
    }
};
#endif // RZ_MYEVENT_HPP
