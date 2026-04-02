#include "subject_modals.h"
#include "imgui.h"
#include <string>
#include <cctype>
#include <cstring>
#include "subject.h"
#include "assessments.h"

namespace UI {

void DrawAddSubjectModal(AppState& state) {
    if (ImGui::BeginPopupModal("Створити предмет", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::SetNextItemWidth(300);
        ImGui::InputText("Назва##subj", state.newSubjName, IM_ARRAYSIZE(state.newSubjName));

        ImGui::Spacing();
        ImGui::TextDisabled("Шкала оцінювання:");
        const char* scaleTypes[] = { "100-бальна", "5-бальна", "10-бальна" };
        ImGui::SetNextItemWidth(200);
        ImGui::Combo("##scale", &state.newSubjScale, scaleTypes, IM_ARRAYSIZE(scaleTypes));

        ImGui::Spacing();
        ImGui::TextDisabled("Компоненти оцінювання:");
        ImGui::Checkbox("Завдання##reg", &state.hasRegular);
        ImGui::SameLine(180);
        ImGui::Checkbox("Курсова##cw", &state.hasCoursework);
        ImGui::Checkbox("Практика##pr", &state.hasPractice);
        ImGui::SameLine(180);
        ImGui::Checkbox("Екзамен##ex", &state.hasExam);

        ImGui::Spacing();
        ImGui::Checkbox("Кастомний пріоритет", &state.hasCustomPriority);
        if (state.hasCustomPriority) {
            ImGui::SameLine();
            const char* customPriorityOptions[] = { "100", "200", "300" };
            ImGui::SetNextItemWidth(80);
            ImGui::Combo("##priority", &state.customPriorityIndex, customPriorityOptions, IM_ARRAYSIZE(customPriorityOptions));
        }

        ImGui::Spacing();
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
            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Введіть назву та виберіть компоненти");
        }

        ImGui::Spacing();
        ImGui::BeginDisabled(!canSave);
        if (ImGui::Button("Створити", ImVec2(100, 0))) {
            Subject* newSubj = new Subject(std::string(state.newSubjName), state.system.getCurrentSemester(), false);

            ScaleType scale = ScaleType::Accumulative;
            if (state.newSubjScale == 1) scale = ScaleType::FivePoint;
            else if (state.newSubjScale == 2) scale = ScaleType::TenPoint;

            if (state.hasRegular)    newSubj->addAssessment(AssessmentFactory::createRegular(scale));
            if (state.hasCoursework) newSubj->addAssessment(AssessmentFactory::createCoursework(scale));
            if (state.hasPractice)   newSubj->addAssessment(AssessmentFactory::createPractice(scale));
            if (state.hasExam)       newSubj->addAssessment(AssessmentFactory::createExam(scale));

            if (state.hasCustomPriority) {
                static const int customPriorityValues[] = { 100, 200, 300 };
                newSubj->setUsersPriority(customPriorityValues[state.customPriorityIndex]);
            }

            state.system.addSubjectToCurrentSemester(newSubj);
            state.selectedSubject = newSubj;

            memset(state.newSubjName, 0, sizeof(state.newSubjName));
            state.hasCustomPriority = false;
            state.customPriorityIndex = 0;
            state.hasRegular = true; state.hasCoursework = false; state.hasPractice = false; state.hasExam = true;

            ImGui::CloseCurrentPopup();
        }
        ImGui::EndDisabled();

        ImGui::SameLine();
        if (ImGui::Button("Скасувати", ImVec2(100, 0))) {
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
