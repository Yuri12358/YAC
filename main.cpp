#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "GuiInteractor.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
	}, Qt::QueuedConnection);
	yac::GuiInteractor interactor(engine.rootContext(), &engine);
    engine.load(url);

	/*
	interactor.fireNewArchiveCreated("D:\\asdf.yac");
	QList<QUrl> urls;
	urls.append(QUrl::fromLocalFile("D:\\test"));
	interactor.fireAddFilesToArchive(urls);

	interactor.fireOpenArchive(QUrl::fromLocalFile("D:\\asdf.yac"));
	*/
    return app.exec();
}
