#ifndef IMAGE_H
#define IMAGE_H

#include <QImage>
#include <QString>
#include <QVector2D>
#include <QColor>
#include <QPixmap>

class Image : public QImage
{
public:
    Image();
    Image(QString filename);
    Image(int width,int height);
    Image(QImage im);

    void load(QString filename);
    void setCenter();
    int getDistance(int x,int y);
    int getDiagonal();
    int getGray(int x,int y);
    float getGrayF(int x,int y);
    void setGray(int x,int y,int value);
    void setGrayF(int x,int y,float value);
    float average();

private:
    QVector2D center;
};

#endif // IMAGE_H
