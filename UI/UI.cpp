#include "ui.h"
#include "imgui.h"
#include <string>
#include <algorithm>
#include <cctype>
#include "grade_entry.h"

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

        // Захист: перевіряємо чи є в назві хоч один символ окрім пробілу
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
            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Введіть реальну назву та оберіть хоча б 1 тип завдання!");
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
            state.selectedSubject = newSubj; // Одразу робимо новий предмет активним

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

        // Перевірка на те, що назва не складається лише з пробілів
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
    // 1. ОНОВЛЮЄМО І СОРТУЄМО ПРЕДМЕТИ ПЕРЕД МАЛЮВАННЯМ
    for (auto subj : appState.subjects) {
        appState.pm.update(subj);
    }
    std::sort(appState.subjects.begin(), appState.subjects.end(), [&appState](Subject* a, Subject* b) {
        return appState.pm.getPriorityForSubject(a) > appState.pm.getPriorityForSubject(b);
    });

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Dashboard", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::Columns(2, "MainLayout");
    ImGui::SetColumnWidth(0, 350.0f);

    // ==========================================
    // ЛІВА ПАНЕЛЬ (Список предметів)
    // ==========================================
    ImGui::TextDisabled("СПИСОК ВАШИХ ПРЕДМЕТІВ");
    ImGui::Separator();
    
    ImGui::BeginChild("SubjectsList", ImVec2(0, ImGui::GetContentRegionAvail().y - 45), true);
    for (size_t i = 0; i < appState.subjects.size(); ++i) {
        Subject* subj = appState.subjects[i];
        int prio = appState.pm.getPriorityForSubject(subj);
        bool isSelected = (appState.selectedSubject == subj);

        ImGui::PushID(static_cast<int>(i));
        
        // Кольорове оформлення залежно від ситуації
        if (subj->hasPendingBlockers()) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f)); // Борг - Червоний
        } else if (prio >= 30) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.3f, 1.0f)); // Увага - Жовтий
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Норма - Зелений
        }

        std::string itemLabel = subj->Getname() + "\nПріоритет: " + std::to_string(prio);
        
        if (ImGui::Selectable(itemLabel.c_str(), isSelected, 0, ImVec2(0, 42))) {
            appState.selectedSubject = subj;
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

    // ==========================================
    // ПРАВА ПАНЕЛЬ (Деталі предмета)
    // ==========================================
    ImGui::BeginChild("SubjectDetailsArea", ImVec2(0, 0), true); 
    if (appState.selectedSubject != nullptr) {
        Subject* subj = appState.selectedSubject;
        
        ImGui::Text("Предмет: %s", subj->Getname().c_str());
        
        // НОВІ КНОПКИ: РЕДАГУВАТИ ТА ВИДАЛИТИ ПРЕДМЕТ
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 280); 
        if (ImGui::Button("Редагувати назву", ImVec2(135, 0))) {
            appState.openEditSubjectModal = true;
            snprintf(appState.editSubjName, sizeof(appState.editSubjName), "%s", subj->Getname().c_str());
        }
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
        if (ImGui::Button("Видалити предмет", ImVec2(135, 0))) {
            auto it = std::find(appState.subjects.begin(), appState.subjects.end(), subj);
            if (it != appState.subjects.end()) {
                delete *it; // Видаляємо з пам'яті
                appState.subjects.erase(it); // Видаляємо зі списку
                appState.selectedSubject = nullptr; // Скидаємо вибір
            }
        }
        ImGui::PopStyleColor(2);
        
        // Якщо предмет щойно видалили, припиняємо малювати цю панель далі
        if (appState.selectedSubject == nullptr) {
            ImGui::EndChild();
            ImGui::NextColumn();
            
            // Викликаємо модалки навіть якщо предмет видалено, щоб інтерфейс не зламався
            DrawAddSubjectModal(appState);
            DrawAddGradeModal(appState);
            DrawEditSubjectModal(appState);
            
            ImGui::Columns(1);
            ImGui::End();
            return;
        }

        ImGui::Separator();
        
        if (subj->hasPendingBlockers()) {
            ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "УВАГА: Є нездана сесія/завдання (Блокувальник)!");
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

                // 1. Тип
                ImGui::TableSetColumnIndex(0);
                switch (assessment->getType()) {
                    case AssessmentType::EXAM: ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "Екзамен / Залік"); break;
                    case AssessmentType::COURSEWORK: ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.4f, 1.0f), "Курсова робота"); break;
                    case AssessmentType::PRACTICE: ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "Практика"); break;
                    case AssessmentType::REGULAR: default: ImGui::Text("Звичайне завдання"); break;
                }

                // 2. Всі оцінки
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

                // 3. Середній бал
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%.1f", assessment->getCurrentScore());

                // 4. Статус блокувальника
                ImGui::TableSetColumnIndex(3);
                if (assessment->getIsBlocker()) {
                    if (assessment->isPassed()) ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Здано");
                    else ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "БОРГ");
                } else {
                    ImGui::Text("-");
                }

                // 5. Дія (Додано кнопку ОЧИСТИТИ)
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
        ImGui::TextDisabled("Оберіть предмет зліва або створіть новий.");
    }
    ImGui::EndChild();

    DrawAddSubjectModal(appState);
    DrawAddGradeModal(appState);
    DrawEditSubjectModal(appState);

    ImGui::Columns(1);
    ImGui::End();
}

}