#ifndef DISPLAYWIDGET_H
#define DISPLAYWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>

extern "C"{
#include<libavformat/avformat.h>
#include<libavcodec/avcodec.h>
#include <libavcodec/packet.h>
}

namespace Ui {
class displayWidget;
}

class displayWidget : public QOpenGLWidget,protected QOpenGLExtraFunctions
{
    Q_OBJECT

public:
    explicit displayWidget(QWidget *parent = nullptr);
    ~displayWidget();

protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w,int h) override;

private:
    Ui::displayWidget *ui;
    QOpenGLShaderProgram shaderProgram;
    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer VBO;
    QOpenGLBuffer EBO;
    QOpenGLTexture texture;
    GLuint ids[3];

private:
    int w;
    int h;
    unsigned char* yData;
    unsigned char* uData;
    unsigned char* vData;

public:
    void uploadYuv(AVFrame *frame);

};

#endif // DISPLAYWIDGET_H
