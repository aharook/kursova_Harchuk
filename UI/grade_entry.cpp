#include "grade_entry.h"
#include "imgui.h"
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>

namespace UI {

// Фільтр: блокує введення БУДЬ-ЧОГО, крім цифр, пробілів та ком
static int GradeInputFilter(ImGuiInputTextCallbackData* data) {
    if (data->EventChar >= '0' && data->EventChar <= '9') return 0; // Дозволяємо цифри
    if (data->EventChar == ' ' || data->EventChar == ',') return 0; // Дозволяємо роздільники
    return 1; // Все інше - ігноруємо і не даємо ввести
}

void DrawAddGradeModal(AppState& state) {
    if (state.openGradeModal) {
        ImGui::OpenPopup("Введення оцінки");
        state.openGradeModal = false;
        memset(state.newGradesBuffer, 0, sizeof(state.newGradesBuffer));
        state.showGradeError = false;  
        state.gradeErrorMessage = "";
    }

    if (ImGui::BeginPopupModal("Введення оцінки", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        
        AssessmentType type = state.selectedAssessmentForGrade->getType();
        int maxLimit = static_cast<int>(state.selectedAssessmentForGrade->getMaxAllowedGrade());
        bool isSingleGrade = (type == AssessmentType::EXAM || type == AssessmentType::COURSEWORK || type == AssessmentType::PRACTICE);

        if (isSingleGrade) {
            ImGui::Text("Введіть ОДНУ оцінку (максимум %d):", maxLimit);
            ImGui::TextDisabled("Якщо оцінка вже є, вона буде перезаписана.");
        } else {
            ImGui::Text("Введіть бали через пробіл (максимум %d):", maxLimit);
        }
        
        // ДОДАНО ФІЛЬТР КЛАВІАТУРИ: ImGuiInputTextFlags_CallbackCharFilter
        ImGui::InputText("##grade", state.newGradesBuffer, IM_ARRAYSIZE(state.newGradesBuffer), ImGuiInputTextFlags_CallbackCharFilter, GradeInputFilter);

        if (state.showGradeError) {
            ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "%s", state.gradeErrorMessage.c_str());
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
                    int g = std::stoi(token); // Намагаємось перетворити текст у число
                    if (g > maxLimit) exceedLimit = true;
                    parsedGrades.push_back(g);
                } catch (...) {
                    // Якщо число занадто велике і не влазить в int (напр. 999999999999)
                    isValid = false; 
                }
            }

            if (!isValid || parsedGrades.empty()) {
                state.showGradeError = true;
                state.gradeErrorMessage = "Помилка: введіть коректні числа (не надто великі)!";
            } else if (exceedLimit) {
                state.showGradeError = true;
                state.gradeErrorMessage = "Помилка: бал не може перевищувати " + std::to_string(maxLimit) + "!";
            } else if (isSingleGrade && parsedGrades.size() > 1) {
                state.showGradeError = true;
                state.gradeErrorMessage = "Помилка: сюди можна ввести лише 1 оцінку!";
            } else {
                if (state.selectedAssessmentForGrade != nullptr) {
                    if (isSingleGrade) {
                        state.selectedAssessmentForGrade->clearGrades();
                    }
                    for (int g : parsedGrades) {
                        state.selectedAssessmentForGrade->addGrade(static_cast<double>(g));
                    }
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

} // namespace UI