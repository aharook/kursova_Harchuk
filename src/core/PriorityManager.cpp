#include "PriorityManager.h"
#include "assessments.h"
#include "GradePolicy.h"
#include <algorithm>

void PriorityManager::update(Subject* s) {
    if (s == nullptr) {
        return; 
    }

    int finalPriority = 0;
    bool hasBadGrades = false;

    for (Assessments* task : s->GetAssessments()) {
        finalPriority = std::max(finalPriority, task->getBasePriority());

        if (task->hasGrades()) {
            double currentScore = task->getCurrentScore();
            ScaleType scale = task->getScale();
            if (!GradePolicy::isPassingScore(scale, currentScore)) {
                hasBadGrades = true;
            }
        }
    }

    if (hasBadGrades) {
        finalPriority += GradePolicy::BAD_GRADE_PENALTY;
    }

    subjectPriorities[s] = finalPriority; 
}

int PriorityManager::getPriorityForSubject(Subject* s) {
    if (s == nullptr) {
        return 0;
    }
    return subjectPriorities[s];
}
