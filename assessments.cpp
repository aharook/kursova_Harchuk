#include "assessments.h"
#include "averageCalculation.h" 



Assessments::Assessments(AssessmentType Type, ScaleType scale, int maxPoints,  bool Isblocker, ICalculationStrategy* strategy, const std::vector<double>& Grades)
    : Type(Type), scale(scale), maxPoints(maxPoints), IsBlocker(Isblocker), strategy(strategy), Grades(Grades) {}

Assessments::~Assessments() {
    delete strategy; 
}

AssessmentType Assessments::getType() const { return Type; }
ScaleType Assessments::getScale() const { return scale; }
int Assessments::getMaxPoints() const { return maxPoints; }
bool Assessments::getIsBlocker() const { return IsBlocker; }
std::vector<double> Assessments::getGrades() const { return Grades; }

void Assessments::addGrade(double newGrade) { Grades.push_back(newGrade); }



double Assessments::getCurrentScore() const {
    if (Grades.empty() || !strategy) return 0.0;

    return strategy->calculate(Grades); 
}

bool Assessments::isPassed() const {
    if (Grades.empty()) return false;
    double passingThreshold = maxPoints * 0.6;
    return getCurrentScore() >= passingThreshold;
}


Assessments* AssessmentFactory::createExam(int maxPoints, ScaleType scale) {

    return new Assessments(AssessmentType::EXAM, scale, maxPoints, true, new SingleGradeStrategy());
}

Assessments* AssessmentFactory::createCoursework(int maxPoints,ScaleType scale) {

    return new Assessments(AssessmentType::COURSEWORK, scale, maxPoints, true, new SingleGradeStrategy());
}

Assessments* AssessmentFactory::createPractice(int maxPoints,ScaleType scale) {

    return new Assessments(AssessmentType::PRACTICE, scale, maxPoints, false, new SingleGradeStrategy());
}

Assessments* AssessmentFactory::createRegular(int maxPoints, ScaleType scale) {

    return new Assessments(AssessmentType::REGULAR, scale, maxPoints, false, StrategyFactory::createStrategy(scale));
}