#include "ui.h"
#include "imgui.h"
#include <string>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <cstdio>
#include "grade_entry.h"
#include "subject.h"
#include "assessments.h"
#include "GradeConverter.h"

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

void DrawDashboard(AppState& appState) {

    std::vector<Subject*> sortedSubjects = appState.system.getGradebook().getSubjects();
    
    for (auto subj : sortedSubjects) {
        appState.pm.update(subj);
    }
    std::sort(sortedSubjects.begin(), sortedSubjects.end(), [&appState](Subject* a, Subject* b) {
        return appState.pm.getPriorityForSubject(a) > appState.pm.getPriorityForSubject(b);
    });

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Dashboard", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::BeginChild("TopPanel", ImVec2(0, 45), true);
    
    ImGui::Text("Поточний семестр: %d", appState.system.getCurrentSemester());
    ImGui::SameLine(180);
    
    if (ImGui::Button("Завантажити", ImVec2(120, 30))) { appState.system.loadSystemState("save.dat"); }
    ImGui::SameLine();
    if (ImGui::Button("Зберегти", ImVec2(120, 30))) { appState.system.saveSystemState("save.dat"); }
    

    bool hasDebts = appState.system.getGradebook().hasPendingBlockers();

    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 240);
    if (hasDebts) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
    }

    if (ImGui::Button(hasDebts ? "Завершити семестр (Є борги!)" : "Завершити семестр", ImVec2(240, 30))) {
        if (!hasDebts) {
            appState.system.endSemester();
            appState.selectedSubject = nullptr;
        }
    }
    ImGui::PopStyleColor(3);

    ImGui::EndChild();
    ImGui::Spacing();

    ImGui::Columns(2, "MainLayout");
    ImGui::SetColumnWidth(0, 350.0f);

    ImGui::TextDisabled("СПИСОК ВАШИХ ПРЕДМЕТІВ");
    ImGui::Separator();
    
    ImGui::BeginChild("SubjectsList", ImVec2(0, ImGui::GetContentRegionAvail().y - 45), true);
    for (size_t i = 0; i < sortedSubjects.size(); ++i) {
        Subject* subj = sortedSubjects[i];
        int prio = appState.pm.getPriorityForSubject(subj);
        bool isSelected = (appState.selectedSubject == subj);

        ImGui::PushID(static_cast<int>(i));
        
        if (subj->hasPendingBlockers()) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
        } else if (prio >= 30) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.3f, 1.0f));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f));
        }

        std::string itemLabel = subj->Getname() + "\nПріоритет: " + std::to_string(prio);
        
        if (ImGui::Selectable(itemLabel.c_str(), isSelected, 0, ImVec2(0, 42))) {
            if (isSelected) appState.selectedSubject = nullptr;
            else appState.selectedSubject = subj;
        }

        ImGui::PopStyleColor();
        ImGui::Separator();
        ImGui::PopID();
    }
    ImGui::EndChild();

    ImGui::Spacing();
    if (ImGui::Button("+ Створити предмет", ImVec2(-1, 35))) {
        ImGui::OpenPopup("Додати предмет");
    }
    
    ImGui::NextColumn();

    ImGui::BeginChild("SubjectDetailsArea", ImVec2(0, 0), true);
    
    if (appState.selectedSubject != nullptr) {
        Subject* subj = appState.selectedSubject;
        
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
            appState.selectedSubject = nullptr; 
        }
        ImGui::PopStyleColor(2);
        
        if (appState.selectedSubject == nullptr) {
            ImGui::EndChild();
            ImGui::NextColumn();
            DrawAddSubjectModal(appState);
            DrawAddGradeModal(appState);
            DrawEditSubjectModal(appState);
            ImGui::Columns(1);
            ImGui::End();
            return;
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
                auto grades = assessment->getGrades();
                if (grades.empty()) {
                    ImGui::TextDisabled("Немає");
                } else {
                    std::string gradesStr = "";
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

    } else {
        ImGui::TextDisabled("ЗАГАЛЬНА СТАТИСТИКА ПО СЕМЕСТРУ (Оберіть предмет зліва для деталей)");
        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

        ImGui::SeparatorText("Класична система оцінювання (з переведенням шкал)");
        
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Показувати оцінки у:");
        ImGui::SameLine();
        const char* displayScales = "12-бальній шкалі\0 100-бальній\0 5-бальній шкалі\0 10-бальній шкалі\0";
        ImGui::SetNextItemWidth(160);
        ImGui::Combo("##displayScale", &appState.selectedDisplayScale, displayScales, 4);
        ImGui::Spacing();
        std::map<std::string, double> actualAverages = appState.system.getGradebook().getActualAverages();

        if (ImGui::BeginTable("StandardSubjects", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) {
            ImGui::TableSetupColumn("Предмет");
            ImGui::TableSetupColumn("Підсумкова оцінка");
            ImGui::TableSetupColumn("Статус");
            ImGui::TableHeadersRow();

            for (auto* sub : sortedSubjects) {
                ScaleType nativeScale = appState.system.getGradebook().getSubjectScale(sub->getLinkId());
                if (nativeScale == ScaleType::Accumulative) continue;

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", sub->Getname().c_str());

                ImGui::TableSetColumnIndex(1);
                
                double nativeScore = actualAverages[sub->getLinkId()];

                ScaleType targetScale = ScaleType::TwelvePoint;
                std::string scaleSuffix = " (12-бальна)";
                
                if (appState.selectedDisplayScale == 1) { targetScale = ScaleType::Accumulative; scaleSuffix = " (100-бальна)"; }
                else if (appState.selectedDisplayScale == 2) { targetScale = ScaleType::FivePoint; scaleSuffix = " (5-бальна)"; }
                else if (appState.selectedDisplayScale == 3) { targetScale = ScaleType::TenPoint; scaleSuffix = " (10-бальна)"; }

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

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.0f, 1.0f));
        ImGui::SeparatorText("Накопичувальна система");
        ImGui::PopStyleColor();

        if (ImGui::BeginTable("AccumulativeSubjects", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) {
            ImGui::TableSetupColumn("Предмет");
            ImGui::TableSetupColumn("Накопичено балів");
            ImGui::TableSetupColumn("Статус");
            ImGui::TableHeadersRow();

            for (auto* sub : sortedSubjects) {
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
                if (appState.selectedDisplayScale == 1) targetScale = ScaleType::Accumulative;
                else if (appState.selectedDisplayScale == 2) targetScale = ScaleType::FivePoint;
                else if (appState.selectedDisplayScale == 3) targetScale = ScaleType::TenPoint;

                double totalScore = 0.0;
                for (auto* sub : sortedSubjects) {
                    ScaleType nativeScale = appState.system.getGradebook().getSubjectScale(sub->getLinkId());
                    double convertedScore = appState.uiConverter.convert(actualAverages[sub->getLinkId()], nativeScale, targetScale);
                    totalScore += convertedScore;
                }

                double averageScore = totalScore / sortedSubjects.size();

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.2f, 1.0f)); 
                ImGui::Text(" Семестр успішно закрито!");
                ImGui::SetWindowFontScale(1.2f); 
                ImGui::Text("Ваш загальний середній бал за семестр: %.2f", averageScore);
                ImGui::SetWindowFontScale(1.0f); 
                ImGui::PopStyleColor();
            }
        }
    }
    ImGui::EndChild();

    DrawAddSubjectModal(appState);
    DrawAddGradeModal(appState);
    DrawEditSubjectModal(appState);

    ImGui::Columns(1);
    ImGui::End();
}

}
