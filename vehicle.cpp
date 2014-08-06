#include <vehicle.h>

Vehicle::Vehicle():
    m_driver(nullptr)
{
    SetName(QString("Vehicle"));
    AddSprite(QPixmap(QString("://car_kangoo")));
}

void    Vehicle::SetDriver(Person * const driver) noexcept  {m_driver = driver;}
Person* Vehicle::GetDriver() const noexcept                 {return m_driver;}
void    Vehicle::ClearDriver() noexcept                     {m_driver = nullptr;}

void    Vehicle::KeyAction(bool const keys[n_keys]) noexcept {
    if (keys[Up]){  SetForwardSpeed(GetForwardSpeed()+0.1);}
    if (keys[Down]){SetForwardSpeed(GetForwardSpeed()-0.1);}
    if (keys[Left]){SetRot(GetRot()-(GetForwardSpeed()/5.0));}
    if (keys[Right]){SetRot(GetRot()+(GetForwardSpeed()/5.0));}
    if (!keys[Up]&&!keys[Down]){SetForwardSpeed(GetForwardSpeed()/2.0);}
}
