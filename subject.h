#ifndef SUBJECT_H
#define SUBJECT_H

#include <string>
#include <vector>
#include "assessments.h"
#include "IObservers.h"

class Subject {
private:
    std::string Name;
    std::string link_id;
    int semester;
    bool IsmultiSemester;
    std::vector<Assessments*> assessments;
    std::vector<IObserver*> observers;

public:
    Subject(const std::string& Name, const std::string& link_id, int semester, bool IsmultiSemester, const std::vector<Assessments*>& assessments_list = {});

    std::string Getname() const { return Name; }
    std::string Genlink_id() const { return link_id; }
    int Getsemestr() const { return semester; }
    std::vector<Assessments*> GetAssessments() const { return assessments; }
    
    void SetName(const std::string& newName);
    void setSemester(int newSemester);
    void setIsMultiSemester(bool status);
    
    void addAssessment(Assessments* newassessmentGrade);
    bool hasPendingBlockers() const;

    void attach(IObserver* observer);
    void notifyObservers();
};

#endif