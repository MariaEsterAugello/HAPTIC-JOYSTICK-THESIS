#include "qjoystick.h"

#include <QDebug>

//#define POLL_INTERVAL 40

QJoystick::QJoystick()
{
    // Sure, we're only using the Joystick, but SDL doesn't work if video isn't initialised
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

//    for(int i=0;i<MAX_AXES;i++)
//    {
//        axis[i]=0;
//    }

}

int QJoystick::currentJoystick()
{
    //return SDL_JoystickIndex(m_joystick);
	return SDL_JoystickGetPlayerIndex(m_joystick);
}

QString QJoystick::joystickName(int js)
{
    Q_ASSERT(js < availableJoysticks());
    Q_ASSERT(js >= 0);
    //return QString(SDL_JoystickName(js));
	return QString(SDL_JoystickNameForIndex(js));
}

int QJoystick::joystickNumAxes(int js)
{
    Q_ASSERT(js < availableJoysticks());
    Q_ASSERT(js >= 0);
    return (SDL_JoystickNumAxes(m_joystick));
}

int QJoystick::joystickNumButtons(int js)
{
    Q_ASSERT(js < availableJoysticks());
    Q_ASSERT(js >= 0);
    return (SDL_JoystickNumButtons(m_joystick));
}
int QJoystick::joystickNumHats(int js)
{
    Q_ASSERT(js < availableJoysticks());
    Q_ASSERT(js >= 0);
    return (SDL_JoystickNumHats(m_joystick));
}

int QJoystick::joystickNumBalls(int js)
{
    Q_ASSERT(js < availableJoysticks());
    Q_ASSERT(js >= 0);
    return (SDL_JoystickNumBalls(m_joystick));
}


void QJoystick::setJoystick(int js)
{
    Q_ASSERT(js < availableJoysticks());
    Q_ASSERT(js >= 0);

    //deswegen lief es nicht
    //SDL_JoystickClose(m_joystick);

    m_joystick = SDL_JoystickOpen(js);
}

QJoystick::~QJoystick()
{
    axis.clear();
    buttons.clear();
    hats.clear();
    balls.clear();
    SDL_JoystickClose(m_joystick);
    SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
    this->deleteLater();
}


int QJoystick::availableJoysticks()
{
     return SDL_NumJoysticks();
}

void QJoystick::getdata()
{
    axis.clear();
    buttons.clear();
    hats.clear();
    balls.clear();


        SDL_Event event;

     SDL_PollEvent(&event);

    for(int i=0;i<SDL_JoystickNumAxes(m_joystick);i++)
    {
        axis.append(SDL_JoystickGetAxis(m_joystick,i));
    }

//    SDL_Event event;
//
//    while ( SDL_PollEvent(&event) )
//    {
//        switch(event.type)
//        {
//        case SDL_JOYAXISMOTION:
//           axis.at(event.jaxis.axis) = event.jaxis.value;
//            axis[event.jaxis.axis] = event.jaxis.value;
//            break;
//        default:
//            break;
//        }
//    }

    for(int i=0;i<SDL_JoystickNumButtons(m_joystick);i++)
    {
        buttons.append(SDL_JoystickGetButton(m_joystick,i));
    }

    for(int i=0;i<SDL_JoystickNumHats(m_joystick);i++)
    {
        hats.append(SDL_JoystickGetHat(m_joystick,i));
    }
    for(int i=0;i<SDL_JoystickNumBalls(m_joystick);i++)
    {
        balls.append(SDL_JoystickGetBall(m_joystick,i, &dx, &dy));
    }
}
