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
    // Конструктор
    Grade(int value, ScaleType scale, Date date)
        : value(value), scale(scale), date(date) {}

    // --- ГЕТЕРИ ---
    int getValue() const { return value; }
    ScaleType getScale() const { return scale; }
    Date getDate() const { return date; }
};