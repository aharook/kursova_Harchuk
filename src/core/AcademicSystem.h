#ifndef ACADEMICSYSTEM_H
#define ACADEMICSYSTEM_H

#include "Gradebook.h"
#include "SemesterManager.h"
#include "AnualReportBuilder.h"
#include "GradeConverter.h"
#include "DataManager.h"
#include "YearlyReport.h"
#include "YearlyReportSaver.h"
#include <cstdio>
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
    AcademicSystem() : converter("data/scales.csv") {
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

    int getLatestReportableYear() const {
        const int latestCompletedSemester = semesterManager.getCurrentSemester() - 1;
        const int latestReportSecondSemester = (latestCompletedSemester % 2 == 0)
            ? latestCompletedSemester
            : (latestCompletedSemester - 1);

        if (latestReportSecondSemester < 2) {
            return 0;
        }

        return latestReportSecondSemester / 2;
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

    double convertGrade(double grade, ScaleType fromScale, ScaleType toScale) const {
        return converter.convert(grade, fromScale, toScale);
    }

    double calculateCurrentSemesterAverage(ScaleType targetScale, int& includedSubjects) const {
        includedSubjects = 0;
        double totalScore = 0.0;

        for (const Subject* subject : gradebook.getSubjects()) {
            bool hasRegularGrades = false;
            for (const Assessments* task : subject->GetAssessments()) {
                if (task->getType() == AssessmentType::REGULAR && task->hasGrades()) {
                    hasRegularGrades = true;
                    break;
                }
            }

            if (!hasRegularGrades) {
                continue;
            }

            const double convertedScore = converter.convert(
                subject->getCurrentScore(),
                subject->getScale(),
                targetScale
            );

            totalScore += convertedScore;
            includedSubjects++;
        }

        return (includedSubjects > 0) ? (totalScore / includedSubjects) : 0.0;
    }

    bool tryGenerateLatestYearlyReport(std::string& statusMessage) {
        const int year = getLatestReportableYear();
        if (year <= 0) {
            statusMessage = "Річний звіт: завершіть 2 семестри.";
            return false;
        }

        const int firstSemesterOfYear = year * 2 - 1;
        const int secondSemesterOfYear = year * 2;
        bool hasFirstSemester = false;
        bool hasSecondSemester = false;

        for (const Subject* subject : archivedSubjects) {
            if (subject->getSemester() == firstSemesterOfYear) {
                hasFirstSemester = true;
            }
            if (subject->getSemester() == secondSemesterOfYear) {
                hasSecondSemester = true;
            }
        }

        if (!hasFirstSemester || !hasSecondSemester) {
            statusMessage = "Річний звіт: потрібна пара семестрів (1+2, 3+4, ...).";
            return false;
        }

        try {
            std::string reportFileName;
            YearlyReport report = generateAndSaveYearlyReport(year, reportFileName);

            char buffer[256] = {};
            std::snprintf(
                buffer,
                sizeof(buffer),
                "Річний звіт за %d рік сформовано. GPA: %.2f. Статус: %s.",
                report.getAcademicYear(),
                report.getAnnualGPA(),
                report.getCanProceed() ? "перехід дозволено" : "є борги"
            );

            statusMessage = std::string(buffer) + " Файл: " + reportFileName;
            return true;
        } catch (const std::exception&) {
            statusMessage = "Річний звіт: помилка створення файлу у папці YearlyReports.";
            return false;
        }
    }

    YearlyReport endYear(int year) {
        if (year <= 0) {
            throw std::invalid_argument("Academic year must be positive.");
        }

        const int firstSemesterOfYear = year * 2 - 1;
        const int secondSemesterOfYear = year * 2;

        const int latestReportYear = getLatestReportableYear();

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
