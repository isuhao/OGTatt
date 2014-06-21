#include "vehicle.h"

Vehicle::Vehicle():
    m_driver(nullptr)
{
    SetName(QString("Vehicle"));
}

void    Vehicle::SetDriver(Person * const driver) noexcept   {m_driver = driver;}
Person* Vehicle::GetDriver() const noexcept                    {return m_driver;}
