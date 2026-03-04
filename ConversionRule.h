#ifndef CONVERSIONRULE_H
#define CONVERSIONRULE_H

#include "assessments.h"
#include <map>


class ConversionRule {
private:

    std::map<ScaleType, double> scaleValues;

public:
    void addValue(ScaleType scale, double value) {
        scaleValues[scale] = value;
    }
    double getValueForScale(ScaleType scale) const {
        auto it = scaleValues.find(scale);
        if (it != scaleValues.end()) {
            return it->second;
        }
        return 0.0; 
    }
};

#endif