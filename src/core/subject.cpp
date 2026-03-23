#include "subject.h"
#include "GradePolicy.h"
#include "SubjectPerformance.h"


std::string Subject::generateLinkId(const std::string& name) {
    std::hash<std::string> hasher;
    return std::to_string(hasher(name));
}

Subject::Subject(const std::string& Name, int semester, bool IsmultiSemester, const std::vector<Assessments*> assessments)
    : Name(Name), IsmultiSemester(IsmultiSemester), assessments(assessments), semester(semester) {
    link_id = generateLinkId(Name);
}

Subject::Subject(const std::string& Name, int semester, bool IsmultiSemester, const std::string& existingId, const std::vector<Assessments*> assessments)
    : Name(Name), IsmultiSemester(IsmultiSemester), link_id(existingId), assessments(assessments), semester(semester) {
}


Subject::~Subject() {
    for (Assessments* task : assessments) {
        delete task;
    }
    assessments.clear();
}

void Subject::SetName(const std::string& newName) { 
    Name = newName; 
}

void Subject::setIsMultiSemester(bool status) { 
    IsmultiSemester = status; 
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

void Subject::addGradeToTask(int taskIndex, double grade) {
    if (taskIndex >= 0 && taskIndex < assessments.size()) {
        assessments[taskIndex]->addGrade(grade);
        notifyObservers();
    }
}

bool Subject::hasPendingBlockers() const {
    return SubjectPerformance::hasPendingBlockers(assessments);
}
int Subject::getPriorityScore() const { 
    return SubjectPerformance::getPriorityScore(assessments);
}
ScaleType Subject::getScale() const {
    return SubjectPerformance::resolveRegularScale(assessments);
}

double Subject::getCurrentScore() const {
    return SubjectPerformance::calculateRegularScore(assessments);
}

bool Subject::isPassed() const {

    if (hasPendingBlockers()) return false;


    if (!SubjectPerformance::hasAnyRegularGrades(assessments)) return false;


    double score = getCurrentScore();
    return GradePolicy::isPassingScore(getScale(), score);
}
