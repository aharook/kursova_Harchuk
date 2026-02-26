#include "assessments.h"
#include "averageCalculation.h" 

std::chrono::system_clock::time_point parseDeadline(const std::string& datetime_str) {
    if (datetime_str.empty()) {
        return std::chrono::time_point<std::chrono::system_clock>::max();
    }
    std::tm tm = {};
    std::istringstream ss(datetime_str);

    ss >> std::get_time(&tm, "%d.%m.%Y %H:%M");
    if (ss.fail()) return std::chrono::system_clock::from_time_t(0);
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

Assessments::Assessments(AssessmentType Type, ScaleType scale, int maxPoints, std::chrono::system_clock::time_point deadline, bool Isblocker, ICalculationStrategy* strategy, const std::vector<double>& Grades)
    : Type(Type), scale(scale), maxPoints(maxPoints), deadline(deadline), IsBlocker(Isblocker), strategy(strategy), Grades(Grades) {}

Assessments::~Assessments() {
    delete strategy; 
}

AssessmentType Assessments::getType() const { return Type; }
ScaleType Assessments::getScale() const { return scale; }
int Assessments::getMaxPoints() const { return maxPoints; }
bool Assessments::getIsBlocker() const { return IsBlocker; }
std::vector<double> Assessments::getGrades() const { return Grades; }
std::chrono::system_clock::time_point Assessments::getDeadline() const { return deadline; }

void Assessments::addGrade(double newGrade) { Grades.push_back(newGrade); }
bool Assessments::isOverdue() const { return std::chrono::system_clock::now() > deadline; }


double Assessments::getCurrentScore() const {
    if (Grades.empty() || !strategy) return 0.0;

    return strategy->calculate(Grades); 
}

bool Assessments::isPassed() const {
    if (Grades.empty()) return false;
    double passingThreshold = maxPoints * 0.6;
    return getCurrentScore() >= passingThreshold;
}


Assessments* AssessmentFactory::createExam(int maxPoints, ScaleType scale, const std::string& deadline_str) {

    return new Assessments(AssessmentType::EXAM, scale, maxPoints, parseDeadline(deadline_str), true, new SingleGradeStrategy());
}

Assessments* AssessmentFactory::createCoursework(int maxPoints,ScaleType scale, const std::string& deadline_str) {

    return new Assessments(AssessmentType::COURSEWORK, scale, maxPoints, parseDeadline(deadline_str), true, new SingleGradeStrategy());
}

Assessments* AssessmentFactory::createPractice(int maxPoints,ScaleType scale, const std::string& deadline_str) {

    return new Assessments(AssessmentType::PRACTICE, scale, maxPoints, parseDeadline(deadline_str), false, new SingleGradeStrategy());
}

Assessments* AssessmentFactory::createRegular(int maxPoints, ScaleType scale, const std::string& deadline_str) {

    return new Assessments(AssessmentType::REGULAR, scale, maxPoints, parseDeadline(deadline_str), false, StrategyFactory::createStrategy(scale));
}