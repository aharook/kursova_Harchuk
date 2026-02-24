#ifndef IOBSERVER_H
#define IOBSERVER_H

class Subject; 

class IObserver {
public:
    virtual ~IObserver() = default;
    
    virtual void update(Subject* s) = 0; 
};

#endif