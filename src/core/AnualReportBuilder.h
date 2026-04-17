#ifndef ANNUALREPORTBUILDER_H
#define ANNUALREPORTBUILDER_H

#include <vector>
#include <map>
#include <string>
#include "YearlyReport.h"
#include "subject.h"
#include "AcademicPorts.h"
#include "SubjectGroupPerformanceService.h"

class AnnualReportBuilder {
public:
    YearlyReport generateReport(int year, const std::vector<Subject*>& allYearSubjects, const IGradeConverter& converter) {
        std::map<std::string, std::vector<Subject*>> groupedSubjects;
        bool hasUnpassedSubjects = false;

        for (Subject* sub : allYearSubjects) {
            groupedSubjects[sub->getLinkId()].push_back(sub);
            if (!sub->isPassed()) {
                hasUnpassedSubjects = true;
            }
        }

        std::vector<SubjectResult> finalResults;
        double totalSum = 0.0;
        int subjectCount = 0;

        for (const auto& pair : groupedSubjects) {
            const std::vector<Subject*>& parts = pair.second;
            std::string name = parts[0]->Getname(); 

            ScaleType subjectScale = SubjectGroupPerformanceService::resolveRegularScale(parts);
            double rawScore = SubjectGroupPerformanceService::calculateRegularScore(parts);

            double normalizedScore = converter.convert(rawScore, subjectScale, ScaleType::Accumulative);

            finalResults.push_back({name, normalizedScore});
            
            totalSum += normalizedScore;
            subjectCount++;
        }

        double annualGPA = 0.0;
        if (subjectCount > 0) {
            annualGPA = totalSum / subjectCount;
        }

        bool canProceed = !hasUnpassedSubjects; 
        
        return YearlyReport(year, finalResults, annualGPA, canProceed);
    }
};

#endif
