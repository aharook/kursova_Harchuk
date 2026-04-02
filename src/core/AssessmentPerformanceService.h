#ifndef ASSESSMENT_PERFORMANCE_SERVICE_H
#define ASSESSMENT_PERFORMANCE_SERVICE_H

#include <vector>
#include "assessments.h"
#include "averageCalculation.h"
#include "GradePolicy.h"

class AssessmentPerformanceService {
private:
    static void collectRegularScores(
        const std::vector<Assessments*>& assessments,
        std::vector<double>& scores,
        ScaleType& scale,
        bool& hasRegularScale
    ) {
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

    static double calculateWithScale(const std::vector<double>& scores, ScaleType scale) {
        if (scores.empty()) {
            return 0.0;
        }

        ICalculationStrategy* strategy = StrategyFactory::createStrategy(scale);
        const double result = strategy->calculate(scores);
        delete strategy;
        return result;
    }

public:
    static bool hasPendingBlockers(const std::vector<Assessments*>& assessments) {
        for (const Assessments* task : assessments) {
            if (task->getIsBlocker() && !task->isPassed()) {
                return true;
            }
        }
        return false;
    }

    static bool hasAnyRegularGrades(const std::vector<Assessments*>& assessments) {
        for (const Assessments* task : assessments) {
            if (task->getType() == AssessmentType::REGULAR && task->hasGrades()) {
                return true;
            }
        }
        return false;
    }

    static int getPriorityScore(const std::vector<Assessments*>& assessments) {
        int totalPriority = 0;
        for (const Assessments* task : assessments) {
            totalPriority += task->getBasePriority();

            if (task->getIsBlocker() && !task->isPassed()) {
                totalPriority += GradePolicy::BLOCKER_PENALTY;
            }
        }
        return totalPriority;
    }

    static ScaleType resolveRegularScale(const std::vector<Assessments*>& assessments) {
        for (const Assessments* task : assessments) {
            if (task->getType() == AssessmentType::REGULAR) {
                return task->getScale();
            }
        }

        if (!assessments.empty()) {
            return assessments.front()->getScale();
        }

        return ScaleType::TwelvePoint;
    }

    static double calculateRegularScore(const std::vector<Assessments*>& assessments) {
        std::vector<double> scores;
        ScaleType scale = ScaleType::TwelvePoint;
        bool hasRegularScale = false;

        collectRegularScores(assessments, scores, scale, hasRegularScale);
        return calculateWithScale(scores, hasRegularScale ? scale : ScaleType::TwelvePoint);
    }
};

#endif
