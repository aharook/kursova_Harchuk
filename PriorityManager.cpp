#include <iostream>
#include <algorithm>
#include "IObservers.h"
#include "subject.h"
#include "assessments.h"

class PriorityManager : public IObserver {
public:
    void update(Subject* s) override {
        int finalPriority = 0;
        bool hasBadGrades = false;

        for (Assessments* task : s->GetAssessments()) {
            finalPriority = std::max(finalPriority, task->getBasePriority());

            if (task->hasGrades()) {
                double currentScore = task->getCurrentScore();
                ScaleType scale = task->getScale();

                if ((scale == ScaleType::FivePoint && currentScore < 3.0) || (scale == ScaleType::TwelvePoint && currentScore < 4.0) || (scale == ScaleType::TenPoint && currentScore < 4.0)) {
                    hasBadGrades = true;
                }
            }
        }

        if (hasBadGrades) {
            finalPriority += 2000;
        }

        s->setPriorityScore(finalPriority);
    }
};