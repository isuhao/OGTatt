#include "player.h"
#include <QKeyEvent>
#include <QDebug>

Player::Player(int number):
    m_number(number),
    m_keys()
{
    for (int i = 0; i < n_keys; i++){
        m_keys[i] = false;
    }
    SetName(QString("Player"));
    AddSprite(QPixmap(QString("://frode")));
}

int Player::GetNumber() const noexcept {return m_number;}

void Player::KeyPress(int key) noexcept{
    switch (key){
        case Qt::Key_Up     : m_keys[Up]    = true;
    break;
        case Qt::Key_Down   : m_keys[Down]  = true;
    break;
        case Qt::Key_Left   : m_keys[Left]  = true;
    break;
        case Qt::Key_Right  : m_keys[Right] = true;
    break;
    }
}

void Player::KeyRelease(int key) noexcept{
    switch (key){
        case Qt::Key_Up     : m_keys[Up]    = false;
    break;
        case Qt::Key_Down   : m_keys[Down]  = false;
    break;
        case Qt::Key_Left   : m_keys[Left]  = false;
    break;
        case Qt::Key_Right  : m_keys[Right] = false;
    break;
    }
}

void Player::KeyAction() noexcept{
    if (m_keys[Up] && GetForwardSpeed() < GetWalkSpeed()){
        SetForwardSpeed(GetForwardSpeed()+0.2);
    }
    if (m_keys[Down] && GetForwardSpeed() > -GetWalkSpeed()){
        SetForwardSpeed(GetForwardSpeed()-0.2);
    }
    if ( m_keys[Up] &&  m_keys[Down]) SetForwardSpeed(0.0);
    if (!m_keys[Up] && !m_keys[Down]) SetForwardSpeed(0.0);


    if (m_keys[Left])   SetRotSpeed(-3.0);
    if (m_keys[Right])  SetRotSpeed( 3.0);
    if ( m_keys[Left] &&  m_keys[Right]) SetRotSpeed(0.0);
    if (!m_keys[Left] && !m_keys[Right]) SetRotSpeed(0.0);
}
