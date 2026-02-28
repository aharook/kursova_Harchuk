#include <iostream>
#include <map>
#include <string>
#include "IObservers.h"
#include "gradebook.h"

class StatisticsManager : public IObserver {
private:
    Gradebook* myGradebook;
    double currentGPA = 0.0;

public:
    StatisticsManager(Gradebook* gb) : myGradebook(gb) {}

    void update(Subject* s) override {
        std::map<std::string, double> averages = myGradebook->getActualAverages();
        
        double totalSum = 0.0;
        int count = 0;

        for (const auto& pair : averages) {
            double subjectScore = pair.second;
            totalSum += subjectScore;
            count++;
        }

        currentGPA = (count > 0) ? (totalSum / count) : 0.0;
    }

    double getGPA() const { 
        return currentGPA; 
    }
};