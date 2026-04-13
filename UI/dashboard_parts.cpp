#include "dashboard_parts_internal.h"
#include "imgui.h"
#include <algorithm>
#include <filesystem>
#include <string>
#include <cstdio>
#include <exception>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <shellapi.h>
#endif
#include "subject.h"
#include "assessments.h"

namespace UI {
namespace DashboardParts {
namespace Detail {

std::string NormalizeSaveName(const char* rawName) {
    std::string fileName = rawName != nullptr ? rawName : "";

    const auto first = fileName.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }

    const auto last = fileName.find_last_not_of(" \t\n\r");
    fileName = fileName.substr(first, last - first + 1);

    if (fileName.size() < 4 || fileName.substr(fileName.size() - 4) != ".dat") {
        fileName += ".dat";
    }

    return fileName;
}

std::string BuildSemesterSaveName(int currentSemester) {
    const int normalizedSemester = (currentSemester > 0) ? currentSemester : 1;
    const int year = (normalizedSemester + 1) / 2;

    return "semester_" + std::to_string(normalizedSemester) + "_year_" + std::to_string(year) + "_save.dat";
}

void RefreshSaves(AppState& appState) {
    appState.availableSaves = appState.system.getAvailableSaves();
    std::sort(appState.availableSaves.begin(), appState.availableSaves.end());

    if (appState.availableSaves.empty()) {
        appState.selectedSaveIndex = -1;
        return;
    }

    const std::string latestSave = appState.system.getLatestSaveName();
    if (!latestSave.empty()) {
        for (size_t i = 0; i < appState.availableSaves.size(); ++i) {
            if (appState.availableSaves[i] == latestSave) {
                appState.selectedSaveIndex = static_cast<int>(i);
                return;
            }
        }
    }

    if (appState.selectedSaveIndex < 0 || appState.selectedSaveIndex >= static_cast<int>(appState.availableSaves.size())) {
        appState.selectedSaveIndex = 0;
    }
}

void SetSystemMessage(AppState& appState, const std::string& message) {
    std::snprintf(appState.systemMessage, sizeof(appState.systemMessage), "%s", message.c_str());
    appState.showSystemMessage = true;
}

bool OpenYearlyReportsFolder(std::string& errorMessage) {
    try {
        const std::filesystem::path folderPath = std::filesystem::absolute("YearlyReports");
        std::filesystem::create_directories(folderPath);

#ifdef _WIN32
        const std::wstring widePath = folderPath.wstring();
        const HINSTANCE result = ShellExecuteW(nullptr, L"open", widePath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        if (reinterpret_cast<std::intptr_t>(result) <= 32) {
            errorMessage = "Не вдалося відкрити папку YearlyReports.";
            return false;
        }

        return true;
#else
        errorMessage = "Відкриття папки підтримується лише на Windows.";
        return false;
#endif
    } catch (const std::exception&) {
        errorMessage = "Не вдалося підготувати папку YearlyReports.";
        return false;
    }
}

std::string BuildYearlyReportMessage(const YearlyReport& report) {
    char buffer[256] = {};
    std::snprintf(
        buffer,
        sizeof(buffer),
        "Річний звіт за %d рік сформовано. GPA: %.2f. Статус: %s.",
        report.getAcademicYear(),
        report.getAnnualGPA(),
        report.getCanProceed() ? "перехід дозволено" : "є борги"
    );
    return std::string(buffer);
}

const Assessments* FindAssessmentByType(const Subject* subject, AssessmentType type) {
    if (subject == nullptr) {
        return nullptr;
    }

    for (const Assessments* assessment : subject->GetAssessments()) {
        if (assessment->getType() == type) {
            return assessment;
        }
    }

    return nullptr;
}

void DrawAssessmentScoreCell(
    AppState& appState,
    const Subject* subject,
    AssessmentType type,
    ScaleType targetScale
) {
    const Assessments* assessment = FindAssessmentByType(subject, type);
    if (assessment == nullptr || !assessment->hasGrades()) {
        ImGui::TextDisabled("-");
        return;
    }

    const double convertedScore = appState.uiConverter.convert(
        assessment->getCurrentScore(),
        assessment->getScale(),
        targetScale
    );
    ImGui::Text("%.1f", convertedScore);
}

}

std::vector<Subject*> BuildSortedSubjects(AppState& appState) {
    std::vector<Subject*> sortedSubjects = appState.system.getGradebook().getSubjects();

    for (Subject* subj : sortedSubjects) {
        appState.pm.update(subj);
    }

    std::sort(sortedSubjects.begin(), sortedSubjects.end(), [&appState](Subject* a, Subject* b) {
        return appState.pm.getPriorityForSubject(a) > appState.pm.getPriorityForSubject(b);
    });

    return sortedSubjects;
}

} 
}
