#include "assessments.h"


std::chrono::system_clock::time_point parseDeadline(const std::string& datetime_str) {
    if (datetime_str.empty()) {
        return std::chrono::time_point<std::chrono::system_clock>::max();
    }
    std::tm tm = {};
    std::istringstream ss(datetime_str);

    ss >> std::get_time(&tm, "%d.%m.%Y %H:%M");
    if (ss.fail()) {
        return std::chrono::system_clock::from_time_t(0);
    }
    std::time_t time = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(time);
}


Assessments::Assessments(AssessmentType Type, int maxPoints, std::chrono::system_clock::time_point deadline, bool Isblocker, const std::vector<Grade*> Grades)
    : Type(Type), maxPoints(maxPoints), deadline(deadline), IsBlocker(Isblocker), Grades(Grades) {}


AssessmentType Assessments::getType() const { return Type; }
int Assessments::getMaxPoints() const { return maxPoints; }
bool Assessments::getIsBlocker() const { return IsBlocker; }
std::vector<Grade*> Assessments::getGrades() const { return Grades; }
std::chrono::system_clock::time_point Assessments::getDeadline() const { return deadline; }


void Assessments::addGrade(Grade* newGrade) {
    Grades.push_back(newGrade);
}

bool Assessments::isOverdue() const {
    auto now = std::chrono::system_clock::now();
    return now > deadline;
}

double Assessments::getCurrentScore() const {
    if (Grades.empty()) return 0.0;
    return Grades.back()->getValue(); 
}

bool Assessments::isPassed() const {
    if (Grades.empty()) return false;
    double passingThreshold = maxPoints * 0.6;
    return getCurrentScore() >= passingThreshold;
}

Assessments* AssessmentFactory::createExam(int maxPoints, const std::string& deadline_str) {
    return new Assessments(AssessmentType::EXAM, maxPoints, parseDeadline(deadline_str), true);
}

Assessments* AssessmentFactory::createCoursework(int maxPoints, const std::string& deadline_str) {
    return new Assessments(AssessmentType::COURSEWORK, maxPoints, parseDeadline(deadline_str), true);
}

Assessments* AssessmentFactory::createPractice(int maxPoints, const std::string& deadline_str) {
    return new Assessments(AssessmentType::PRACTICE, maxPoints, parseDeadline(deadline_str), false);
}

Assessments* AssessmentFactory::createRegular(int maxPoints, const std::string& deadline_str) {
    return new Assessments(AssessmentType::REGULAR, maxPoints, parseDeadline(deadline_str), false);
}