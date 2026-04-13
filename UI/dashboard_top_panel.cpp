#include "dashboard_parts_internal.h"
#include "imgui.h"
#include <algorithm>
#include <string>

namespace UI {
namespace DashboardParts {

bool DrawTopPanel(AppState& appState) {
    if (!appState.saveListInitialized) {
        Detail::RefreshSaves(appState);
        appState.saveListInitialized = true;
    }

    static std::string pendingOverwriteSaveName;
    const float topPanelHeight = appState.showSystemMessage ? 130.0f : 104.0f;

    ImGui::BeginChild(
        "TopPanel",
        ImVec2(0, topPanelHeight),
        true,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
    );

    ImGui::Text("Семестр: %d", appState.system.getCurrentSemester());
    ImGui::SameLine();

    const char* themeLabel = "Темна тема";
    const float checkboxWidth = ImGui::CalcTextSize(themeLabel).x + ImGui::GetFrameHeight() + ImGui::GetStyle().ItemInnerSpacing.x;
    const float rightAlignedX = ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - checkboxWidth;
    if (rightAlignedX > ImGui::GetCursorPosX()) {
        ImGui::SetCursorPosX(rightAlignedX);
    }
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
            Detail::SetSystemMessage(appState, "Помилка: оберіть збереження.");
        } else {
            const std::string selectedSave = appState.availableSaves[appState.selectedSaveIndex];
            const bool loaded = appState.system.loadSystemState(selectedSave);
            if (loaded) {
                appState.selectedSubject = nullptr;
                appState.selectedAssessmentForGrade = nullptr;
                appState.openGradeModal = false;
                appState.openEditSubjectModal = false;
                appState.dataReloadedThisFrame = true;
                Detail::SetSystemMessage(appState, "Збереження завантажено.");
            } else {
                Detail::SetSystemMessage(appState, "Помилка завантаження.");
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
        const int semesterToSave = std::max(1, appState.system.getCurrentSemester());
        const std::string targetSaveName = Detail::BuildSemesterSaveName(semesterToSave);
        appState.system.saveSystemState(targetSaveName);

        if (appState.system.endSemester()) {
            Detail::RefreshSaves(appState);
            appState.selectedSubject = nullptr;
            Detail::SetSystemMessage(appState, "Семестр завершено. Збереження семестру створено.");
        }
    }
    ImGui::EndDisabled();
    ImGui::PopStyleColor(2);

    ImGui::SameLine();
    if (ImGui::Button("Закінчити рік", ImVec2(140, 28))) {
        std::string statusMessage;
        const bool reportCreated = appState.system.tryGenerateLatestYearlyReport(statusMessage);
        Detail::SetSystemMessage(appState, statusMessage);
        if (reportCreated) {
            Detail::RefreshSaves(appState);
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Річні звіти", ImVec2(140, 28))) {
        std::string errorMessage;
        if (Detail::OpenYearlyReportsFolder(errorMessage)) {
            Detail::SetSystemMessage(appState, "Відкрито папку YearlyReports.");
        } else {
            Detail::SetSystemMessage(appState, errorMessage);
        }
    }

    if (ImGui::BeginPopupModal("Зберегти як", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::SetNextItemWidth(280);
        ImGui::InputText("Назва файлу", appState.saveFileName, IM_ARRAYSIZE(appState.saveFileName));
        ImGui::Spacing();

        if (ImGui::Button("Підтвердити", ImVec2(120, 0))) {
            const std::string requestedSaveName = Detail::NormalizeSaveName(appState.saveFileName);
            if (requestedSaveName.empty()) {
                Detail::SetSystemMessage(appState, "Помилка: введіть назву збереження.");
            } else {
                const bool alreadyExists = std::find(appState.availableSaves.begin(), appState.availableSaves.end(), requestedSaveName) != appState.availableSaves.end();
                if (alreadyExists) {
                    pendingOverwriteSaveName = requestedSaveName;
                    ImGui::OpenPopup("Підтвердження перезапису");
                } else {
                    appState.system.saveSystemState(requestedSaveName);
                    Detail::RefreshSaves(appState);
                    Detail::SetSystemMessage(appState, "Нове збереження створено.");
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
                Detail::RefreshSaves(appState);
                Detail::SetSystemMessage(appState, "Збереження перезаписано.");
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

} 
}
