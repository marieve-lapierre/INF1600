#include <QImage>
#include <QDebug>
#include <QQueue>

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
    dwWaitResult = WaitForSingleObject(m_mutex, 0);
    if(dwWaitResult == WAIT_OBJECT_0) {
        dwWaitResult = WaitForSingleObject(m_libre, 0);
        if(dwWaitResult == WAIT_OBJECT_0) {
            // tracer la taille de la file lorsqu'elle change
            m_queue.push_back(*item);
            SimpleTracer::writeEvent(this, 0);
            ReleaseSemaphore(m_occupe, 1, NULL);
            ReleaseMutex(m_mutex);
        }
    }
    if(dwWaitResult == WAIT_TIMEOUT) {
        printf("Error from the libre semaphore.");
    }
}

QImage *ImageQueue::dequeue()
{
    DWORD dwWaitResult;
    dwWaitResult = WaitForSingleObject(m_mutex, 0);
    if(dwWaitResult == WAIT_OBJECT_0) {
        dwWaitResult = WaitForSingleObject(m_occupe, 0);
        if(dwWaitResult == WAIT_OBJECT_0) {
            // tracer la taille de la file lorsqu'elle change
            QImage temp = m_queue.at(0);
            m_queue.pop_front();
            SimpleTracer::writeEvent(this, 0);
            ReleaseSemaphore(m_libre, 1, NULL);
            ReleaseMutex(m_mutex);
            return &temp;
        }
    }
    if(dwWaitResult == WAIT_TIMEOUT) {
        printf("Error from the libre semaphore.");
    }
}
