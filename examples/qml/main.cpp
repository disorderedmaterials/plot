#include "component.h"
#include <QApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("./main.qml")));

    auto *visMan = engine.findChild<Mildred::MildredWidget *>("visualisationManager");
    if (visMan)
        printf("FOUNDS ITS.\n");

    // Create test data
    std::vector<double> sinX, sinValues;
    const auto nPoints = 1000;
    const auto delta = 2.0 * M_PI / nPoints;
    sinX.reserve(nPoints);
    sinValues.reserve(nPoints);
    auto x = 0.0;
    for (auto n = 0; n < nPoints; ++n)
    {
        sinX.push_back(x);
        sinValues.push_back(sin(x));
        x += delta;
    }
    //    auto *sinData = ui_.TestingWidget->addData1D("sin");
    //    sinData->setData(sinX, sinValues);

    return app.exec();
}
