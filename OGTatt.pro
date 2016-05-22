    LIBS += ../OGTatt/Urho3D/lib/libUrho3D.a \
    -lpthread \
    -ldl \
    -lGL

DEFINES += URHO3D_COMPILE_QT

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += \
    ../OGTatt/Urho3D/include \
    ../OGTatt/Urho3D/include/Urho3D/ThirdParty \

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    bike.cpp \
    bullet.cpp \
    car.cpp \
    cookiejar.cpp \
    deco.cpp \
    effect.cpp \
    explosion.cpp \
    firepit.cpp \
    grass.cpp \
    hitfx.cpp \
    honti.cpp \
    inputmaster.cpp \
    level.cpp \
    luckey.cpp \
    mastercontrol.cpp \
    muzzle.cpp \
    objectpool.cpp \
    ogtattcam.cpp \
    pedestrian.cpp \
    pickup.cpp \
    player.cpp \
    poolmaster.cpp \
    poolobject.cpp \
    sceneobject.cpp \
    streetlight.cpp \
    tile.cpp \
    vehicle.cpp \
    wallcollider.cpp \
    character.cpp \
    intvector3.cpp \
    frop.cpp

HEADERS += \
    bike.h \
    bullet.h \
    car.h \
    cookiejar.h \
    deco.h \
    effect.h \
    explosion.h \
    firepit.h \
    grass.h \
    hitfx.h \
    honti.h \
    inputmaster.h \
    level.h \
    luckey.h \
    mastercontrol.h \
    muzzle.h \
    objectpool.h \
    ogtattcam.h \
    pedestrian.h \
    pickup.h \
    player.h \
    poolmaster.h \
    poolobject.h \
    sceneobject.h \
    streetlight.h \
    tile.h \
    vehicle.h \
    wallcollider.h \
    character.h \
    intvector3.h \
    frop.h

OTHER_FILES += \
    Docs/Ideas.txt
