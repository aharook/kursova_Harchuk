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
    AcademicSystem() : converter("scales.csv") {
        const std::string latestSave = dataManager.getLatestSaveFileName();
        if (!latestSave.empty()) {
            loadSystemState(latestSave);
        }
    }
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

    std::vector<std::string> getAvailableSaves() const {
        return dataManager.getListOfSaves();
    }

    std::string getLatestSaveName() const {
        return dataManager.getLatestSaveFileName();
    }

    void saveSystemState(const std::string& filename) {
        dataManager.saveCurrentProgress(gradebook, archivedSubjects, semesterManager.getCurrentSemester(), filename);
    }

    bool loadSystemState(const std::string& filename) {
        int loadedSemester = 1; 
        bool success = dataManager.loadCurrentProgress(gradebook, archivedSubjects, loadedSemester, filename);
        if (success) {
            semesterManager.setCurrentSemester(loadedSemester);
        }
        return success;
    }

    bool canEndCurrentSemester() const {
        return semesterManager.canEndSemester(gradebook);
    }

    bool endSemester() {
        if (!canEndCurrentSemester()) {
            return false;
        }

        semesterManager.transitionToNextSemester(gradebook, archivedSubjects);
        return true;
    }

    YearlyReport endYear(int year) {
        const int firstSemesterOfYear = year * 2 - 1;
        const int secondSemesterOfYear = year * 2;

        std::vector<Subject*> yearSubjects;
        yearSubjects.reserve(archivedSubjects.size());

        for (Subject* subject : archivedSubjects) {
            const int semester = subject->getSemester();
            if (semester == firstSemesterOfYear || semester == secondSemesterOfYear) {
                yearSubjects.push_back(subject);
            }
        }

        return reportBuilder.generateReport(year, yearSubjects, converter);
    }
};

#endif
