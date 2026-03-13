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
            double v100 = 0, v5 = 0, v10 = 0, v12 = 0;
                if (std::getline(ss, cell, ',')) v100 = std::stod(cell);
                if (std::getline(ss, cell, ',')) v5 = std::stod(cell);
                if (std::getline(ss, cell, ',')) v10 = std::stod(cell);
                if (std::getline(ss, cell, ',')) v12 = std::stod(cell);

                ConversionRule rule;
                rule.addValue(ScaleType::Accumulative, v100); 
                rule.addValue(ScaleType::FivePoint, v5);
                rule.addValue(ScaleType::TenPoint, v10);
                rule.addValue(ScaleType::TwelvePoint, v12);
                
                rules.push_back(rule); 
        }
    }
public:
    GradeConverter(const std::string& filename = "d:/Rcit/kursova_Harchuk/data/scales.csv") {
        loadMappings(filename);
    }
    double convert(double grade, ScaleType fromScale, ScaleType toScale) const {
        if (fromScale == toScale) return grade;
        
        if (rules.empty()){ return grade;} 


        double closestDiff = 1000000.0;
        double targetValue = 0.0;

        for (const auto& rule : rules) {
            double ruleFromVal = rule.getValueForScale(fromScale);
            double diff = std::abs(ruleFromVal - grade);
            
            if (diff < closestDiff) {
                closestDiff = diff;
                targetValue = rule.getValueForScale(toScale);
            }
        }

        return targetValue;
    }
};

#endif
