#include "PriorityManager.h"
#include "assessments.h"
#include <algorithm>

void PriorityManager::update(Subject* s) {
    int finalPriority = 0;
    bool hasBadGrades = false;

    for (Assessments* task : s->GetAssessments()) {
        finalPriority = std::max(finalPriority, task->getBasePriority());

        if (task->hasGrades()) {
            double currentScore = task->getCurrentScore();
            ScaleType scale = task->getScale();

            // Перевірка на погані оцінки
            if ((scale == ScaleType::FivePoint && currentScore < 3.0) || 
                (scale == ScaleType::TwelvePoint && currentScore < 4.0) || 
                (scale == ScaleType::TenPoint && currentScore < 4.0) ||
                (scale == ScaleType::Accumulative && currentScore < 60.0)) {
                hasBadGrades = true;
            }
        }
    }

    if (hasBadGrades) {
        finalPriority += 2000;
    }

    subjectPriorities[s] = finalPriority; 
}

int PriorityManager::getPriorityForSubject(Subject* s) {
    return subjectPriorities[s];
}