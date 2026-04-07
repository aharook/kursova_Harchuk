#include "assessments.h"
#include "averageCalculation.h" 
#include "GradePolicy.h"

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
    return GradePolicy::getMaxAllowedGrade(scale);
}

void Assessments::clearGrades() {
    Grades.clear();
}

void Assessments::saveGrades(const std::vector<double>& newGrades, bool appendIfRegular) {
    if (newGrades.empty()) {
        return;
    }

    const bool canAppend = (Type == AssessmentType::REGULAR) && appendIfRegular;
    if (!canAppend) {
        Grades.clear();
    }

    if (Type == AssessmentType::REGULAR) {
        Grades.insert(Grades.end(), newGrades.begin(), newGrades.end());
        return;
    }

    Grades.push_back(newGrades.back());
}

void Assessments::addGrade(double newGrade) { Grades.push_back(newGrade); }

double Assessments::getCurrentScore() const {
    if (Grades.empty() || !strategy) return 0.0;
    return strategy->calculate(Grades); 
}

bool Assessments::isPassed() const {
    if (Grades.empty()) return false;

    double score = getCurrentScore();
    return GradePolicy::isPassingScore(scale, score);
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
