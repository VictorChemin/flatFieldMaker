#include "flatfield.h"

FlatField::FlatField()
{

}

FlatField::FlatField(int width,int height):
    field(QPixmap(width,height).toImage())
{
    field.setCenter();
}

FlatField::FlatField(QImage im):
    field(QPixmap(im.width(),im.height()).toImage())
{
    field.setCenter();
}

void FlatField::resetVectors()
{
    int diag(field.getDiagonal()+1);
    value.clear();
    count.clear();
    for (int i=0;i<diag;++i) {
        value.append(0.);
        count.append(0);
    }
}

void FlatField::addImages(QVector<Image> ims)
{
    int t(0);
    int tmax(0);
    int tstep;

    int w;
    int h;
    int k;

    progress=0;

    for (auto& im:ims)
    {
        w=im.width();
        h=im.height();
        tmax+=w*h;
    }
    tstep=tmax/100;

    for (auto& im:ims)
    {
        im.setCenter();
        w=im.width();
        h=im.height();
        for (int i=0;i<w;++i)
        {
            for (int j=0;j<h;++j)
            {
                k=im.getDistance(i,j);
                value[k]+=im.getGrayF(i,j);
                count[k]++;

                if (t%tstep==0)
                {
                    progressChanged();
                    ++progress;
                }
                ++t;
            }
        }
    }
}

void FlatField::computeFlatField()
{
    int w;
    int h;
    int k;

    w=field.width();
    h=field.height();
    for (int i=0;i<w;++i)
    {
        for (int j=0;j<h;++j)
        {
            k=field.getDistance(i,j);
            field.setGrayF(i,j,value[k]/count[k]);
        }
    }
}

void FlatField::setImage(int width, int height)
{
    field=QPixmap(width,height).toImage();
    field.setCenter();
}

Image FlatField::getImage()
{
    return field;
}

void FlatField::setProgressBar(QProgressBar *b)
{
    bar=b;
}

void FlatField::progressChanged()
{
    if (progress>100) progress=100;
    else if (progress<0) progress=0;
    bar->setValue(progress);
}

QVector<Image> FlatField::applyFFtoImages(QVector<Image> ims)
{
    int t(0);
    int tmax(0);
    int tstep;

    QVector<Image> vecIm;
    Image imTemp;
    QColor colTemp;

    float hue;
    float sat;
    float lig;
    float av(field.average());

    int w;
    int h;

    progress=0;

    for (auto& im:ims)
    {
        w=im.width();
        h=im.height();
        tmax+=w*h;
    }
    tstep=tmax/100;

    w=field.width();
    h=field.height();
    for (auto& im:ims)
    {
        imTemp=im.copy();
        for (int i=0;i<w;++i)
        {
            for (int j=0;j<h;++j)
            {
                colTemp=im.pixelColor(i,j);
                hue=colTemp.hueF();
                sat=colTemp.saturationF();
                lig=colTemp.lightnessF();

                lig=lig/field.pixelColor(i,j).lightnessF()*av;

                colTemp.setHslF(hue,sat,lig);
                imTemp.setPixelColor(i,j,colTemp);

                if (t%tstep==0)
                {
                    progressChanged();
                    ++progress;
                }
                ++t;
            }
        }
        vecIm.append(imTemp);
    }
    return vecIm;
}

Image FlatField::applyFFtoImage(Image im)
{
    int t(0);
    int tmax(0);
    int tstep;

    Image imTemp;
    QColor colTemp;

    float hue;
    float sat;
    float lig;
    float av(field.average());

    int w;
    int h;


    progress=0;

    w=im.width();
    h=im.height();
    tmax+=w*h;

    tstep=tmax/100;

    w=field.width();
    h=field.height();
    imTemp=im.copy();
    for (int i=0;i<w;++i)
    {
        for (int j=0;j<h;++j)
        {
            colTemp=im.pixelColor(i,j);
            hue=colTemp.hueF();
            sat=colTemp.saturationF();
            lig=colTemp.lightnessF();

            lig=lig/field.pixelColor(i,j).lightnessF()*av;

            colTemp.setHslF(hue,sat,lig);
            imTemp.setPixelColor(i,j,colTemp);

            if (t%tstep==0)
            {
                progressChanged();
                ++progress;
            }
            ++t;
        }
    }

    return imTemp;
}

void FlatField::setFlatField(Image FF)
{
    field=FF;
}

void FlatField::smoothVectors(double fact)
{
    QVector<double> resultVec;
    resultVec=value;
    int m(fact*value.length());
    double valTemp[m*2-1];
    int l1;
    int l2;
    double calc;
    double coef;


    for (int i=0;i<value.size();++i)
    {
        for (int j=-m+1;j<m;++j)
        {
            l1=qAbs(i+j);
            if (l1>=value.size())
            {
                l1=value.size()-1;
            }
            l2=j+m-1;
            valTemp[l2]=value[l1]/count[l1];
        }
        slowSort(valTemp,m*2-1);
        calc=0;
        coef=0;
        for (int j=-m+1;j<m;++j)
        {
            coef+=1./(qAbs(j)+1);
            l2=j+m-1;
            calc+=valTemp[l2]/(qAbs(j)+1);
        }
        calc/=coef;
        resultVec[i]=calc*count[i];
    }

    value=resultVec;
}

void FlatField::slowSort(double vec[], int length)
{
    double temp;
    for (int i=length-1;i>0;--i)
    {
        for (int j=0;j<i;++j)
        {
            if (vec[j+1]<vec[j])
            {
                temp=vec[j];
                vec[j]=vec[j+1];
                vec[j+1]=temp;
            }
        }
    }
}
