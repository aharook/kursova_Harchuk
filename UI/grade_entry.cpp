#include "grade_entry.h"
#include "imgui.h"
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>

namespace UI {

static int GradeInputFilter(ImGuiInputTextCallbackData* data) {
    if (data->EventChar >= '0' && data->EventChar <= '9') return 0;
    if (data->EventChar == ' ' || data->EventChar == ',') return 0;
    return 1;
}

void DrawAddGradeModal(AppState& state) {
    if (state.openGradeModal) {
        ImGui::OpenPopup("Введення оцінки");
        state.openGradeModal = false;
        memset(state.newGradesBuffer, 0, sizeof(state.newGradesBuffer));
        state.showGradeError = false;  
        strcpy(state.gradeErrorMessage, "");
        state.overwriteGradesOnSave = false;
    }

    if (ImGui::BeginPopupModal("Введення оцінки", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        
        AssessmentType type = state.selectedAssessmentForGrade->getType();
        int maxLimit = static_cast<int>(state.selectedAssessmentForGrade->getMaxAllowedGrade());
        bool isSingleGrade = (type == AssessmentType::EXAM || type == AssessmentType::COURSEWORK || type == AssessmentType::PRACTICE);

        if (isSingleGrade) {
            ImGui::Text("Введіть ОДНУ оцінку (максимум %d):", maxLimit);
            ImGui::TextDisabled("Для цього типу нова оцінка замінює попередню.");
        } else {
            ImGui::Text("Введіть бали через пробіл (максимум %d):", maxLimit);
            ImGui::Checkbox("Перезаписати існуючі оцінки", &state.overwriteGradesOnSave);
        }
        ImGui::InputText("##grade", state.newGradesBuffer, IM_ARRAYSIZE(state.newGradesBuffer), ImGuiInputTextFlags_CallbackCharFilter, GradeInputFilter);

        if (state.showGradeError) {
            ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "%s", state.gradeErrorMessage);
        }

        ImGui::Separator();

        if (ImGui::Button("Зберегти", ImVec2(120, 0))) {
            std::string input(state.newGradesBuffer);
            std::replace(input.begin(), input.end(), ',', ' '); 
            
            std::stringstream ss(input);
            std::string token;
            std::vector<int> parsedGrades;
            bool isValid = true;
            bool exceedLimit = false;

            while (ss >> token) {
                try {
                    int g = std::stoi(token); 
                    if (g > maxLimit) exceedLimit = true;
                    parsedGrades.push_back(g);
                } catch (...) {

                    isValid = false; 
                }
            }

            if (!isValid || parsedGrades.empty()) {
                state.showGradeError = true;
                strcpy(state.gradeErrorMessage, "Помилка: введіть коректні числа (не надто великі)!");
            } else if (exceedLimit) {
                state.showGradeError = true;
                std::string msg = "Помилка: бал не може перевищувати " + std::to_string(maxLimit) + "!";
                strcpy(state.gradeErrorMessage, msg.c_str());
            } else if (isSingleGrade && parsedGrades.size() > 1) {
                state.showGradeError = true;
                strcpy(state.gradeErrorMessage, "Помилка: сюди можна ввести лише 1 оцінку!");
            } else {
                if (state.selectedAssessmentForGrade != nullptr) {
                    std::vector<double> gradesToSave;
                    gradesToSave.reserve(parsedGrades.size());
                    for (int g : parsedGrades) {
                        gradesToSave.push_back(static_cast<double>(g));
                    }
                    state.selectedAssessmentForGrade->saveGrades(gradesToSave, !state.overwriteGradesOnSave);
                }
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Скасувати", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

}
