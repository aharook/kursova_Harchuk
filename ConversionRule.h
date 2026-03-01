#ifndef CONVERSIONRULE_H
#define CONVERSIONRULE_H

#include "assessments.h"
#include <map>


class ConversionRule {
private:
    // Зберігаємо пари: Шкала -> Відповідний бал
    std::map<ScaleType, double> scaleValues;

public:
    // Додаємо нове значення для конкретної шкали
    void addValue(ScaleType scale, double value) {
        scaleValues[scale] = value;
    }

    // Отримуємо бал за шкалою. Якщо шкали немає - повертаємо 0.0
    double getValueForScale(ScaleType scale) const {
        auto it = scaleValues.find(scale);
        if (it != scaleValues.end()) {
            return it->second;
        }
        return 0.0; 
    }
};

#endif