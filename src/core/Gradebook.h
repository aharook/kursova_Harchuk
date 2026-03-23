#ifndef GRADEBOOK_H
#define GRADEBOOK_H

#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include "subject.h"

class Gradebook {
private:
    std::vector<Subject*> subjects; 

public:
    ~Gradebook() {
        for (Subject* sub : subjects) delete sub;
    }

    void addSubject(Subject* sub) {
        subjects.push_back(sub);
    }

    bool removeSubject(Subject* target) {
        auto it = std::find(subjects.begin(), subjects.end(), target);
        if (it == subjects.end()) {
            return false;
        }

        delete *it;
        subjects.erase(it);
        return true;
    }

    void clear() {
        for (Subject* sub : subjects) {
            delete sub;
        }
        subjects.clear();
    }
    
    const std::vector<Subject*>& getSubjects() const { return subjects; }

    std::map<std::string, double> getActualAverages() const {
        std::map<std::string, double> actualAverages;
        
        for (const Subject* sub : subjects) {
            actualAverages[sub->getLinkId()] = sub->getCurrentScore();
        }

        return actualAverages;
    }

    ScaleType getSubjectScale(const std::string& target_link_id) const {
        for (const Subject* sub : subjects) {
            if (sub->getLinkId() == target_link_id) {
                return sub->getScale();
            }
        }
        return ScaleType::TwelvePoint; 
    }

    bool hasPendingBlockers() const {
        for (const Subject* sub : subjects) {
            if (sub->hasPendingBlockers()) {
                return true;
            }
        }
        return false;
    }

    std::vector<Subject*> extractAllSubjects() {
        std::vector<Subject*> extracted = subjects; 
        subjects.clear(); 
        return extracted; 
    }
};

#endif
