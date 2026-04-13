#include "dashboard_parts_internal.h"
#include "imgui.h"
#include <map>
#include <string>
#include <vector>

namespace UI {
namespace DashboardParts {

void DrawSemesterOverview(AppState& appState, const std::vector<Subject*>& sortedSubjects, bool hasDebts) {
    ImGui::TextDisabled("ЗАГАЛЬНА СТАТИСТИКА ПО СЕМЕСТРУ (Оберіть предмет зліва для деталей)");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::SeparatorText("Класична система оцінювання");

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Показувати оцінки у:");
    ImGui::SameLine();
    const char* displayScales = "12-бальній\0 5-бальній\0 10-бальній\0";
    ImGui::SetNextItemWidth(140);
    ImGui::Combo("##displayScale", &appState.selectedDisplayScale, displayScales, 3);
    ImGui::Spacing();

    std::map<std::string, double> actualAverages = appState.system.getGradebook().getActualAverages();

    if (ImGui::BeginTable("StandardSubjects", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("Предмет");
        ImGui::TableSetupColumn("Підсумкова оцінка");
        ImGui::TableSetupColumn("Екзамен");
        ImGui::TableSetupColumn("Курсова");
        ImGui::TableSetupColumn("Практика");
        ImGui::TableSetupColumn("Статус");
        ImGui::TableHeadersRow();

        for (Subject* sub : sortedSubjects) {
            ScaleType nativeScale = appState.system.getGradebook().getSubjectScale(sub->getLinkId());
            if (nativeScale == ScaleType::Accumulative) {
                continue;
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", sub->Getname().c_str());

            ImGui::TableSetColumnIndex(1);

            double nativeScore = actualAverages[sub->getLinkId()];

            ScaleType targetScale = ScaleType::TwelvePoint;
            std::string scaleSuffix = " (12-бальна)";

            if (appState.selectedDisplayScale == 1) {
                targetScale = ScaleType::FivePoint;
                scaleSuffix = " (5-бальна)";
            } else if (appState.selectedDisplayScale == 2) {
                targetScale = ScaleType::TenPoint;
                scaleSuffix = " (10-бальна)";
            }

            double convertedScore = appState.uiConverter.convert(nativeScore, nativeScale, targetScale);
            ImGui::Text("%.1f%s", convertedScore, scaleSuffix.c_str());

            ImGui::TableSetColumnIndex(2);
            Detail::DrawAssessmentScoreCell(appState, sub, AssessmentType::EXAM, targetScale);

            ImGui::TableSetColumnIndex(3);
            Detail::DrawAssessmentScoreCell(appState, sub, AssessmentType::COURSEWORK, targetScale);

            ImGui::TableSetColumnIndex(4);
            Detail::DrawAssessmentScoreCell(appState, sub, AssessmentType::PRACTICE, targetScale);

            ImGui::TableSetColumnIndex(5);
            if (sub->isPassed()) {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Здано");
            } else {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Борг");
            }
        }
        ImGui::EndTable();
    }

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.7f, 0.8f, 1.0f));
    ImGui::SeparatorText("Накопичувальна система");
    ImGui::PopStyleColor();

    if (ImGui::BeginTable("AccumulativeSubjects", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("Предмет");
        ImGui::TableSetupColumn("Накопичено балів");
        ImGui::TableSetupColumn("Екзамен");
        ImGui::TableSetupColumn("Курсова");
        ImGui::TableSetupColumn("Практика");
        ImGui::TableSetupColumn("Статус");
        ImGui::TableHeadersRow();

        for (Subject* sub : sortedSubjects) {
            ScaleType nativeScale = appState.system.getGradebook().getSubjectScale(sub->getLinkId());
            if (nativeScale != ScaleType::Accumulative) {
                continue;
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", sub->Getname().c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.1f / 100.0", actualAverages[sub->getLinkId()]);

            ImGui::TableSetColumnIndex(2);
            Detail::DrawAssessmentScoreCell(appState, sub, AssessmentType::EXAM, ScaleType::Accumulative);

            ImGui::TableSetColumnIndex(3);
            Detail::DrawAssessmentScoreCell(appState, sub, AssessmentType::COURSEWORK, ScaleType::Accumulative);

            ImGui::TableSetColumnIndex(4);
            Detail::DrawAssessmentScoreCell(appState, sub, AssessmentType::PRACTICE, ScaleType::Accumulative);

            ImGui::TableSetColumnIndex(5);
            if (sub->isPassed()) {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Допущено");
            } else {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Борг");
            }
        }
        ImGui::EndTable();
    }

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (!sortedSubjects.empty()) {
        if (hasDebts) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
            ImGui::Text("Семестр не закрито: у вас є борги або незавершені предмети!");
            ImGui::PopStyleColor();
        } else {
            ScaleType targetScale = ScaleType::TwelvePoint;
            if (appState.selectedDisplayScale == 1) {
                targetScale = ScaleType::FivePoint;
            } else if (appState.selectedDisplayScale == 2) {
                targetScale = ScaleType::TenPoint;
            }

            double totalScore = 0.0;
            int includedSubjects = 0;
            for (Subject* sub : sortedSubjects) {
                bool hasRegularGrades = false;
                const auto& assessments = sub->GetAssessments();
                for (const Assessments* task : assessments) {
                    if (task->getType() == AssessmentType::REGULAR && task->hasGrades()) {
                        hasRegularGrades = true;
                        break;
                    }
                }

                if (!hasRegularGrades) {
                    continue;
                }

                ScaleType nativeScale = appState.system.getGradebook().getSubjectScale(sub->getLinkId());
                double convertedScore = appState.uiConverter.convert(actualAverages[sub->getLinkId()], nativeScale, targetScale);
                totalScore += convertedScore;
                includedSubjects++;
            }

            double averageScore = (includedSubjects > 0) ? (totalScore / includedSubjects) : 0.0;

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));
            ImGui::Text(" Семестр успішно закрито!");
            ImGui::SetWindowFontScale(1.2f);
            if (includedSubjects > 0) {
                ImGui::Text("Ваш загальний середній бал за семестр: %.2f", averageScore);
            } else {
                ImGui::Text("Немає оцінок для розрахунку середнього.");
            }
            ImGui::SetWindowFontScale(1.0f);
            ImGui::PopStyleColor();
        }
    }
}

}
}
