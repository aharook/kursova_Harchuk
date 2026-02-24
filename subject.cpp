#include <iostream>
#include <string>
#include <vector>
#include "assessments.h"

class Subject{
    std::string Name;
    std::string link_id;
    int semester;
    bool IsmultiSemester;
    std::vector<Assessments*> assessments;
public:
    Subject(const std::string& Name, const std::string& link_id, int semester, bool IsmultiSemester, const std::vector<Assessments*> assessments_list = {} )
    : Name(Name), link_id(link_id), semester(semester), IsmultiSemester(IsmultiSemester), assessments(assessments_list){}

    std::string Getname()const {return Name;}
    std::string Genlink_id()const{return link_id;}
    int Getsemestr()const{return semester;}
    std::vector<Assessments*> GetAssessments()const{return assessments;}
    
    void SetName(const std::string& newName) { Name = newName; }
    void setSemester(int newSemester) { if (newSemester > 0) semester = newSemester; }
    void setIsMultiSemester(bool status) { IsmultiSemester = status; }

    void addAssessment(Assessments* newassessmentGrade) {
        assessments.push_back(newassessmentGrade);
    }

    bool hasPendingBlockers() const {
        for (Assessments* task : assessments) {
            if (task->getIsBlocker() == true && task->isPassed() == false) {
                return true;
            }
        }
        return false; 
    }
};

