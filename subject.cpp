#include <iostream>
#include <string>
#include <vector>

class Subject{

    std::string Name;

    std::string link_id;

    int semester;

    bool IsmultiSemester;

    std::vector<int> assessments;
public:

    Subject(const std::string& Name,const std::string& link_id,int semester,bool IsmultiSemestr, const std::vector<int>& assessments = {} )
    : Name(Name), link_id(link_id), semester(semester), IsmultiSemester(IsmultiSemester), assessments(assessments){}

    std::string Getname()const {return Name;}

    std::string Genlink_id()const{return link_id;}

    int Getsemestr()const{return semester;}

    std::vector<int> GetAssessments()const{return assessments;}

    void SetName(const std::string& newName)
    {
        Name = newName;
    }

    void setSemester(int newSemester) {
        if (newSemester > 0) {
            semester = newSemester;
        }
    }
    void setIsMultiSemester(bool status) {
        IsmultiSemester = status;
    }

    void addAssessment(int assessmentGrade) {
        assessments.push_back(assessmentGrade);
    }
};