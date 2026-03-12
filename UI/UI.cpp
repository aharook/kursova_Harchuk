#include "UI.h"
#include "imgui.h"
#include <string>

namespace UI {

void DrawAddSubjectModal(AppState& state) {
    if (ImGui::BeginPopupModal("Додати предмет", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("Назва предмета", state.newSubjName, IM_ARRAYSIZE(state.newSubjName));

        ImGui::Separator();
        ImGui::Text("Система оцінювання:");
        const char* scaleTypes[] = { "12-бальна", "100-бальна", "5-бальна", "10-бальна" };
        ImGui::Combo("##scale", &state.newSubjScale, scaleTypes, IM_ARRAYSIZE(scaleTypes));

        ImGui::Separator();
        ImGui::Text("Складові оцінки (Асесменти):");
        ImGui::Checkbox("Звичайні завдання", &state.hasRegular);
        ImGui::Checkbox("Курсова робота", &state.hasCoursework);
        ImGui::Checkbox("Практика", &state.hasPractice);
        ImGui::Checkbox("Сесія (Екзамен/Залік)", &state.hasExam);

        ImGui::Separator();

        bool hasAnyAssessment = state.hasRegular || state.hasCoursework || state.hasPractice || state.hasExam;
        bool hasName = strlen(state.newSubjName) > 0;
        bool canSave = hasAnyAssessment && hasName;

        if (!canSave) {
            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Введіть назву та оберіть хоча б 1 тип завдання!");
        } else {
            ImGui::Text(" "); 
        }

        ImGui::BeginDisabled(!canSave);
        if (ImGui::Button("Зберегти", ImVec2(120, 0))) {
            Subject* newSubj = new Subject(std::string(state.newSubjName), 1, false);

            ScaleType scale = ScaleType::TwelvePoint;
            if (state.newSubjScale == 1) scale = ScaleType::Accumulative;
            else if (state.newSubjScale == 2) scale = ScaleType::FivePoint;
            else if (state.newSubjScale == 3) scale = ScaleType::TenPoint; 

            if (state.hasRegular)    newSubj->addAssessment(AssessmentFactory::createRegular(scale));
            if (state.hasCoursework) newSubj->addAssessment(AssessmentFactory::createCoursework(scale)); 
            if (state.hasPractice)   newSubj->addAssessment(AssessmentFactory::createPractice(scale)); 
            if (state.hasExam)       newSubj->addAssessment(AssessmentFactory::createExam(scale));    

            state.subjects.push_back(newSubj); 

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

void DrawAddGradeModal(AppState& state) {
    if (ImGui::BeginPopupModal("Введення оцінки", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Введіть отриманий бал:");
        ImGui::InputFloat("##grade", &state.newGradeValue, 0.5f, 1.0f, "%.1f");

        ImGui::Separator();

        if (ImGui::Button("Зберегти", ImVec2(120, 0))) {
            if (state.selectedAssessmentForGrade != nullptr) {
                state.selectedAssessmentForGrade->addGrade(state.newGradeValue);
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Скасувати", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void DrawDashboard(AppState& appState) {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Dashboard", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::Columns(2, "MainLayout");
    ImGui::SetColumnWidth(0, 350.0f);

    // ==========================================
    // ЛІВА ПАНЕЛЬ
    // ==========================================
    ImGui::Text("Ваші дисципліни");
    ImGui::Separator();
    
    ImGui::BeginChild("SubjectsList", ImVec2(0, ImGui::GetContentRegionAvail().y - 40), false);
    for (int i = 0; i < appState.subjects.size(); ++i) {
        appState.pm.update(appState.subjects[i]);
        int prio = appState.pm.getPriorityForSubject(appState.subjects[i]);

        std::string itemLabel = appState.subjects[i]->Getname() + " (Пріоритет: " + std::to_string(prio) + ")";
        
        if (appState.subjects[i]->hasPendingBlockers()) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
        }

        if (ImGui::Selectable(itemLabel.c_str(), appState.selectedSubjectIndex == i)) {
            appState.selectedSubjectIndex = i;
        }

        if (appState.subjects[i]->hasPendingBlockers()) {
            ImGui::PopStyleColor();
        }
    }
    ImGui::EndChild();

    ImGui::Spacing();
    if (ImGui::Button("+ Створити предмет", ImVec2(-1, 30))) {
        ImGui::OpenPopup("Додати предмет");
    }
    
    ImGui::NextColumn();

    // ==========================================
    // ПРАВА ПАНЕЛЬ
    // ==========================================
    ImGui::BeginChild("SubjectDetailsArea", ImVec2(0, 0), false);
    if (appState.selectedSubjectIndex >= 0 && appState.selectedSubjectIndex < appState.subjects.size()) {
        Subject* subj = appState.subjects[appState.selectedSubjectIndex];
        
        ImGui::Text("Предмет: %s", subj->Getname().c_str());
        ImGui::Separator();
        
        if (subj->hasPendingBlockers()) {
            ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "УВАГА: Є нездана сесія/завдання (Блокувальник)!");
        } else {
            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Предмет закривається нормально.");
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Деталі оцінювання:");
        ImGui::Spacing();

        if (ImGui::BeginTable("AssessmentsTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) {
            ImGui::TableSetupColumn("Тип завдання");
            ImGui::TableSetupColumn("Середній бал");
            ImGui::TableSetupColumn("Блокувальник?");
            ImGui::TableSetupColumn("Дія");
            ImGui::TableHeadersRow();

            const auto& assessmentsList = subj->GetAssessments(); 

            for (size_t i = 0; i < assessmentsList.size(); ++i) {
                Assessments* assessment = assessmentsList[i];
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                switch (assessment->getType()) {
                    case AssessmentType::EXAM:
                        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "Екзамен / Залік");
                        break;
                    case AssessmentType::COURSEWORK:
                        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.4f, 1.0f), "Курсова робота");
                        break;
                    case AssessmentType::PRACTICE:
                        ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "Практика");
                        break;
                    case AssessmentType::REGULAR:
                    default:
                        ImGui::Text("Звичайне завдання");
                        break;
                }

                ImGui::TableSetColumnIndex(1);
                float avgGrade = assessment->getCurrentScore();
                ImGui::Text("%.1f", avgGrade);

                ImGui::TableSetColumnIndex(2);
                if (assessment->getIsBlocker()) {
                    if (assessment->isPassed()) {
                        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Здано");
                    } else {
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "БОРГ");
                    }
                } else {
                    ImGui::Text("-");
                }

                ImGui::TableSetColumnIndex(3);
                ImGui::PushID(i);
                if (ImGui::Button("Додати оцінку")) {
                    appState.selectedAssessmentForGrade = assessment;
                    appState.newGradeValue = 0.0f;
                    appState.openGradeModal = true;
                }
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
        
        if (appState.openGradeModal) {
            ImGui::OpenPopup("Введення оцінки");
            appState.openGradeModal = false;
        }

    } else {
        ImGui::Text("Оберіть предмет зліва або створіть новий.");
    }
    ImGui::EndChild();

    DrawAddSubjectModal(appState);
    DrawAddGradeModal(appState);

    ImGui::Columns(1);
    ImGui::End();
}

} 