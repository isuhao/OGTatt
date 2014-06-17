#ifndef VEHICLE_H
#define VEHICLE_H

#include <gameobject.h>
#include <person.h>

class Vehicle : public GameObject
{
public:
    Vehicle();
private:
    Person* m_driver;
public:
    void    SetDriver(const Person * const driver) noexcept;
    Person* GetDriver() const noexcept;
};

#endif // VEHICLE_H
