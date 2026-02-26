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
        
        AverageStrategy avgCalc; 

        for (const Subject* sub : subjects) {
            std::vector<double> subjectScores;

            for (const Assessments* task : sub->GetAssessments()) {
                subjectScores.push_back(task->getCurrentScore());
            }


            actualAverages[sub->Genlink_id()] = avgCalc.calculate(subjectScores);
        }

        return actualAverages;
    }
};

#endif