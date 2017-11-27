#include <QImage>
#include <QDebug>
#include <QQueue>
#include <QColor>

#include "imagequeue.h"
#include "simpletracer.h"


ImageQueue::ImageQueue(QObject *parent, int capacity) :
    QObject(parent),
    m_capacity(capacity)
{
    m_libre = CreateSemaphore(NULL, m_capacity, m_capacity, NULL);
    m_occupe = CreateSemaphore(NULL, 0, m_capacity, NULL);
    m_mutex = CreateMutex(NULL, FALSE, NULL);
}

ImageQueue::~ImageQueue()
{
}

void ImageQueue::enqueue(QImage *item)
{
    DWORD dwWaitResult;
    dwWaitResult = WaitForSingleObject(m_libre, INFINITE);
    if(dwWaitResult == WAIT_OBJECT_0) {
        dwWaitResult = WaitForSingleObject(m_mutex, INFINITE);
        if(dwWaitResult == WAIT_OBJECT_0) {
            // tracer la taille de la file lorsqu'elle change
            m_queue.push_back(*item);
            SimpleTracer::writeEvent(this, 0);
            ReleaseSemaphore(m_occupe, 1, NULL);
            ReleaseMutex(m_mutex);
        }
    }
}

QImage *ImageQueue::dequeue()
{
    DWORD dwWaitResult;
    dwWaitResult = WaitForSingleObject(m_occupe, INFINITE);
    if(dwWaitResult == WAIT_OBJECT_0) {
        dwWaitResult = WaitForSingleObject(m_mutex, INFINITE);
        if(dwWaitResult == WAIT_OBJECT_0) {
            // tracer la taille de la file lorsqu'elle change
            QImage *temp = new QImage(m_queue.at(0).width(), m_queue.at(0).height(), QImage::Format_ARGB32);
            QColor oldColor;
            int r,g,b;

            for(int x=0; x<temp->width(); x++){
                for(int y=0; y<temp->height(); y++){
                    oldColor = QColor(temp->pixel(x,y));

                    r = oldColor.red();
                    g = oldColor.green();
                    b = oldColor.blue();

                    temp->setPixel(x,y, qRgb(r,g,b));
                }
            }
            m_queue.pop_front();
            SimpleTracer::writeEvent(this, 0);
            ReleaseSemaphore(m_libre, 1, NULL);
            ReleaseMutex(m_mutex);
            return temp;
        }
    }
    printf("PROBLEME");
}
