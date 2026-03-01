#ifndef GRADECONVERTER_H
#define GRADECONVERTER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include "ConversionRule.h"
#include "assessments.h"

class GradeConverter {
private:

    std::vector<ConversionRule> rules;

    void loadMappings(const std::string& filename) {
        std::ifstream file(filename);

        std::string line;
        std::getline(file, line); 

        while (std::getline(file, line)) {
            if (line.empty()) continue;

            std::stringstream ss(line);
            std::string cell;
            double v5, v10, v12;
            if (std::getline(ss, cell, ',')) v5 = std::stod(cell);
            if (std::getline(ss, cell, ',')) v10 = std::stod(cell);
            if (std::getline(ss, cell, ',')) v12 = std::stod(cell);

            ConversionRule rule;
            rule.addValue(ScaleType::FivePoint, v5);
            rule.addValue(ScaleType::TenPoint, v10);
            rule.addValue(ScaleType::TwelvePoint, v12);
            
            rules.push_back(rule);
        }
    }

public:
    GradeConverter(const std::string& filename = "scales.csv") {
        loadMappings(filename);
    }


    double convert(double grade, ScaleType fromScale, ScaleType toScale) const {
        if (fromScale == toScale) return grade;
        
        if (rules.empty()) return grade;

        const ConversionRule* bestMatch = &rules[0];
        double minDiff = 9999.0; 

        for (const auto& rule : rules) {
            double currentVal = rule.getValueForScale(fromScale);
            double diff = std::abs(currentVal - grade);
            
            if (diff < minDiff) {
                minDiff = diff;
                bestMatch = &rule;
            }
        }
        return bestMatch->getValueForScale(toScale);
    }
};

#endif