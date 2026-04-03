#ifndef YEARLYREPORTSAVER_H
#define YEARLYREPORTSAVER_H

#include "YearlyReport.h"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <string>

class YearlyReportSaver {
public:
    static std::string save(const YearlyReport& report, int year, const std::string& directory = "YearlyReports") {
        const int firstSemesterOfYear = year * 2 - 1;
        const int secondSemesterOfYear = year * 2;
        const std::string fileName = "year_" + std::to_string(year)
            + "_semesters_" + std::to_string(firstSemesterOfYear)
            + "_" + std::to_string(secondSemesterOfYear)
            + "_report.csv";

        const std::filesystem::path reportsPath = std::filesystem::path(directory) / fileName;
        std::filesystem::create_directories(reportsPath.parent_path());

        std::ofstream outFile(reportsPath, std::ios::binary);
        if (!outFile.is_open()) {
            throw std::logic_error("Не вдалося створити файл річного звіту у папці YearlyReports.");
        }

        outFile << "\xEF\xBB\xBF";
        outFile << "Рік,Семестр 1,Семестр 2,GPA,Статус\n";
        outFile << report.getAcademicYear() << ","
                << firstSemesterOfYear << ","
                << secondSemesterOfYear << ","
                << std::fixed << std::setprecision(2) << report.getAnnualGPA() << ","
                << (report.getCanProceed() ? "перехід дозволено" : "є борги") << "\n\n";

        outFile << "Предмет,Підсумковий бал (100)\n";
        for (const SubjectResult& subjectResult : report.getResults()) {
            outFile << subjectResult.subjectName << ","
                    << std::fixed << std::setprecision(2) << subjectResult.finalScore << "\n";
        }

        return fileName;
    }
};

#endif