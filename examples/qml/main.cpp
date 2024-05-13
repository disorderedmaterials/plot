#include "component.h"
#include "data.h"
#include <QApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("./main.qml")));

    // Create data source
    Mildred::DataSource data;
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
    auto *sinData = data.addData1DSource();
    sinData->setData(sinX, sinValues);

    engine.rootContext()->setContextProperty("mildredData", &data);

    return app.exec();
}
