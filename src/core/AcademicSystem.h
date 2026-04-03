#ifndef ACADEMICSYSTEM_H
#define ACADEMICSYSTEM_H

#include "Gradebook.h"
#include "SemesterManager.h"
#include "AnualReportBuilder.h"
#include "GradeConverter.h"
#include "DataManager.h"
#include "YearlyReport.h"
#include "YearlyReportSaver.h"
#include <string>
#include <vector>
#include <stdexcept>

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
        if (year <= 0) {
            throw std::invalid_argument("Academic year must be positive.");
        }

        const int firstSemesterOfYear = year * 2 - 1;
        const int secondSemesterOfYear = year * 2;

        const int latestCompletedSemester = semesterManager.getCurrentSemester() - 1;
        const int latestReportSecondSemester = (latestCompletedSemester % 2 == 0)
            ? latestCompletedSemester
            : (latestCompletedSemester - 1);
        const int latestReportYear = (latestReportSecondSemester >= 2)
            ? (latestReportSecondSemester / 2)
            : 0;

        if (year != latestReportYear) {
            throw std::logic_error("Yearly report can be built only for the latest completed semester pair.");
        }

        std::vector<Subject*> yearSubjects;
        yearSubjects.reserve(archivedSubjects.size());

        bool hasFirstSemester = false;
        bool hasSecondSemester = false;

        for (Subject* subject : archivedSubjects) {
            const int semester = subject->getSemester();
            if (semester == firstSemesterOfYear || semester == secondSemesterOfYear) {
                yearSubjects.push_back(subject);
                if (semester == firstSemesterOfYear) {
                    hasFirstSemester = true;
                }
                if (semester == secondSemesterOfYear) {
                    hasSecondSemester = true;
                }
            }
        }

        if (!hasFirstSemester || !hasSecondSemester) {
            throw std::logic_error("Yearly report requires both semesters in the pair.");
        }

        return reportBuilder.generateReport(year, yearSubjects, converter);
    }

    YearlyReport generateAndSaveYearlyReport(int year, std::string& savedFileName) {
        YearlyReport report = endYear(year);

        savedFileName = YearlyReportSaver::save(report, year);

        return report;
    }
};

#endif
