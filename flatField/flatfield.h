#ifndef FLATFIELD_H
#define FLATFIELD_H

#include <image.h>
#include <QPixmap>
#include <QVector>
#include <QObject>
#include <QProgressBar>
#include <QDebug>

class FlatField : public QObject
{
    Q_OBJECT

public:
    FlatField();
    FlatField(int width,int height);
    FlatField(QImage im);

    void resetVectors();
    void addImages(QVector<Image> ims);
    void computeFlatField();
    void setImage(int width,int height);
    void setProgressBar(QProgressBar* b);
    void progressChanged();
    void setFlatField(Image FF);
    void smoothVectors(double fact);
    void slowSort(double vec[],int length);

    Image getImage();
    Image applyFFtoImage(Image im);
    QVector<Image> applyFFtoImages(QVector<Image> ims);

    QVector<double> value;
    QVector<int> count;

private:
    Image field;
    int progress;
    QProgressBar *bar;
};

#endif // FLATFIELD_H
