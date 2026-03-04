#ifndef CSVREPORTBUILDER_H
#define CSVREPORTBUILDER_H

#include <iostream>
#include <fstream> 
#include <vector>
#include <string>
#include "subject.h"
#include "SemesterManager.h"

class CsvReportBuilder {
public:
    static void generateReport(const std::vector<Subject*>& subjects, const std::string& filename) {
        

        std::ofstream file(filename);


        if (!file.is_open()) {
            std::cerr << "Помилка: не вдалося створити файл " << filename << std::endl;
            return;
        }

        file << "\xEF\xBB\xBF";

        file << "Назва предмету,ID предмету,Семестр,Кількість завдань,Наявність боргів\n";


        for (const Subject* sub : subjects) {
            
            // Дістаємо дані
            std::string name = sub->Getname();
            std::string id = sub->Genlink_id();

            int tasksCount = sub->GetAssessments().size();
            std::string hasDebts = sub->hasPendingBlockers() ? "Так" : "Ні";

            file << name << ","
                 << id << ","

                 << tasksCount << ","
                 << hasDebts << "\n";
        }

        file.close();
        
        std::cout << "Успіх! Звіт збережено у файл: " << filename << std::endl;
    }
};

#endif