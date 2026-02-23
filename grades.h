#ifndef GRADES_H
#define GRADES_H

#include <iostream>
#include <chrono>

enum class ScaleType {
    HundredPoint,
    TwelvePoint,
    FivePoint,
    PassFail
};

struct Date {
    std::chrono::system_clock::time_point value; 
    
    static Date now() {
        return {std::chrono::system_clock::now()};
    }
};

class Grade {
private:
    int value;
    ScaleType scale;
    Date date;

public:
    Grade(int value, ScaleType scale, Date date)
        : value(value), scale(scale), date(date) {}

    int getValue() const { return value; }
    ScaleType getScale() const { return scale; }
    Date getDate() const { return date; }
};

#endif // GRADES_H
