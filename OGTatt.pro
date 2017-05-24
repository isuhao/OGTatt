TARGET = ogtatt

    LIBS += ../OGTatt/Urho3D/lib/libUrho3D.a \
    -lpthread \
    -ldl \
    -lGL

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += \
    ../OGTatt/Urho3D/include \
    ../OGTatt/Urho3D/include/Urho3D/ThirdParty \
    ../OGTatt/Urho3D/include/Urho3D/ThirdParty/Bullet \

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
    controllable.cpp \
    level.cpp \
    luckey.cpp \
    mastercontrol.cpp \
    muzzle.cpp \
    objectpool.cpp \
    ogtattcam.cpp \
    pedestrian.cpp \
    pickup.cpp \
    player.cpp \
    spawnmaster.cpp \
    sceneobject.cpp \
    streetlight.cpp \
    tile.cpp \
    vehicle.cpp \
    wallcollider.cpp \
    character.cpp \
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
    controllable.h \
    level.h \
    luckey.h \
    mastercontrol.h \
    muzzle.h \
    objectpool.h \
    ogtattcam.h \
    pedestrian.h \
    pickup.h \
    player.h \
    spawnmaster.h \
    sceneobject.h \
    streetlight.h \
    tile.h \
    vehicle.h \
    wallcollider.h \
    character.h \
    frop.h

OTHER_FILES += \
    Docs/Ideas.txt

unix {
    isEmpty(DATADIR) {
        DATADIR = ~/.local/share
    }
    DEFINES += DATADIR=\\\"$${DATADIR}/ogtatt\\\"

    target.path = /usr/games/
    INSTALLS += target

    resources.path = $$DATADIR/luckey/ogtatt/
    resources.files = Resources/*
    INSTALLS += resources

    icon.path = $$DATADIR/icons/
    icon.files = ogtatt.svg
    INSTALLS += icon

    desktop.path = $$DATADIR/applications/
    desktop.files = ogtatt.desktop
    INSTALLS += desktop
}
