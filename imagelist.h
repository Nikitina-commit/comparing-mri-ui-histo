#ifndef IMAGELIST_H
#define IMAGELIST_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QString>
#include <QDir>
#include <QFileDialog>
#include <QPoint>

class ImageList
{
public:
    ImageList();
    QImage img, cropImg, boundImg, finImg, nullImg;
    int perp[3] = {0,0,0};
    QDir dir;
    QString name;
    QPoint Zero, pos;
    bool tumor_update = 0;

    void setImg(QImage A) {
        img = A;
     }
    void setDir(QDir a) {
        dir = a;
    }
    void setName(QString a)
    { name = a;
    }
    void setCrop(QImage A){
        cropImg = A;
    }
    void setBound(QImage A){
    boundImg = A;
    }
    void setFinish(QImage A){
       finImg = A;
    }
    void setZero(int x, int y){
        Zero.setX(x);
        Zero.setY(y);
    }
    void setPos(QPoint p){
        pos = p;
    }
    void setPerp(int c, int k){
        perp[c] = k;
    }

    QImage getImg()
    {
        return img;
    }

    QDir getDir()
    {
        return dir;
    }

    QString getName()
    {
        return name;
    }

    QImage getCrop()
    {
        return cropImg;
    }

    QImage getBound(){
        return boundImg;
    }

    QImage getFinish(){
        return finImg;
    }
    QPoint getZero(){
        return Zero;
    }

    QImage deleteImg()
    {
        QImage nullImg(1,1, QImage::Format_RGB32);  // Create the image with the exact size of the shrunk scene
        nullImg.fill(Qt::transparent);
        return nullImg;
    }

    QPoint getPos(){
        return pos;
    }
    int getPerp(int c){
        return perp[c];
    }

private:



};

#endif // IMAGELIST_H
