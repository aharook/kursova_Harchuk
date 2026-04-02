#include "dashboard_parts.h"
#include "imgui.h"
#include <algorithm>
#include <string>
#include <map>
#include <cstdio>
#include <cstring>
#include "subject.h"
#include "assessments.h"

namespace UI {
namespace DashboardParts {

static std::string NormalizeSaveName(const char* rawName) {
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

static std::string BuildSemesterSaveName(int currentSemester) {
    const int normalizedSemester = (currentSemester > 0) ? currentSemester : 1;
    const int year = (normalizedSemester + 1) / 2;
    const int semesterInYear = ((normalizedSemester - 1) % 2) + 1;

    return "year_" + std::to_string(year) + "_sem_" + std::to_string(semesterInYear) + ".dat";
}

static void RefreshSaves(AppState& appState) {
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

static void SetSystemMessage(AppState& appState, const std::string& message) {
    std::snprintf(appState.systemMessage, sizeof(appState.systemMessage), "%s", message.c_str());
    appState.showSystemMessage = true;
}

static std::string BuildYearlyReportMessage(const YearlyReport& report) {
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

bool DrawTopPanel(AppState& appState) {
    if (!appState.saveListInitialized) {
        RefreshSaves(appState);
        appState.saveListInitialized = true;
    }

    static std::string pendingOverwriteSaveName;

    ImGui::BeginChild("TopPanel", ImVec2(0, 86), true);

    ImGui::Text("Семестр: %d", appState.system.getCurrentSemester());
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - 110.0f + ImGui::GetCursorPosX());
    ImGui::Checkbox("Темна тема##theme", &appState.isDarkTheme);

    ImGui::Spacing();

    if (ImGui::Button("Зберегти", ImVec2(110, 28))) {
        ImGui::OpenPopup("Зберегти як");
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(240);
    const char* selectedSaveLabel = (appState.selectedSaveIndex >= 0 && appState.selectedSaveIndex < static_cast<int>(appState.availableSaves.size()))
        ? appState.availableSaves[appState.selectedSaveIndex].c_str()
        : "(нема збережень)";

    if (ImGui::BeginCombo("##SavePicker", selectedSaveLabel)) {
        for (int i = 0; i < static_cast<int>(appState.availableSaves.size()); ++i) {
            const bool isSelected = appState.selectedSaveIndex == i;
            if (ImGui::Selectable(appState.availableSaves[i].c_str(), isSelected)) {
                appState.selectedSaveIndex = i;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();
    if (ImGui::Button("Завантажити", ImVec2(120, 28))) {
        if (appState.selectedSaveIndex < 0 || appState.selectedSaveIndex >= static_cast<int>(appState.availableSaves.size())) {
            SetSystemMessage(appState, "Помилка: оберіть збереження.");
        } else {
            const std::string selectedSave = appState.availableSaves[appState.selectedSaveIndex];
            const bool loaded = appState.system.loadSystemState(selectedSave);
            if (loaded) {
                appState.selectedSubject = nullptr;
                appState.selectedAssessmentForGrade = nullptr;
                appState.openGradeModal = false;
                appState.openEditSubjectModal = false;
                appState.dataReloadedThisFrame = true;
                SetSystemMessage(appState, "Збереження завантажено.");
            } else {
                SetSystemMessage(appState, "Помилка завантаження.");
            }
        }
    }

    const bool canEndSemester = appState.system.canEndCurrentSemester();
    const bool hasDebts = !canEndSemester;
    if (hasDebts) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
    }

    ImGui::BeginDisabled(!canEndSemester);
    ImGui::SameLine();
    if (ImGui::Button("Закінчити семестр", ImVec2(160, 28))) {
        if (appState.system.endSemester()) {
            std::string targetSaveName;
            if (appState.selectedSaveIndex >= 0 && appState.selectedSaveIndex < static_cast<int>(appState.availableSaves.size())) {
                targetSaveName = appState.availableSaves[appState.selectedSaveIndex];
            } else {
                targetSaveName = "autosave.dat";
            }

            appState.system.saveSystemState(targetSaveName);
            RefreshSaves(appState);
            appState.selectedSubject = nullptr;
            SetSystemMessage(appState, "Семестр завершено і збережено у файл.");
        }
    }
    ImGui::EndDisabled();
    ImGui::PopStyleColor(2);

    ImGui::SameLine();
    if (ImGui::Button("Закінчити рік", ImVec2(140, 28))) {
        const int completedYears = (appState.system.getCurrentSemester() - 1) / 2;
        if (completedYears <= 0) {
            SetSystemMessage(appState, "Річний звіт: завершіть 2 семестри.");
        } else {
            YearlyReport report = appState.system.endYear(completedYears);
            SetSystemMessage(appState, BuildYearlyReportMessage(report));
        }
    }

    if (ImGui::BeginPopupModal("Зберегти як", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::SetNextItemWidth(280);
        ImGui::InputText("Назва файлу", appState.saveFileName, IM_ARRAYSIZE(appState.saveFileName));
        ImGui::Spacing();

        if (ImGui::Button("Підтвердити", ImVec2(120, 0))) {
            const std::string requestedSaveName = NormalizeSaveName(appState.saveFileName);
            if (requestedSaveName.empty()) {
                SetSystemMessage(appState, "Помилка: введіть назву збереження.");
            } else {
                const bool alreadyExists = std::find(appState.availableSaves.begin(), appState.availableSaves.end(), requestedSaveName) != appState.availableSaves.end();
                if (alreadyExists) {
                    pendingOverwriteSaveName = requestedSaveName;
                    ImGui::OpenPopup("Підтвердження перезапису");
                } else {
                    appState.system.saveSystemState(requestedSaveName);
                    RefreshSaves(appState);
                    SetSystemMessage(appState, "Нове збереження створено.");
                    ImGui::CloseCurrentPopup();
                }
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Скасувати", ImVec2(100, 0))) {
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::BeginPopupModal("Підтвердження перезапису", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::TextWrapped("Збереження з такою назвою вже існує:\n%s\nПерезаписати?", pendingOverwriteSaveName.c_str());
            ImGui::Spacing();

            if (ImGui::Button("Так, перезаписати", ImVec2(150, 0))) {
                appState.system.saveSystemState(pendingOverwriteSaveName);
                RefreshSaves(appState);
                SetSystemMessage(appState, "Збереження перезаписано.");
                pendingOverwriteSaveName.clear();
                ImGui::CloseCurrentPopup();
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Ні", ImVec2(80, 0))) {
                pendingOverwriteSaveName.clear();
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::EndPopup();
    }

    if (appState.showSystemMessage) {
        ImGui::Spacing();
        ImGui::TextWrapped("%s", appState.systemMessage);
    }

    ImGui::EndChild();
    return hasDebts;
}

void DrawSubjectsList(AppState& appState, const std::vector<Subject*>& sortedSubjects) {
    ImGui::TextDisabled("СПИСОК ВАШИХ ПРЕДМЕТІВ");
    ImGui::Separator();

    ImGui::BeginChild("SubjectsList", ImVec2(0, ImGui::GetContentRegionAvail().y - 45), true);
    for (size_t i = 0; i < sortedSubjects.size(); ++i) {
        Subject* subj = sortedSubjects[i];
        int prio = appState.pm.getPriorityForSubject(subj);
        bool isSelected = (appState.selectedSubject == subj);

        ImGui::PushID(static_cast<int>(i));

        ImVec4 textColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        ImVec4 bgColor = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);

        if (subj->hasPendingBlockers()) {
            textColor = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
            bgColor = ImVec4(1.0f, 0.9f, 0.9f, 0.3f);
        } else if (prio >= 2000) {
            textColor = ImVec4(1.0f, 0.4f, 0.0f, 1.0f);
            bgColor = ImVec4(1.0f, 0.95f, 0.85f, 0.2f);
        } else if (prio >= 500) {
            textColor = ImVec4(0.8f, 0.6f, 0.0f, 1.0f);
            bgColor = ImVec4(1.0f, 0.98f, 0.9f, 0.1f);
        } else {
            textColor = ImVec4(0.2f, 0.6f, 0.2f, 1.0f);
            bgColor = ImVec4(0.9f, 1.0f, 0.9f, 0.15f);
        }

        ImGui::PushStyleColor(ImGuiCol_Text, textColor);

        std::string starIndicator = subj->hasCustomUsersPriority() ? " ?" : "";
        std::string itemLabel = subj->Getname() + starIndicator;

        if (ImGui::Selectable(itemLabel.c_str(), isSelected, 0, ImVec2(0, 36))) {
            if (isSelected) appState.selectedSubject = nullptr;
            else appState.selectedSubject = subj;
        }

        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Пріоритет: %d", prio);
        }

        ImGui::PopStyleColor();
        ImGui::Separator();
        ImGui::PopID();
    }
    ImGui::EndChild();

    ImGui::Spacing();
    if (ImGui::Button("+ Створити предмет", ImVec2(-1, 35))) {
        ImGui::OpenPopup("Створити предмет");
    }
}

bool DrawSelectedSubjectDetails(AppState& appState) {
    Subject* subj = appState.selectedSubject;
    if (subj == nullptr) {
        return false;
    }

    ImGui::Text("Предмет: %s", subj->Getname().c_str());

    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 280);
    if (ImGui::Button("Редагувати назву", ImVec2(135, 0))) {
        appState.openEditSubjectModal = true;
        snprintf(appState.editSubjName, sizeof(appState.editSubjName), "%s", subj->Getname().c_str());
    }
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));

    if (ImGui::Button("Видалити предмет", ImVec2(135, 0))) {
        appState.system.getGradebook().removeSubject(subj);
        appState.selectedSubject = nullptr;
    }
    ImGui::PopStyleColor(2);

    if (appState.selectedSubject == nullptr) {
        return true;
    }

    ImGui::Separator();

    if (subj->hasPendingBlockers()) {
        ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "УВАГА: Є нездана сесія/завдання");
    } else {
        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Предмет закривається нормально.");
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::TextDisabled("ДЕТАЛІ ОЦІНЮВАННЯ:");
    ImGui::Spacing();

    if (ImGui::BeginTable("AssessmentsTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("Тип завдання");
        ImGui::TableSetupColumn("Всі оцінки");
        ImGui::TableSetupColumn("Середній");
        ImGui::TableSetupColumn("Статус");
        ImGui::TableSetupColumn("Дія");
        ImGui::TableHeadersRow();

        const auto& assessmentsList = subj->GetAssessments();

        for (size_t i = 0; i < assessmentsList.size(); ++i) {
            Assessments* assessment = assessmentsList[i];
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            switch (assessment->getType()) {
                case AssessmentType::EXAM: ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "Екзамен / Залік"); break;
                case AssessmentType::COURSEWORK: ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.4f, 1.0f), "Курсова робота"); break;
                case AssessmentType::PRACTICE: ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "Практика"); break;
                case AssessmentType::REGULAR: default: ImGui::Text("Звичайне завдання"); break;
            }

            ImGui::TableSetColumnIndex(1);
            std::vector<double> grades = assessment->getGrades();
            if (grades.empty()) {
                ImGui::TextDisabled("Немає");
            } else {
                std::string gradesStr;
                for (size_t g = 0; g < grades.size(); ++g) {
                    gradesStr += std::to_string(static_cast<int>(grades[g]));
                    if (g < grades.size() - 1) gradesStr += ", ";
                }
                ImGui::TextWrapped("%s", gradesStr.c_str());
            }

            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%.1f", assessment->getCurrentScore());

            ImGui::TableSetColumnIndex(3);
            if (assessment->getIsBlocker()) {
                if (assessment->isPassed()) ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Здано");
                else ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "БОРГ");
            } else {
                ImGui::Text("-");
            }

            ImGui::TableSetColumnIndex(4);
            ImGui::PushID(static_cast<int>(i));
            if (ImGui::Button("Оцінки")) {
                appState.selectedAssessmentForGrade = assessment;
                appState.openGradeModal = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Очистити")) {
                assessment->clearGrades();
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }

    return false;
}

void DrawSemesterOverview(AppState& appState, const std::vector<Subject*>& sortedSubjects, bool hasDebts) {
    ImGui::TextDisabled("ЗАГАЛЬНА СТАТИСТИКА ПО СЕМЕСТРУ (Оберіть предмет зліва для деталей)");
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    ImGui::SeparatorText("Класична система оцінювання (з переведенням шкал)");

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Показувати оцінки у:");
    ImGui::SameLine();
    const char* displayScales = "12-бальній\0 5-бальній\0 10-бальній\0";
    ImGui::SetNextItemWidth(140);
    ImGui::Combo("##displayScale", &appState.selectedDisplayScale, displayScales, 3);
    ImGui::Spacing();
    std::map<std::string, double> actualAverages = appState.system.getGradebook().getActualAverages();

    if (ImGui::BeginTable("StandardSubjects", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("Предмет");
        ImGui::TableSetupColumn("Підсумкова оцінка");
        ImGui::TableSetupColumn("Статус");
        ImGui::TableHeadersRow();

        for (Subject* sub : sortedSubjects) {
            ScaleType nativeScale = appState.system.getGradebook().getSubjectScale(sub->getLinkId());
            if (nativeScale == ScaleType::Accumulative) continue;

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", sub->Getname().c_str());

            ImGui::TableSetColumnIndex(1);

            double nativeScore = actualAverages[sub->getLinkId()];

            ScaleType targetScale = ScaleType::TwelvePoint;
            std::string scaleSuffix = " (12-бальна)";

            if (appState.selectedDisplayScale == 1) { targetScale = ScaleType::FivePoint; scaleSuffix = " (5-бальна)"; }
            else if (appState.selectedDisplayScale == 2) { targetScale = ScaleType::TenPoint; scaleSuffix = " (10-бальна)"; }

            double convertedScore = appState.uiConverter.convert(nativeScore, nativeScale, targetScale);

            ImGui::Text("%.1f%s", convertedScore, scaleSuffix.c_str());

            ImGui::TableSetColumnIndex(2);
            if (!sub->hasPendingBlockers()) {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Здано");
            } else {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Борг");
            }
        }
        ImGui::EndTable();
    }

    ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.7f, 0.8f, 1.0f));
    ImGui::SeparatorText("Накопичувальна система");
    ImGui::PopStyleColor();

    if (ImGui::BeginTable("AccumulativeSubjects", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("Предмет");
        ImGui::TableSetupColumn("Накопичено балів");
        ImGui::TableSetupColumn("Статус");
        ImGui::TableHeadersRow();

        for (Subject* sub : sortedSubjects) {
            ScaleType nativeScale = appState.system.getGradebook().getSubjectScale(sub->getLinkId());
            if (nativeScale != ScaleType::Accumulative) continue;

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", sub->Getname().c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.1f / 100.0", actualAverages[sub->getLinkId()]);

            ImGui::TableSetColumnIndex(2);
            if (!sub->hasPendingBlockers()) ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Допущено");
            else ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Борг");
        }
        ImGui::EndTable();
    }

    ImGui::Spacing(); ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (!sortedSubjects.empty()) {
        if (hasDebts) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
            ImGui::Text("Семестр не закрито: у вас є борги або незавершені предмети!");
            ImGui::PopStyleColor();
        } else {
            ScaleType targetScale = ScaleType::TwelvePoint;
            if (appState.selectedDisplayScale == 1) targetScale = ScaleType::FivePoint;
            else if (appState.selectedDisplayScale == 2) targetScale = ScaleType::TenPoint;

            double totalScore = 0.0;
            int includedSubjects = 0;
            for (Subject* sub : sortedSubjects) {
                bool hasRegularGrades = false;
                const auto& assessments = sub->GetAssessments();
                for (const Assessments* task : assessments) {
                    if (task->getType() == AssessmentType::REGULAR && task->hasGrades()) {
                        hasRegularGrades = true;
                        break;
                    }
                }

                if (!hasRegularGrades) {
                    continue;
                }

                ScaleType nativeScale = appState.system.getGradebook().getSubjectScale(sub->getLinkId());
                double convertedScore = appState.uiConverter.convert(actualAverages[sub->getLinkId()], nativeScale, targetScale);
                totalScore += convertedScore;
                includedSubjects++;
            }

            double averageScore = (includedSubjects > 0) ? (totalScore / includedSubjects) : 0.0;

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));
            ImGui::Text(" Семестр успішно закрито!");
            ImGui::SetWindowFontScale(1.2f);
            if (includedSubjects > 0) {
                ImGui::Text("Ваш загальний середній бал за семестр: %.2f", averageScore);
            } else {
                ImGui::Text("Немає regular-оцінок для розрахунку середнього.");
            }
            ImGui::SetWindowFontScale(1.0f);
            ImGui::PopStyleColor();
        }
    }
}

}
}
