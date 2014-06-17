#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QWidget>

class GameObject
{
public:
    GameObject();
private:
    QString m_name;

    double m_pi;
    double m_x;
    double m_y;
    double m_z;

    double m_forwardspeed;
    double m_sidespeed;
    double m_zspeed;

    double m_rot;
    double m_rotspeed;

public:
    void SetName(QString name) noexcept;
    QString GetName() const noexcept;


    void SetX(const double x) noexcept;
    double GetX() const noexcept;
    void SetY(const double y) noexcept;
    double GetY() const noexcept;
    void SetZ(const double z) noexcept;
    double GetZ() const noexcept;

    void SetForwardSpeed(const double forwardspeed) noexcept;
    double GetForwardSpeed() const noexcept;
    void SetSideSpeed(const double sidespeed) noexcept;
    double GetSideSpeed() const noexcept;
    void SetZSpeed(const double zspeed) noexcept;
    double GetZSpeed() const noexcept;

    void SetRot(const double rot) noexcept;
    double GetRot() const noexcept;
    void SetRotSpeed(const double rotspeed) noexcept;
    double GetRotSpeed() const noexcept;

    void Move() noexcept;
};

#endif // GAMEOBJECT_H
