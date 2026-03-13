#ifndef SUBJECT_H
#define SUBJECT_H

#include <string>
#include <vector>
#include "assessments.h"
#include "IObservers.h"
#include <functional>

class Subject {
private:
    std::string Name;
    std::string link_id;
    bool IsmultiSemester;
    std::vector<Assessments*> assessments;
    std::vector<IObserver*> observers;
    int semester = 1;
    std::string generateLinkId(const std::string& name);

public:
    Subject(const std::string& Name, int semester, bool IsmultiSemester, const std::vector<Assessments*> assessments = {});
    

    Subject(const std::string& Name, int semester, bool IsmultiSemester, const std::string& existingId, const std::vector<Assessments*> assessments = {});

    ~Subject(); 

    std::string Getname() const { return Name; }
    std::string getLinkId() const { return link_id; } 
    bool getIsMultiSemester() const { return IsmultiSemester; }
    std::vector<Assessments*> GetAssessments() const { return assessments; }
    
    void SetName(const std::string& newName);
    void setIsMultiSemester(bool status);
    
    void addAssessment(Assessments* newassessmentGrade);

    void attach(IObserver* observer);
    void notifyObservers();

    void addGradeToTask(int taskIndex, double grade);

    void setSemester(int s) { semester = s; }
    int getSemester() const { return semester; }
    
    bool hasPendingBlockers() const;
    int getPriorityScore() const; 
    ScaleType getScale() const;
    double getCurrentScore() const;
    bool isPassed() const;
};

#endif
