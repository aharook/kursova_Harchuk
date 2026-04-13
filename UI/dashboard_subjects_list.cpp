#include "dashboard_parts.h"
#include "imgui.h"
#include <string>
#include <vector>

namespace UI {
namespace DashboardParts {

void DrawSubjectsList(AppState& appState, const std::vector<Subject*>& sortedSubjects) {
    ImGui::TextDisabled("СПИСОК ВАШИХ ПРЕДМЕТІВ");
    ImGui::Separator();

    ImGui::BeginChild("SubjectsList", ImVec2(0, ImGui::GetContentRegionAvail().y - 45), true);
    for (size_t i = 0; i < sortedSubjects.size(); ++i) {
        Subject* subj = sortedSubjects[i];
        const int prio = appState.pm.getPriorityForSubject(subj);
        const bool isSelected = (appState.selectedSubject == subj);

        ImGui::PushID(static_cast<int>(i));

        std::string itemLabel = subj->Getname();
        if (subj->hasCustomUsersPriority()) {
            itemLabel += " *";
        }

        if (ImGui::Selectable(itemLabel.c_str(), isSelected, 0, ImVec2(0, 30))) {
            appState.selectedSubject = isSelected ? nullptr : subj;
        }

        ImGui::Indent(14.0f);
        ImGui::TextDisabled("Пріоритет: %d", prio);
        if (subj->hasPendingBlockers()) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), "Борг");
        }
        ImGui::Unindent(14.0f);

        ImGui::Separator();
        ImGui::PopID();
    }
    ImGui::EndChild();

    ImGui::Spacing();
    if (ImGui::Button("+ Створити предмет", ImVec2(-1, 35))) {
        ImGui::OpenPopup("Створити предмет");
    }
}

} // namespace DashboardParts
} // namespace UI
