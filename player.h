#ifndef PLAYER_H
#define PLAYER_H

#include <person.h>

class Player : public Person
{
public:
    Player(int number);
    int m_number;
    int GetNumber() const noexcept;
};

#endif // PLAYER_H
