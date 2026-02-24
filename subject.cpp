#include "subject.h"

Subject::Subject(const std::string& Name, const std::string& link_id, int semester, bool IsmultiSemester, const std::vector<Assessments*>& assessments_list)
    : Name(Name), link_id(link_id), semester(semester), IsmultiSemester(IsmultiSemester), assessments(assessments_list) {}

void Subject::SetName(const std::string& newName) { 
    Name = newName; 
}

void Subject::setSemester(int newSemester) { 
    if (newSemester > 0) {
        semester = newSemester; 
    }
}

void Subject::setIsMultiSemester(bool status) { 
    IsmultiSemester = status; 
}

bool Subject::hasPendingBlockers() const {
    for (Assessments* task : assessments) {
        if (task->getIsBlocker() && !task->isPassed()) {
            return true;
        }
    }
    return false; 
}

void Subject::attach(IObserver* observer) {
    observers.push_back(observer);
}

void Subject::notifyObservers() {
    for (IObserver* obs : observers) {
        obs->update(this);
    }
}

void Subject::addAssessment(Assessments* newassessmentGrade) {
    assessments.push_back(newassessmentGrade);
    notifyObservers();
}