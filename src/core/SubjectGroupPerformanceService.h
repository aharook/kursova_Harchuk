#ifndef SUBJECT_GROUP_PERFORMANCE_SERVICE_H
#define SUBJECT_GROUP_PERFORMANCE_SERVICE_H

#include <vector>
#include "subject.h"
#include "AssessmentPerformanceService.h"

class SubjectGroupPerformanceService {
public:
    static ScaleType resolveRegularScale(const std::vector<Subject*>& subjects) {
        for (const Subject* subject : subjects) {
            const std::vector<Assessments*>& assessments = subject->GetAssessments();
            for (const Assessments* task : assessments) {
                if (task->getType() == AssessmentType::REGULAR) {
                    return task->getScale();
                }
            }
        }
        return ScaleType::TwelvePoint;
    }

    static double calculateRegularScore(const std::vector<Subject*>& subjects) {
        std::vector<double> scores;
        ScaleType scale = ScaleType::TwelvePoint;
        bool hasRegularScale = false;

        for (const Subject* subject : subjects) {
            const std::vector<Assessments*>& assessments = subject->GetAssessments();
            for (const Assessments* task : assessments) {
                if (task->getType() != AssessmentType::REGULAR || !task->hasGrades()) {
                    continue;
                }

                if (!hasRegularScale) {
                    scale = task->getScale();
                    hasRegularScale = true;
                }

                scores.push_back(task->getCurrentScore());
            }
        }

        if (scores.empty()) {
            return 0.0;
        }

        ICalculationStrategy* strategy = StrategyFactory::createStrategy(hasRegularScale ? scale : ScaleType::TwelvePoint);
        const double result = strategy->calculate(scores);
        delete strategy;
        return result;
    }
};

#endif
