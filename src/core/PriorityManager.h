#ifndef PRIORITYMANAGER_H
#define PRIORITYMANAGER_H

#include <map>
#include "IObservers.h"
#include "subject.h" 

class PriorityManager : public IObserver {
private:
    std::map<Subject*, int> subjectPriorities; 

public:
    void update(Subject* s) override;
    int getPriorityForSubject(Subject* s);
};

#endif 