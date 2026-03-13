#include "assessments.h"
#include <map>
#include "averageCalculation.h" 

Assessments::Assessments(AssessmentType Type, ScaleType scale, int basePriority, bool Isblocker, ICalculationStrategy* strategy, const std::vector<double>& Grades)
    : Type(Type), scale(scale), basePriority(basePriority), IsBlocker(Isblocker), strategy(strategy), Grades(Grades) {}

Assessments::~Assessments() {
    delete strategy; 
}

AssessmentType Assessments::getType() const { return Type; }
ScaleType Assessments::getScale() const { return scale; }
int Assessments::getBasePriority() const { return basePriority; }
bool Assessments::getIsBlocker() const { return IsBlocker; }
std::vector<double> Assessments::getGrades() const { return Grades; }
bool Assessments::hasGrades() const { return !Grades.empty(); }

double Assessments::getMaxAllowedGrade() const {
    switch (scale) {
        case ScaleType::TwelvePoint: return 12.0;
        case ScaleType::TenPoint: return 10.0;
        case ScaleType::FivePoint: return 5.0;
        case ScaleType::Accumulative: return 100.0; 
        default: return 100.0;
    }
}

void Assessments::clearGrades() {
    Grades.clear();
}

void Assessments::addGrade(double newGrade) { Grades.push_back(newGrade); }

double Assessments::getCurrentScore() const {
    if (Grades.empty() || !strategy) return 0.0;
    return strategy->calculate(Grades); 
}

bool Assessments::isPassed() const {
    if (Grades.empty()) return false;
    
    double score = getCurrentScore();
    
    static const std::map<ScaleType, double> passingThresholds = {
        {ScaleType::TenPoint, 6.0},
        {ScaleType::TwelvePoint, 4.0},
        {ScaleType::FivePoint, 3.0},
        {ScaleType::Accumulative, 60.0}
    };

    auto it = passingThresholds.find(scale);
    if (it != passingThresholds.end()) {
        return score >= it->second;
    }
    
    return score > 0.0; 
}

Assessments* AssessmentFactory::createExam(ScaleType scale) {
    return new Assessments(AssessmentType::EXAM, scale, 1000, true, new SingleGradeStrategy());
}

Assessments* AssessmentFactory::createCoursework(ScaleType scale) {
    return new Assessments(AssessmentType::COURSEWORK, scale, 500, true, new SingleGradeStrategy());
}

Assessments* AssessmentFactory::createPractice(ScaleType scale) {
    return new Assessments(AssessmentType::PRACTICE, scale, 200, false, new SingleGradeStrategy());
}

Assessments* AssessmentFactory::createRegular(ScaleType scale) {
    ICalculationStrategy* currentStrategy = nullptr;

    if (scale == ScaleType::Accumulative) {

        currentStrategy = new SumStrategy(); 
    } else {

        currentStrategy = new AverageStrategy();
    }

    return new Assessments(AssessmentType::REGULAR, scale, 50, false, currentStrategy);
}
