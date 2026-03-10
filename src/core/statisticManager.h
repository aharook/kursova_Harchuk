#ifndef STATISTICMANAGER_H
#define STATISTICMANAGER_H

#include <iostream>
#include <map>
#include <string>
#include "IObservers.h"
#include "Gradebook.h"
#include "GradeConverter.h"

class StatisticsManager : public IObserver {
private:
    Gradebook* myGradebook;
    GradeConverter converter;
    double currentGPA = -1.0; 

public:
    StatisticsManager(Gradebook* gb) : myGradebook(gb), converter("scales.csv") {}

    void update(Subject* s) override {
        if (myGradebook->hasPendingBlockers()) {
            currentGPA = -1.0; 
            return; 
        }

        std::map<std::string, double> averages = myGradebook->getActualAverages();
        double totalConvertedSum = 0.0;
        int count = 0;
        ScaleType targetScale = ScaleType::Accumulative; 

        for (const auto& pair : averages) {
            std::string link_id = pair.first;
            double rawScore = pair.second;
            
            ScaleType originalScale = myGradebook->getSubjectScale(link_id);
            double convertedScore = converter.convert(rawScore, originalScale, targetScale);
            
            totalConvertedSum += convertedScore;
            count++;
        }

        currentGPA = (count > 0) ? (totalConvertedSum / count) : 0.0;
    }

    double getGPA() const { 
        return currentGPA; 
    }

    bool isSemesterFinished() const {
        return currentGPA != -1.0;
    }
};

#endif