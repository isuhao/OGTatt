#include "player.h"
#include <QKeyEvent>

Player::Player(int number):
    m_number(number),
    m_keys()
{
    //SET ALL KEYS TO FALSE
    for (int i = 0; i < n_keys; i++){
        m_keys[i] = false;
    }

    SetName(QString("Player"));
    AddSprite(QPixmap(QString("://frode")));
}

int Player::GetNumber() const noexcept {return m_number;}

void Player::KeyPress(int key) noexcept{
    switch (key){
        case Qt::Key_Up     : m_keys[Up]        = true; break;
        case Qt::Key_Down   : m_keys[Down]      = true; break;
        case Qt::Key_Left   : m_keys[Left]      = true; break;
        case Qt::Key_Right  : m_keys[Right]     = true; break;
        case Qt::Key_Shift  : m_keys[Run]       = true; break;
        case Qt::Key_Space  : m_keys[Jump]      = true; break;
        case Qt::Key_Z      : m_keys[Strave]    = true; break;
        case Qt::Key_Enter  : m_keys[Interact]  = true; break;
    default : break;
    }
}

void Player::KeyRelease(int key) noexcept{
    switch (key){
        case Qt::Key_Up     : m_keys[Up]        = false; break;
        case Qt::Key_Down   : m_keys[Down]      = false; break;
        case Qt::Key_Left   : m_keys[Left]      = false; break;
        case Qt::Key_Right  : m_keys[Right]     = false; break;
        case Qt::Key_Shift  : m_keys[Run]       = false; break;
        case Qt::Key_Space  : m_keys[Jump]      = false; break;
        case Qt::Key_Z      : m_keys[Strave]    = false; break;
        case Qt::Key_Enter  : m_keys[Interact]  = false; break;
    default : break;
    }
}

void Player::KeyAction() noexcept{
    if (GetVehicle()){GetVehicle()->KeyAction(m_keys);}
    else{
        //WHEN ON GROUND
        if (GetZ()==0){
            //WALK AND RUN
            double topspeed = (1 + m_keys[Run]) * GetWalkSpeed();
            if (m_keys[Up]){
                SetForwardSpeed((9*GetForwardSpeed()+topspeed)/10.0);
            }
            if (m_keys[Down]){
                SetForwardSpeed((9*GetForwardSpeed()-GetWalkSpeed())/10.0);
            }
            //STOP WALKING
            if ( m_keys[Up] &&  m_keys[Down]) SetForwardSpeed(0.0);
            if (!m_keys[Up] && !m_keys[Down]) SetForwardSpeed(0.0);

            if (!m_keys[Strave]){
                //STOP STRAVING
                SetSideSpeed(0.0);
                //TURN
                if (m_keys[Left])   SetRotSpeed(-3.0);
                if (m_keys[Right])  SetRotSpeed( 3.0);

            }

            if (m_keys[Strave]){
                //STOP TURNING
                SetRotSpeed(0.0);
                //STRAVE
                if (m_keys[Left]){
                    SetSideSpeed((9*GetSideSpeed()-GetWalkSpeed())/10.0);
                }
                if (m_keys[Right]){
                    SetSideSpeed((9*GetSideSpeed()+GetWalkSpeed())/10.0);
                }
            }
            //STOP TURNING AND STRAVING
            if ( m_keys[Left] &&  m_keys[Right]){
                SetRotSpeed(0.0);
                SetSideSpeed(0.0);
            }
            if (!m_keys[Left] && !m_keys[Right]){
                SetRotSpeed(0.0);
                SetSideSpeed(0.0);
            }
            //JUMP
            if (m_keys[Jump]){SetZSpeed(0.5);}
        }
        //FALL WHEN NOT ON GROUND
        if (GetZ() > 0) {SetZSpeed(GetZSpeed()-0.04); SetRotSpeed(0.0);}
        if (GetZ() < 0) {SetZSpeed(0); SetZ(0);}
    }
}
