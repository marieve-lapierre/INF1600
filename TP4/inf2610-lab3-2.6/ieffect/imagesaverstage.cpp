#include "imagesaverstage.h"

#include <QDebug>

ImageSaverStage::ImageSaverStage(QObject *parent) :
    PipelineStage(parent)
{
}

void ImageSaverStage::execute()
{
    setState(PipelineStage::waiting);
    int i = 0;
    if (!m_output.exists())
        m_output.mkpath(".");
    while(true) {
        QImage *image = m_cons->dequeue();
        if (image == 0)
            break;
        QString fName = QString("%1.png").arg(i);
        qDebug() << "save " << fName;
        setState(PipelineStage::running);
        printf(m_output.filePath(fName).toLatin1().data());
        bool ret = image->save(m_output.filePath(fName));
        setState(PipelineStage::waiting);
        delete image;
        (void) ret;
        i++;
    }
}
