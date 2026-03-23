#include "UI.h"
#include "imgui.h"
#include "grade_entry.h"
#include "subject_modals.h"
#include "dashboard_parts.h"

namespace UI {

void DrawDashboard(AppState& appState) {
    std::vector<Subject*> sortedSubjects = DashboardParts::BuildSortedSubjects(appState);

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Dashboard", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    bool hasDebts = DashboardParts::DrawTopPanel(appState);
    if (appState.dataReloadedThisFrame) {
        appState.dataReloadedThisFrame = false;
        ImGui::End();
        return;
    }
    ImGui::Spacing();

    ImGui::Columns(2, "MainLayout");
    ImGui::SetColumnWidth(0, 350.0f);

    DashboardParts::DrawSubjectsList(appState, sortedSubjects);

    ImGui::NextColumn();

    ImGui::BeginChild("SubjectDetailsArea", ImVec2(0, 0), true);

    if (appState.selectedSubject != nullptr) {
        bool deselected = DashboardParts::DrawSelectedSubjectDetails(appState);
        if (deselected) {
            ImGui::EndChild();
            ImGui::NextColumn();
            DrawAddSubjectModal(appState);
            DrawAddGradeModal(appState);
            DrawEditSubjectModal(appState);
            ImGui::Columns(1);
            ImGui::End();
            return;
        }
    } else {
        DashboardParts::DrawSemesterOverview(appState, sortedSubjects, hasDebts);
    }
    ImGui::EndChild();

    DrawAddSubjectModal(appState);
    DrawAddGradeModal(appState);
    DrawEditSubjectModal(appState);

    ImGui::Columns(1);
    ImGui::End();
}

}
