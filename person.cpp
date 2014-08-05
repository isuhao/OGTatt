#include "person.h"

Person::Person():
    m_walkspeed(2.0),
    m_alive(true)
{
    SetName(QString("Person"));
}

void    Person::SetWalkSpeed(const double walkspeed) noexcept   {m_walkspeed = walkspeed;}
double  Person::GetWalkSpeed() const noexcept                   {return m_walkspeed;}
void    Person::SetHealth(const int health) noexcept            {m_health = health;}
int     Person::GetHealth() const noexcept                      {return m_health;}
void    Person::CheckHealth() noexcept                          {if (m_health <= 0) m_alive = false;}
void    Person::Die() noexcept                                  {m_alive = false;}
void    Person::Revive() noexcept                               {m_alive = true;}
bool    Person::GetAlive() const noexcept                       {return m_alive;}
