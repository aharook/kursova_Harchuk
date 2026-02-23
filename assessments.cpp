#include <iostream>
#include <string>
#include <vector>

enum class AssessmentType {
    EXAM,
    COURSEWORK,
    PRACTICE,
    REGULAR
};
class assessments{

    AssessmentType Type;
    double maxPoints;
    std::string deadline;
    bool IsBlocker;
    std::vector<int> Grades;

public:

    assessments(AssessmentType Type, double maxPoints, std::string& deadline, bool Isblocker, std::vector<int>& Grades = {} )
    : Type(Type), maxPoints(maxPoints), deadline(deadline), IsBlocker(IsBlocker), Grades(Grades){}

    AssessmentType getType() const {return Type;}
    double getMaxPoints() const {return maxPoints;}
    std::string getDeadline() const {return deadline;}
    bool getIsBlocker() const {return IsBlocker;}
    std::vector<int> getGrades() const {return Grades;}

    void addGrade(double Grade) {
        Grades.push_back(Grade);
    }
};