#ifndef PERSON_H
#define PERSON_H

#include <gameobject.h>

class Person : public GameObject
{
public:
    Person();
private:
    double m_walkspeed;
    int m_health;
    bool m_alive;

public:
    void SetWalkSpeed(const double walkspeed) noexcept;
    double GetWalkSpeed() const noexcept;
    void SetHealth(const int health) noexcept;
    int  GetHealth() const noexcept;
    void CheckHealth() noexcept;
    void Die() noexcept;
    void Revive() noexcept;
    bool GetAlive() const noexcept;
};

#endif // PERSON_H
