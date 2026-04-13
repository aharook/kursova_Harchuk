#include "dashboard_parts.h"
#include "imgui.h"
#include <string>
#include <vector>
#include <cstdio>

namespace UI {
namespace DashboardParts {

bool DrawSelectedSubjectDetails(AppState& appState) {
    Subject* subj = appState.selectedSubject;
    if (subj == nullptr) {
        return false;
    }

    ImGui::Text("Предмет: %s", subj->Getname().c_str());

    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 280);
    if (ImGui::Button("Редагувати назву", ImVec2(135, 0))) {
        appState.openEditSubjectModal = true;
        std::snprintf(appState.editSubjName, sizeof(appState.editSubjName), "%s", subj->Getname().c_str());
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

    if (subj->isPassed()) {
        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Предмет закривається нормально.");
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "УВАГА: Є нездані оцінки або борги");
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
                    if (g < grades.size() - 1) {
                        gradesStr += ", ";
                    }
                }
                ImGui::TextWrapped("%s", gradesStr.c_str());
            }

            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%.1f", assessment->getCurrentScore());

            ImGui::TableSetColumnIndex(3);
            if (assessment->getIsBlocker()) {
                if (assessment->isPassed()) {
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Здано");
                } else {
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "БОРГ");
                }
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

} 
} 