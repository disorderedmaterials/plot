#include <QApplication>
#include <QQmlApplicationEngine>
#include "component.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

//    qmlRegisterType<Mildred::MildredWidget>("com.company.my", 1, 0, "MildredEntity");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("./main.qml")));
    return app.exec();
}
