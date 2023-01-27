#include "senso_stick.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Senso_Stick w;
    w.show();

    return a.exec();
}
