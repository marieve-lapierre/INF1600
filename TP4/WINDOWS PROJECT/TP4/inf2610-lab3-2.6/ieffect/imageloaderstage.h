#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QObject>
#include <QDir>
#include "pipelinestage.h"

class ImageLoaderStage : public PipelineStage
{
    Q_OBJECT
    Q_PROPERTY(QDir imagedir READ imageDir WRITE setImageDir)

public:
    explicit ImageLoaderStage(QObject *parent = 0);
    void execute();
    QDir imageDir() const {
        return m_imagedir;
    }
    void setImageDir(QDir &dir) {
        m_imagedir = dir;
    }
private:
    QDir m_imagedir;
};

#endif // IMAGELOADER_H
