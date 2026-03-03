#include "displaywidget.h"
#include "ui_displaywidget.h"

#include <PlayerController.h>
#include <QFile>
#include <VideoThread.h>

displayWidget::displayWidget(QWidget *parent)
    : QOpenGLWidget(parent),
    ui(new Ui::displayWidget),
    VBO(QOpenGLBuffer::VertexBuffer),
    EBO(QOpenGLBuffer::IndexBuffer),
    texture(QOpenGLTexture::Target2D),
    w(1920),h(1080)
{
    ui->setupUi(this);
    yData = new unsigned char[w*h];
    uData = new unsigned char[w*h/4];
    vData = new unsigned char[w*h/4];
    // QFile file(":/YUV.dat");
    // if(file.open(QIODevice::ReadOnly)){
    //     file.read((char*)yData,w*h);
    //     file.read((char*)uData,w*h/4);
    //     file.read((char*)vData,w*h/4);
    //     file.close();
    // }
    // VideoThread *vt= new VideoThread();
    // connect(vt,&VideoThread::frameReady,this,[=](uchar* y, uchar* u, uchar* v, int width, int height){
    //     this->uploadYuv(y,u,v,width,height);
    //     delete[] y;
    //     delete[] u;
    //     delete[] v;
    // });
    // vt->start();
}

displayWidget::~displayWidget()
{
    makeCurrent();
    VAO.destroy();
    VBO.destroy(); // 记得销毁 VBO/EBO
    EBO.destroy();
    glDeleteTextures(3, ids);

    // 释放内存
    if (yData) delete[] yData;
    if (uData) delete[] uData;
    if (vData) delete[] vData;

    doneCurrent();
    delete ui;
}

void displayWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.0f,0.5f,0.9f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glGenTextures(3, ids);
    for (int i = 0; i < 3; ++i) {
        glBindTexture(GL_TEXTURE_2D, ids[i]);//告诉OpenGL下面代码中对2D纹理的任何设置都是针对索引为ids[i]的纹理的
        // 缩放过滤：必须设为线性，因为 U/V 需要拉伸
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        // 边缘处理：避免边缘出现杂色
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    // 2. 顶点数据准备 (必须补回 VBO/EBO)
    float vertices[] = {
        // 位置(x,y,z)      // 纹理坐标(u,v)
        -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, // 左上
        1.0f,  1.0f, 0.0f,  1.0f, 0.0f, // 右上
        -1.0f, -1.0f, 0.0f,  0.0f, 1.0f, // 左下
        1.0f, -1.0f, 0.0f,  1.0f, 1.0f  // 右下
    };
    unsigned int indices[] = { 0, 1, 2, 3 };

    VAO.create();
    VAO.bind();

    VBO.create();
    VBO.bind();
    VBO.allocate(vertices, sizeof(vertices));

    EBO.create();
    EBO.bind();
    EBO.allocate(indices, sizeof(indices));


    // shaderProgram.create();
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,":/gl.vert");
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,":/gl.frag");
    shaderProgram.link();
    shaderProgram.bind();

    shaderProgram.enableAttributeArray(0);
    shaderProgram.setAttributeBuffer(0,GL_FLOAT,0,3,5*sizeof(float));

    shaderProgram.enableAttributeArray(1);
    shaderProgram.setAttributeBuffer(1,GL_FLOAT,3*sizeof(float),2,5*sizeof(float));

    VAO.release();
    // uploadYuv(yData, uData, vData, w, h);
}

void displayWidget::paintGL()
{
    shaderProgram.bind();

    // 绑定纹理单元索引到 Shader 的 uniform 变量
    shaderProgram.setUniformValue("tex_y", 0); // 对应 GL_TEXTURE0
    shaderProgram.setUniformValue("tex_u", 1); // 对应 GL_TEXTURE1
    shaderProgram.setUniformValue("tex_v", 2); // 对应 GL_TEXTURE2

    // 激活并绑定纹理（虽然 upload 里绑过了，但 paintGL 建议显式绑定）
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ids[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ids[1]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, ids[2]);

    VAO.bind();
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, nullptr);
    VAO.release();
    shaderProgram.release();
}

void displayWidget::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
}

void displayWidget::uploadYuv(AVFrame *frame)
{
    if(!frame) return;
    makeCurrent();
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,ids[0]);
    glPixelStorei(GL_UNPACK_ROW_LENGTH,frame->linesize[0]);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RED,frame->width,frame->height,0,GL_RED,GL_UNSIGNED_BYTE,frame->data[0]);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,ids[1]);
    glPixelStorei(GL_UNPACK_ROW_LENGTH,frame->linesize[1]);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RED,frame->width/2,frame->height/2,0,GL_RED,GL_UNSIGNED_BYTE,frame->data[1]);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,ids[2]);
    glPixelStorei(GL_UNPACK_ROW_LENGTH,frame->linesize[2]);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RED,frame->width/2,frame->height/2,0,GL_RED,GL_UNSIGNED_BYTE,frame->data[2]);

    // 重置对齐参数，避免影响后续渲染
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    update(); // 触发 paintGL

    // 【重要：释放引用】
    // 渲染完成后，减少引用计数。如果此时没有其他地方使用，FFmpeg 会自动回收这块内存。
    av_frame_free(&frame);


    // makeCurrent(); // 确保 OpenGL 上下文激活

    // // 解决字节对齐问题（非常重要：如果宽度不是 4 的倍数，不加这行画面会花）
    // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // // 更新 Y 平面
    // glActiveTexture(GL_TEXTURE0); // 激活 0 号纹理单元
    // glBindTexture(GL_TEXTURE_2D, ids[0]);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, y);

    // // 更新 U 平面 (宽高减半)
    // glActiveTexture(GL_TEXTURE1); // 激活 1 号纹理单元
    // glBindTexture(GL_TEXTURE_2D, ids[1]);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, u);

    // // 更新 V 平面 (宽高减半)
    // glActiveTexture(GL_TEXTURE2); // 激活 2 号纹理单元
    // glBindTexture(GL_TEXTURE_2D, ids[2]);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, v);

    // update(); // 触发 paintGL
}
