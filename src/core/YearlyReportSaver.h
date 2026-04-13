#ifndef YEARLYREPORTSAVER_H
#define YEARLYREPORTSAVER_H

#include "YearlyReport.h"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
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
            + "_report.xls";

        const std::filesystem::path reportsPath = std::filesystem::path(directory) / fileName;
        std::filesystem::create_directories(reportsPath.parent_path());

        std::ofstream outFile(reportsPath, std::ios::binary);
        if (!outFile.is_open()) {
            throw std::logic_error("Не вдалося створити файл річного звіту у папці YearlyReports.");
        }

        auto escapeXml = [](const std::string& value) {
            std::string escaped;
            escaped.reserve(value.size());
            for (char c : value) {
                switch (c) {
                case '&': escaped += "&amp;"; break;
                case '<': escaped += "&lt;"; break;
                case '>': escaped += "&gt;"; break;
                case '\"': escaped += "&quot;"; break;
                case '\'': escaped += "&apos;"; break;
                default: escaped += c; break;
                }
            }
            return escaped;
        };

        std::ostringstream gpaStream;
        gpaStream << std::fixed << std::setprecision(2) << report.getAnnualGPA();

        outFile << "\xEF\xBB\xBF";
        outFile << "<?xml version=\"1.0\"?>\n";
        outFile << "<?mso-application progid=\"Excel.Sheet\"?>\n";
        outFile << "<Workbook xmlns=\"urn:schemas-microsoft-com:office:spreadsheet\"\n";
        outFile << " xmlns:o=\"urn:schemas-microsoft-com:office:office\"\n";
        outFile << " xmlns:x=\"urn:schemas-microsoft-com:office:excel\"\n";
        outFile << " xmlns:ss=\"urn:schemas-microsoft-com:office:spreadsheet\">\n";
        outFile << "  <Worksheet ss:Name=\"Yearly Report\">\n";
        outFile << "    <Table>\n";
        outFile << "      <Row>\n";
        outFile << "        <Cell><Data ss:Type=\"String\">Рік</Data></Cell>\n";
        outFile << "        <Cell><Data ss:Type=\"String\">Семестр 1</Data></Cell>\n";
        outFile << "        <Cell><Data ss:Type=\"String\">Семестр 2</Data></Cell>\n";
        outFile << "        <Cell><Data ss:Type=\"String\">GPA</Data></Cell>\n";
        outFile << "        <Cell><Data ss:Type=\"String\">Статус</Data></Cell>\n";
        outFile << "      </Row>\n";

        outFile << "      <Row>\n";
        outFile << "        <Cell><Data ss:Type=\"Number\">" << report.getAcademicYear() << "</Data></Cell>\n";
        outFile << "        <Cell><Data ss:Type=\"Number\">" << firstSemesterOfYear << "</Data></Cell>\n";
        outFile << "        <Cell><Data ss:Type=\"Number\">" << secondSemesterOfYear << "</Data></Cell>\n";
        outFile << "        <Cell><Data ss:Type=\"Number\">" << gpaStream.str() << "</Data></Cell>\n";
        outFile << "        <Cell><Data ss:Type=\"String\">"
                << (report.getCanProceed() ? "перехід дозволено" : "є борги")
                << "</Data></Cell>\n";
        outFile << "      </Row>\n";

        outFile << "      <Row/>\n";
        outFile << "      <Row>\n";
        outFile << "        <Cell><Data ss:Type=\"String\">Предмет</Data></Cell>\n";
        outFile << "        <Cell><Data ss:Type=\"String\">Підсумковий бал </Data></Cell>\n";
        outFile << "      </Row>\n";

        for (const SubjectResult& subjectResult : report.getResults()) {
            std::ostringstream scoreStream;
            scoreStream << std::fixed << std::setprecision(2) << subjectResult.finalScore;

            outFile << "      <Row>\n";
            outFile << "        <Cell><Data ss:Type=\"String\">"
                    << escapeXml(subjectResult.subjectName)
                    << "</Data></Cell>\n";
            outFile << "        <Cell><Data ss:Type=\"Number\">"
                    << scoreStream.str()
                    << "</Data></Cell>\n";
            outFile << "      </Row>\n";
        }

        outFile << "    </Table>\n";
        outFile << "  </Worksheet>\n";
        outFile << "</Workbook>\n";

        return fileName;
    }
};

#endif