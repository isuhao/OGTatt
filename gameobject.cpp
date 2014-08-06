#include "gameobject.h"
#include <cmath>

GameObject::GameObject():
    m_name(QString("GameObject")),
    m_sprites(),
    m_frame(0),
    m_pi(3.141592653),
    m_x(100.0),
    m_y(100.0),
    m_z(0.0),
    m_forwardspeed(0.0),
    m_sidespeed(0.0),
    m_zspeed(0.0),
    m_rot(0.0),
    m_rotspeed(0.0)
{

}
void    GameObject::SetName(QString name) noexcept                 {m_name = name;}
QString GameObject::GetName() const noexcept                    {return m_name;}
void    GameObject::AddSprite(const QPixmap &sprite) noexcept    {m_sprites.push_back(sprite);}
QPixmap GameObject::GetSprite(int const frame) const noexcept   {return m_sprites[frame];}

double  GameObject::Pi() const noexcept {return m_pi;}

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
    if (m_rot > 360) m_rot -= 360;
    if (m_rot <   0) m_rot += 360;
    double rotrad = 2*m_pi*(-m_rot/360.0);
    m_x += -(sin(rotrad)*m_forwardspeed)+(cos(-rotrad)*m_sidespeed);
    m_y += -(cos(rotrad)*m_forwardspeed)+(sin(-rotrad)*m_sidespeed);;
    m_z += m_zspeed;
}

void GameObject::Draw(QPainter * const painter) noexcept
{
    double scale = 0.5 + (m_z/50.0);
    QTransform matrix;
    matrix.rotate(GetRot());
    QPixmap sprite_o = GetSprite(m_frame);
    QPixmap sprite_t = sprite_o.transformed(matrix);
    const double width  = scale * sprite_t.width();
    const double height = scale * sprite_t.height();
    const double dx = (scale * (sprite_o.width()-sprite_t.width()))/2;
    const double dy = (scale * (sprite_o.height()-sprite_t.height()))/2;
    painter->drawPixmap( GetX()+dx, GetY()+dy, width, height, sprite_t);
}
