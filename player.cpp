#include "player.h"

Player::Player(int number):
    m_number(number)
{
    SetName(QString("Player"));
}

int Player::GetNumber() const noexcept {return m_number;}
