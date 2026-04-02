#include "SemesterManager.h"

SemesterManager::SemesterManager() : currentSemester(1) {}

int SemesterManager::getCurrentSemester() const {
    return currentSemester;
}

void SemesterManager::setCurrentSemester(int semester) {
    currentSemester = (semester > 0) ? semester : 1;
}

bool SemesterManager::canEndSemester(const Gradebook& activeGradebook) const {
    return !activeGradebook.hasPendingBlockers();
}

void SemesterManager::transitionToNextSemester(Gradebook& activeGradebook, std::vector<Subject*>& archive) {
    std::vector<Subject*> currentSubjects = activeGradebook.extractAllSubjects();
    std::vector<Subject*> subjectsForNextSemester;

    for (Subject* sub : currentSubjects) {
        archive.push_back(sub);

        if (sub->getIsMultiSemester()) {
            Subject* continuationSubject = new Subject(
                sub->Getname(),
                currentSemester + 1,
                true,
                sub->getLinkId() 
            );
            if (sub->hasCustomUsersPriority()) {
                continuationSubject->setUsersPriority(sub->getUsersPriority());
            }
            subjectsForNextSemester.push_back(continuationSubject);
        }
    }

    for (Subject* sub : subjectsForNextSemester) {
        activeGradebook.addSubject(sub);
    }

    currentSemester++;
}
