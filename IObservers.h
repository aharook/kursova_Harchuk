#ifndef IOBSERVER_H
#define IOBSERVER_H

class Subject; // Кажемо компілятору: "Subject десь існує, не панікуй"

class IObserver {
public:
    virtual ~IObserver() = default;
    
    // Метод, через який Subject буде "кричати" своїм підписникам
    virtual void update(Subject* s) = 0; 
};

#endif