#include "gptparser.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GptParser w;
    w.show();

    return a.exec();
}
