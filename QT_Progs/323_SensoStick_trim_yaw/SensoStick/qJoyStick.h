#ifndef QJOYSTICK_H
#define QJOYSTICK_H

#include <QObject>
#include <QString>
#include <QList>

#include <SDL.h>




class QJoystick : public QObject
{
    Q_OBJECT

public:
    QJoystick();
    ~QJoystick();
    int availableJoysticks();
    int currentJoystick();
    QString joystickName(int id);
    int joystickNumAxes(int id);
    int joystickNumButtons(int id);
    int joystickNumHats(int id);
    int joystickNumBalls(int id);
    QList<int> axis;
    QList<bool> buttons;
    QList<int> hats;
    QList<int> balls;

    int dx;
    int dy;
    void getdata();

public slots:
    void setJoystick(int jsNumber);

signals:

private:
    SDL_Joystick* m_joystick;
};

#endif // QJOYSTICK_H
