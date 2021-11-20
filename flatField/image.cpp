#include "image.h"

Image::Image()
{

}

Image::Image(QString filename):
    QImage(filename)
{
    setCenter();
}

Image::Image(int width,int height):
    QImage(QPixmap(width,height).toImage())
{

}

Image::Image(QImage im):
    QImage(QPixmap(im.width(),im.height()).toImage())
{

}

void Image::load(QString filename)
{
    QImage::load(filename);
    setCenter();
}

void Image::setCenter()
{
    center=QVector2D(double(width()-1)/2.,double(height()-1)/2.);
}

int Image::getDistance(int x, int y)
{
    QVector2D distance(center-QVector2D(x,y));
    return distance.length();
}

int Image::getDiagonal()
{
    return getDistance(0,0);
}

int Image::getGray(int x, int y)
{
    return pixelColor(x,y).lightness();
}

float Image::getGrayF(int x, int y)
{
    return pixelColor(x,y).lightnessF();
}

void Image::setGray(int x, int y, int value)
{
    QColor colorTemp;
    colorTemp.setRgb(value,value,value);
    setPixelColor(x,y,colorTemp);
}

void Image::setGrayF(int x, int y, float value)
{
    QColor colorTemp;
    colorTemp.setRgbF(value,value,value);
    setPixelColor(x,y,colorTemp);
}

float Image::average()
{
    int w(width());
    int h(height());
    float result(0);

    for (int i=0;i<w;++i)
    {
        for (int j=0;j<h;++j)
        {
            result+=getGrayF(i,j);
        }
    }
    result/=(w*h);
    return result;
}
