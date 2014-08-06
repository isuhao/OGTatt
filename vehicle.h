#ifndef VEHICLE_H
#define VEHICLE_H

#include <gameobject.h>

class Person;

class Vehicle : public GameObject
{
public:
    Vehicle();
private:
    Person* m_driver;
public:
    void    SetDriver(Person * const driver) noexcept;
    Person* GetDriver() const noexcept;
    void    ClearDriver() noexcept;
    void    KeyAction(bool const keys[n_keys]) noexcept;
};

#endif // VEHICLE_H
