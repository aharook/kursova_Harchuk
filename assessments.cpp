#include <iostream>
#include <string>
#include <vector>
#include "grades.h"

enum class AssessmentType {
    EXAM,
    COURSEWORK,
    PRACTICE,
    REGULAR
};

class assessments {

    AssessmentType Type;
    double maxPoints;
    std::string deadline;
    bool IsBlocker;
    std::vector<Grade*> Grades;

public:
    assessments(AssessmentType Type, double maxPoints, const std::string& deadline, bool Isblocker, const std::vector<Grade*> Grades = {} )
    : Type(Type), maxPoints(maxPoints), deadline(deadline), IsBlocker(Isblocker), Grades(Grades){}

    AssessmentType getType() const {return Type;}
    double getMaxPoints() const {return maxPoints;}
    std::string getDeadline() const {return deadline;}
    bool getIsBlocker() const {return IsBlocker;}
    std::vector<Grade*> getGrades() const {return Grades;}

    void addGrade(Grade* newGrade) {
        Grades.push_back(newGrade);
    }
};

int main(){
    
};