#ifndef SEMESTERMANAGER_H
#define SEMESTERMANAGER_H

#include <vector>
#include <string>
#include "Gradebook.h"
#include "subject.h"

class SemesterManager {
private:
    std::vector<Subject*> archive;
    int semester = 1;

public:
    ~SemesterManager() {
        for (Subject* sub : archive) {
            delete sub;
        }
    }

    bool canEndSemester(Gradebook* gb) const {
        if (gb->hasPendingBlockers()) {
            return false;
        }
        return true; 
    }

bool transitionToNextSemester(Gradebook* gb) {
        if (!canEndSemester(gb)) {
            return false; 
        }

        std::vector<Subject*> currentSubjects = gb->extractAllSubjects();

        for (Subject* sub : currentSubjects) {

            sub->setSemester(this->semester); 

            archive.push_back(sub);
        }

        semester++;
        return true;    
    }
};

#endif 