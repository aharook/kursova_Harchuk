#ifndef YEARLYREPORT_H
#define YEARLYREPORT_H

#include <string>
#include <vector>

struct SubjectResult {
    std::string subjectName;
    double finalScore;
};

class YearlyReport {
private:
    int academicYear;                      
    std::vector<SubjectResult> results;    
    double annualGPA;                      
    bool canProceedToNextYear;             

public:

    YearlyReport(int year, const std::vector<SubjectResult>& res, double gpa, bool canProceed)
        : academicYear(year), results(res), annualGPA(gpa), canProceedToNextYear(canProceed) {}

    int getAcademicYear() const { return academicYear; }
    std::vector<SubjectResult> getResults() const { return results; }
    double getAnnualGPA() const { return annualGPA; }
    bool getCanProceed() const { return canProceedToNextYear; }
};

#endif