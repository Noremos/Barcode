#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QQmlContext>

#include <QDebug>
#include "backend.h"
#include "QML/imageitem.h"
#include "bimageprovider.h"
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

// view.engine()->rootContext()->setContextProperty("msgBoard", &msgBoard);
    //qmlRegisterType<SelMat>("com.ui", 1, 0, "SelMat");
    //qmlRegisterSingletonType("ui.main","MainUI", 1, 0, "MainUI");
    //qmlRegisterSingletonInstance("ui.main", 1, 0, "MainUI", &mainUI);
    //qmlRegisterSingletonInstance("UI", 1, 0, "DesimgUI", &back.desUI);
    //qmlRegisterType<Backend>("ui.main", 1, 0, "MainUI");
    //QQuickStyle::setStyle("Material");
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
    &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    qmlRegisterType<ImageItem>("My", 1, 0, "ImageItem");
//    engine.addImageProvider(QLatin1String("colors"), new BImageProvider(&back));
    Backend back;
    engine.rootContext()->setContextProperty("backend", &back);
    EnumClass::declareQML();

    engine.load(url);


    //ApplicationWindow
    QObject *rootObject = engine.rootObjects().first();
    back.rootWindow = rootObject;
    //QObject *qmlObject = rootObject->findChild<QObject *>("window");

    // Step 2a: set or get the desired property value for the root object
    //rootObject->setProperty("visible", true);
    //qDebug() << rootObject->property("id").toString();
    //qDebug() << qmlObject->property("visible");

    // Step 2b: set or get the desired property value for any qml object
    //qmlObject->setProperty("visible", true);
    //qDebug() << qmlObject->property("visible");

//    qDebug() << "test";
//    qDebug() << engine.property("id").toString();
//    QMetaObject::invokeMethod(engine.rootContext()->contextObject(), "addSubImg",
//                              Q_ARG(int, -1));

    return app.exec();
}
//signals:
//void newMessagePosted(const QString &subject);
// onNewMessagePosted: console.log("New message received:", subject)



//Убирать кнопку повтоить при изменении сета, блокировать
