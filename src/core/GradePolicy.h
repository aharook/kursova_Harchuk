#ifndef GRADEPOLICY_H
#define GRADEPOLICY_H

#include <array>

#include "assessments.h"

class GradePolicy {
public:
    static constexpr int BLOCKER_PENALTY = 1000;
    static constexpr int BAD_GRADE_PENALTY = 2000;

    static double getPassingThreshold(ScaleType scale) {
        return resolvePolicy(scale).passingThreshold;
    }

    static double getMaxAllowedGrade(ScaleType scale) {
        return resolvePolicy(scale).maxAllowedGrade;
    }

    static bool isPassingScore(ScaleType scale, double score) {
        return score >= getPassingThreshold(scale);
    }

private:
    struct ScalePolicy {
        ScaleType scale;
        double passingThreshold;
        double maxAllowedGrade;
    };

    static ScalePolicy resolvePolicy(ScaleType scale) {
        constexpr std::array<ScalePolicy, 4> policies = {{
            {ScaleType::TenPoint, 6.0, 10.0},
            {ScaleType::TwelvePoint, 4.0, 12.0},
            {ScaleType::FivePoint, 3.0, 5.0},
            {ScaleType::Accumulative, 60.0, 100.0}
        }};

        for (const auto& policy : policies) {
            if (policy.scale == scale) {
                return policy;
            }
        }

        return {scale, 0.0, 100.0};
    }
};

#endif