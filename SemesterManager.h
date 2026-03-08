#ifndef SEMESTERMANAGER_H
#define SEMESTERMANAGER_H

#include <vector>
#include "Gradebook.h"
#include "subject.h"

class SemesterManager {
private:
    int currentSemester;

public:
    SemesterManager();
    
    int getCurrentSemester() const;
    
    bool canEndSemester(const Gradebook& activeGradebook) const;
    
    void transitionToNextSemester(Gradebook& activeGradebook, std::vector<Subject*>& archive);
};

#endif