#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream> 
#include "Grades.h"


std::chrono::system_clock::time_point parseDeadline(const std::string& datetime_str) {
    std::tm tm = {};
    std::istringstream ss(datetime_str);
    
    ss >> std::get_time(&tm, "  %d.%m.%Y");
    std::time_t time = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(time);
}

enum class AssessmentType {
    EXAM,
    COURSEWORK,
    PRACTICE,
    REGULAR
};

class assessments {
    AssessmentType Type;
    double maxPoints;
    
    std::chrono::system_clock::time_point deadline; 
    
    bool IsBlocker;
    std::vector<Grade*> Grades;

public:
    assessments(AssessmentType Type, double maxPoints, std::chrono::system_clock::time_point deadline, bool Isblocker, const std::vector<Grade*> Grades = {} )
    : Type(Type), maxPoints(maxPoints), deadline(deadline), IsBlocker(Isblocker), Grades(Grades){}

    AssessmentType getType() const {return Type;}
    double getMaxPoints() const {return maxPoints;}
    bool getIsBlocker() const {return IsBlocker;}
    std::vector<Grade*> getGrades() const {return Grades;}
    std::chrono::system_clock::time_point getDeadline() const { return deadline; }

    void addGrade(Grade* newGrade) {
        Grades.push_back(newGrade);
    }

    bool isOverdue() const {
    auto now = std::chrono::system_clock::now();
    return now > deadline; 

    
}
};
class AssessmentFactory {
public:

    static assessments* createExam(double maxPoints, const std::string& deadline_str) {
        auto deadline = parseDeadline(deadline_str);
        return new assessments(AssessmentType::EXAM, maxPoints, deadline, true); 
    }

    static assessments* createCoursework(double maxPoints, const std::string& deadline_str) {
        auto deadline = parseDeadline(deadline_str);
        return new assessments(AssessmentType::COURSEWORK, maxPoints, deadline, true);
    }

    static assessments* createRegular(double maxPoints, const std::string& deadline_str = "") {
        auto deadline = parseDeadline(deadline_str);
        return new assessments(AssessmentType::REGULAR, maxPoints, deadline, false);
    }
};