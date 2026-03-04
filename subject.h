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
    bool IsmultiSemester;
    std::vector<Assessments*> assessments;
    std::vector<IObserver*> observers;
    int priorityScore = 0; 
    int semester = 1;

public:
    Subject(const std::string& Name, const std::string& link_id, int semester, bool IsmultiSemester, const std::vector<Assessments*> assessments = {});

    std::string Getname() const { return Name; }

    std::string Genlink_id() const { return link_id; }
    
    std::vector<Assessments*> GetAssessments() const { return assessments; }
    
    void SetName(const std::string& newName);
    void setSemester(int newSemester);
    void setIsMultiSemester(bool status);
    
    void addAssessment(Assessments* newassessmentGrade);

    void attach(IObserver* observer);
    void notifyObservers();

    void addGradeToTask(int taskIndex, double grade);

    void setPriorityScore(int score) { priorityScore = score; }
    int getPriorityScore() const { return priorityScore; }
    bool hasPendingBlockers() const {}
    void setSemester(int s) {semester = s;}
    int getSemester() const {return semester;}
};

#endif