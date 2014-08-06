#include "person.h"

Person::Person():
    m_walkspeed(2.0),
    m_alive(true),
    m_vehicle(nullptr)
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

void    Person::EnterVehicle(Vehicle* vehicle) noexcept         {m_vehicle = vehicle;}
void    Person::LeaveVehicle() noexcept                         {m_vehicle = nullptr;}
Vehicle* Person::GetVehicle() const noexcept                    {return m_vehicle;}

void    Person::Move() noexcept {
    if (m_vehicle){
        SetX(m_vehicle->GetX());
        SetY(m_vehicle->GetY());
        SetZ(m_vehicle->GetZ());
        SetRot(m_vehicle->GetRot());
    }
    else{
        SetRot(GetRot()+GetRotSpeed());
        if (GetRot() > 360) SetRot(GetRot()-360);
        if (GetRot() <   0) SetRot(GetRot()+360);
        double rotrad = 2*Pi()*(-GetRot()/360.0);
        SetX(GetX()-(sin(rotrad)*GetForwardSpeed())+(cos(-rotrad)*GetSideSpeed()));
        SetY(GetY()-(cos(rotrad)*GetForwardSpeed())+(sin(-rotrad)*GetSideSpeed()));
        SetZ(GetZ()+GetZSpeed());
    }
}
