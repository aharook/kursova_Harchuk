#ifndef ACADEMICSYSTEM_H
#define ACADEMICSYSTEM_H

#include "Gradebook.h"
#include "SemesterManager.h"
#include "AnualReportBuilder.h"
#include "GradeConverter.h"
#include "DataManager.h"
#include "YearlyReport.h"
#include <string>
#include <vector>

class AcademicSystem {
private:
    Gradebook gradebook;
    SemesterManager semesterManager; 
    GradeConverter converter;
    DataManager dataManager;
    AnnualReportBuilder reportBuilder;
    std::vector<Subject*> archivedSubjects; 

public:
    AcademicSystem() : converter("scales.csv") {}
    ~AcademicSystem() {
        for (Subject* sub : archivedSubjects) delete sub;
    }

    void addSubjectToCurrentSemester(Subject* subject) {
        gradebook.addSubject(subject);
    }

    Gradebook& getGradebook() { 
        return gradebook; 
    }
    
    int getCurrentSemester() const {
        return semesterManager.getCurrentSemester(); 
    }

    void saveSystemState(const std::string& filename) {
        dataManager.saveCurrentProgress(gradebook, semesterManager.getCurrentSemester(), filename);
    }

    bool loadSystemState(const std::string& filename) {
        int tempSemester = 1; 
        bool success = dataManager.loadCurrentProgress(gradebook, tempSemester, filename);
        return success;
    }

    bool endSemester() {
        bool hasBlockers = gradebook.hasPendingBlockers();
        

        semesterManager.transitionToNextSemester(gradebook, archivedSubjects);
        
        return hasBlockers; 
    }

    YearlyReport endYear(int year) {
        return reportBuilder.generateReport(year, archivedSubjects, gradebook.getActualAverages(), converter);
    }
};

#endif
