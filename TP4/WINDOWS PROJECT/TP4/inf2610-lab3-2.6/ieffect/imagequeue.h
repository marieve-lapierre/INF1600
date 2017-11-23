#ifndef IMAGEQUEUE_H
#define IMAGEQUEUE_H

#include <QObject>
#include <windows.h>
#include <stdio.h>
#include <QVector>

class QImage;

class ImageQueue : public QObject
{
    Q_OBJECT
public:
    explicit ImageQueue(QObject *parent = 0, int capacity = 4);
    ~ImageQueue();
    void enqueue(QImage *item);
    QImage *dequeue();
    bool isEmpty();
private:
    int m_capacity;
    QVector<QImage> m_queue;
    HANDLE m_libre;
    HANDLE m_occupe;
    HANDLE m_mutex;
};

#endif // IMAGEQUEUE_H
