#include "imageloaderstage.h"

#include <QDir>
#include <QImageReader>
#include <QDebug>

ImageLoaderStage::ImageLoaderStage(QObject *parent) :
    PipelineStage(parent)
{
}

void ImageLoaderStage::execute()
{
    setState(PipelineStage::waiting);
    // Load only supported formats
    QStringList filters;
    foreach(QByteArray format, QImageReader::supportedImageFormats()) {
        filters += "*." + format;
    }
    if (m_imagedir.exists()) {
        foreach(QString img, m_imagedir.entryList(filters, QDir::Files)) {
            setState(PipelineStage::running);
            QString imgPath = m_imagedir.filePath(img);
            qDebug() << "load " << QFileInfo(imgPath).fileName();
            setState(PipelineStage::waiting);
            m_prod->enqueue(new QImage(imgPath));
        }
    }
    // no more image to process
    m_prod->enqueue(0);
}
