#include "gameobject.h"
#include <cmath>

GameObject::GameObject():
    m_name(QString("GameObject")),
    m_pi(3.141592653),
    m_x(0.0),
    m_y(0.0),
    m_z(0.0),
    m_forwardspeed(0.0),
    m_sidespeed(0.0),
    m_zspeed(0.0),
    m_rot(0.0),
    m_rotspeed(0.0)
{

}
void GameObject::SetName(QString name) noexcept {m_name = name;}
QString GameObject::GetName() const noexcept    {return m_name;}

void    GameObject::SetX(const double x) noexcept {m_x = x;}
double  GameObject::GetX() const noexcept           {return m_x;}
void    GameObject::SetY(const double y) noexcept   {m_y = y;}
double  GameObject::GetY() const noexcept           {return m_y;}
void    GameObject::SetZ(const double z) noexcept   {m_z = z;}
double  GameObject::GetZ() const noexcept           {return m_z;}

void    GameObject::SetForwardSpeed(const double forwardspeed) noexcept {m_forwardspeed = forwardspeed;}
double  GameObject::GetForwardSpeed() const noexcept                    {return m_forwardspeed;}
void    GameObject::SetSideSpeed(const double sidespeed) noexcept       {m_sidespeed = sidespeed;}
double  GameObject::GetSideSpeed() const noexcept                       {return m_sidespeed;}
void    GameObject::SetZSpeed(const double zspeed) noexcept             {m_zspeed = zspeed;}
double  GameObject::GetZSpeed() const noexcept                          {return m_zspeed;}

void    GameObject::SetRot(const double rot) noexcept           {m_rot = rot;}
double  GameObject::GetRot() const noexcept                     {return m_rot;}
void    GameObject::SetRotSpeed(const double rotspeed) noexcept {m_rotspeed = rotspeed;}
double  GameObject::GetRotSpeed() const noexcept                {return m_rotspeed;}

void    GameObject::Move() noexcept {
    m_rot += m_rotspeed;
    if (m_rot > 2*m_pi) m_rot -= 2*m_pi;
    if (m_rot <   0.0) m_rot += 2*m_pi;
    m_x += cos(m_rot)*m_forwardspeed;
    m_y += sin(m_rot)*m_forwardspeed;
    m_z += m_zspeed;
}
