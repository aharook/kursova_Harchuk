#ifndef GRADEPOLICY_H
#define GRADEPOLICY_H

#include "assessments.h"

class GradePolicy {
public:
    static constexpr int BLOCKER_PENALTY = 1000;
    static constexpr int BAD_GRADE_PENALTY = 2000;

    static double getPassingThreshold(ScaleType scale) {
        switch (scale) {
            case ScaleType::TenPoint: return 6.0;
            case ScaleType::TwelvePoint: return 4.0;
            case ScaleType::FivePoint: return 3.0;
            case ScaleType::Accumulative: return 60.0;
            default: return 0.0;
        }
    }

    static double getMaxAllowedGrade(ScaleType scale) {
        switch (scale) {
            case ScaleType::TwelvePoint: return 12.0;
            case ScaleType::TenPoint: return 10.0;
            case ScaleType::FivePoint: return 5.0;
            case ScaleType::Accumulative: return 100.0;
            default: return 100.0;
        }
    }

    static bool isPassingScore(ScaleType scale, double score) {
        return score >= getPassingThreshold(scale);
    }
};

#endif