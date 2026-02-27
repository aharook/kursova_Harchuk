#ifndef GRADEBOOK_H
#define GRADEBOOK_H

#include <vector>
#include <map>
#include <string>
#include "subject.h"
#include "averageCalculation.h" 

class Gradebook {
private:
    std::vector<Subject*> subjects; 

public:
    ~Gradebook() {
        for (Subject* sub : subjects) delete sub;
    }

    void addSubject(Subject* sub) {
        subjects.push_back(sub);
    }

    std::map<std::string, double> getActualAverages() const {
        std::map<std::string, double> actualAverages;
        
        for (const Subject* sub : subjects) {
            std::vector<Assessments*> assessments = sub->GetAssessments();
            
            if (assessments.empty()) {
                actualAverages[sub->Genlink_id()] = 0.0;
                continue;
            }

            std::vector<double> subjectScores;
            for (const Assessments* task : assessments) {
                subjectScores.push_back(task->getCurrentScore());
            }

            ScaleType scale = assessments.front()->getScale();
            
            ICalculationStrategy* strategy = StrategyFactory::createStrategy(scale);
            actualAverages[sub->Genlink_id()] = strategy->calculate(subjectScores);
            
            delete strategy; 
        }

        return actualAverages;
    }
};

#endif