#include "SemesterManager.h"
#include "assessments.h"
#include <map>

namespace {

using AssessmentCreator = Assessments* (*)(ScaleType);

const std::map<AssessmentType, AssessmentCreator> kAssessmentCreators = {
    {AssessmentType::EXAM, AssessmentFactory::createExam},
    {AssessmentType::COURSEWORK, AssessmentFactory::createCoursework},
    {AssessmentType::PRACTICE, AssessmentFactory::createPractice},
    {AssessmentType::REGULAR, AssessmentFactory::createRegular}
};

Assessments* cloneAssessmentTemplate(const Assessments* source) {
    if (source == nullptr) {
        return nullptr;
    }

    const auto it = kAssessmentCreators.find(source->getType());
    if (it != kAssessmentCreators.end()) {
        return it->second(source->getScale());
    }

    return AssessmentFactory::createRegular(source->getScale());
}

}

SemesterManager::SemesterManager() : currentSemester(1) {}

int SemesterManager::getCurrentSemester() const {
    return currentSemester;
}

void SemesterManager::setCurrentSemester(int semester) {
    currentSemester = (semester > 0) ? semester : 1;
}

bool SemesterManager::canEndSemester(const Gradebook& activeGradebook) const {
    for (const Subject* subject : activeGradebook.getSubjects()) {
        if (!subject->isPassed()) {
            return false;
        }
    }

    return true;
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
                false,
                sub->getLinkId() 
            );

            for (const Assessments* assessment : sub->GetAssessments()) {
                Assessments* clonedAssessment = cloneAssessmentTemplate(assessment);
                if (clonedAssessment != nullptr) {
                    continuationSubject->addAssessment(clonedAssessment);
                }
            }

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
