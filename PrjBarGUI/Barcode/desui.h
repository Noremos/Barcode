#ifndef DESUI_H
#define DESUI_H

#include <QObject>
#include <QQuickItem>

class DesUI : public QObject
{
    Q_OBJECT
public:
    explicit DesUI(QObject *parent = nullptr);
    Q_INVOKABLE void addSImg(int index);
    Q_INVOKABLE void removeSImg(int index);
    Q_INVOKABLE void crateGraph(int index);
    Q_INVOKABLE void selectImg(int index);

signals:
};

#endif // DESUI_H
