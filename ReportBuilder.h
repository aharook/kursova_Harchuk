#ifndef CSVREPORTBUILDER_H
#define CSVREPORTBUILDER_H

#include <iostream>
#include <fstream> 
#include <vector>
#include <string>
#include "subject.h"


class CsvReportBuilder {
public:

    static void generateReport(const std::vector<Subject*>& archive, const std::string& filename) {
        
        std::ofstream file(filename);

        file << "\xEF\xBB\xBF";

        file << "Назва предмету,ID предмету,Семестр,Кількість завдань,Наявність боргів\n";

        for (const Subject* sub : archive) {
            
            std::string name = sub->Getname();
            std::string id = sub->Genlink_id();
            
            int semester = sub->getSemester(); 
            
            int tasksCount = sub->GetAssessments().size();
            std::string hasDebts = sub->hasPendingBlockers() ? "Так" : "Ні";
            file << name << ","
                 << id << ","
                 << semester << ","
                 << tasksCount << ","
                 << hasDebts << "\n";
        }
        file.close();
    }
};

#endif