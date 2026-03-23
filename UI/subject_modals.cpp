#include "subject_modals.h"
#include "imgui.h"
#include <string>
#include <cctype>
#include <cstring>
#include "subject.h"
#include "assessments.h"

namespace UI {

void DrawAddSubjectModal(AppState& state) {
    if (ImGui::BeginPopupModal("Додати предмет", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("Назва предмета", state.newSubjName, IM_ARRAYSIZE(state.newSubjName));

        ImGui::Separator();
        ImGui::Text("Система оцінювання:");
        const char* scaleTypes[] = { "12-бальна", "100-бальна", "5-бальна", "10-бальна" };
        ImGui::Combo("##scale", &state.newSubjScale, scaleTypes, IM_ARRAYSIZE(scaleTypes));

        ImGui::Separator();
        ImGui::Text("Складові оцінки:");
        ImGui::Checkbox("Звичайні завдання", &state.hasRegular);
        ImGui::Checkbox("Курсова робота", &state.hasCoursework);
        ImGui::Checkbox("Практика", &state.hasPractice);
        ImGui::Checkbox("Сесія (Екзамен/Залік)", &state.hasExam);

        ImGui::Separator();

        bool hasName = false;
        for (size_t i = 0; i < strlen(state.newSubjName); i++) {
            if (!std::isspace(static_cast<unsigned char>(state.newSubjName[i]))) {
                hasName = true;
                break;
            }
        }

        bool hasAnyAssessment = state.hasRegular || state.hasCoursework || state.hasPractice || state.hasExam;
        bool canSave = hasAnyAssessment && hasName;

        if (!canSave) {
            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Введіть реальну назву та оберіть тип завдання!");
        } else {
            ImGui::Text(" ");
        }

        ImGui::BeginDisabled(!canSave);
        if (ImGui::Button("Зберегти", ImVec2(120, 0))) {
            Subject* newSubj = new Subject(std::string(state.newSubjName), state.system.getCurrentSemester(), false);

            ScaleType scale = ScaleType::TwelvePoint;
            if (state.newSubjScale == 1) scale = ScaleType::Accumulative;
            else if (state.newSubjScale == 2) scale = ScaleType::FivePoint;
            else if (state.newSubjScale == 3) scale = ScaleType::TenPoint;

            if (state.hasRegular)    newSubj->addAssessment(AssessmentFactory::createRegular(scale));
            if (state.hasCoursework) newSubj->addAssessment(AssessmentFactory::createCoursework(scale));
            if (state.hasPractice)   newSubj->addAssessment(AssessmentFactory::createPractice(scale));
            if (state.hasExam)       newSubj->addAssessment(AssessmentFactory::createExam(scale));

            state.system.addSubjectToCurrentSemester(newSubj);
            state.selectedSubject = newSubj;

            memset(state.newSubjName, 0, sizeof(state.newSubjName));
            state.hasRegular = true; state.hasCoursework = false; state.hasPractice = false; state.hasExam = true;

            ImGui::CloseCurrentPopup();
        }
        ImGui::EndDisabled();

        ImGui::SameLine();
        if (ImGui::Button("Скасувати", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void DrawEditSubjectModal(AppState& state) {
    if (state.openEditSubjectModal) {
        ImGui::OpenPopup("Редагувати предмет");
        state.openEditSubjectModal = false;
    }

    if (ImGui::BeginPopupModal("Редагувати предмет", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("Нова назва", state.editSubjName, IM_ARRAYSIZE(state.editSubjName));

        bool hasName = false;
        for (size_t i = 0; i < strlen(state.editSubjName); i++) {
            if (!std::isspace(static_cast<unsigned char>(state.editSubjName[i]))) { hasName = true; break; }
        }

        ImGui::Separator();
        ImGui::BeginDisabled(!hasName);
        if (ImGui::Button("Зберегти", ImVec2(120, 0))) {
            if (state.selectedSubject) {
                state.selectedSubject->SetName(std::string(state.editSubjName));
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndDisabled();

        ImGui::SameLine();
        if (ImGui::Button("Скасувати", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

}
