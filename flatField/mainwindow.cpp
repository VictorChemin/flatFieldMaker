#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(this,SIGNAL(newProgress(int)),this,SLOT(setProgress(int)));
    ui->progressBar->setAlignment(Qt::AlignCenter);
    setWindowIcon(QIcon(":/icon.ico"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_selectImages_clicked()
{
    files+=QFileDialog::getOpenFileNames(this,"Select one or multiple images","/","Images (*.png *.jpg)");
    images.clear();
    for (auto& file : files) {
        images.append(QImage(file));
    }
    if (files.length()==1)
    {
        ui->deleteImages->setText("Delete 1 image");
    }
    else
    {
        ui->deleteImages->setText("Delete "+QString::number(files.length())+" images");
    }
}

void MainWindow::on_deleteImages_clicked()
{
    images.clear();
    ui->deleteImages->setText("Delete 0 image");
}

void MainWindow::initializeFinalImage()
{
    width=-1;
    height=-1;
    for (auto& image : images) {
        if (width<0) width=image.width();
        else if (image.width()<width) width=image.width();
        if (height<0) height=image.height();
        else if (image.height()<height) height=image.height();
    }
    finalImg=QPixmap(width,height).toImage();
}

void MainWindow::averageImages()
{
    QColor colorTemp;
    int r(0);
    int g(0);
    int b(0);
    int n(images.size());

    initializeFinalImage();

    progress=0;
    progress100=0;
    totalProgress=width*height*3;

    for (int i=0;i<width;++i) {
        for (int j=0;j<height;++j) {
            if (progress100!=progress*100/totalProgress)
            {
                progress100=progress*100/totalProgress;
                emit newProgress(progress100);
            }
            r=0;
            g=0;
            b=0;
            for (auto& image : images) {
                r+=image.pixelColor(i,j).red();
                g+=image.pixelColor(i,j).green();
                b+=image.pixelColor(i,j).blue();
            }
            colorTemp.setRed(r/n);
            colorTemp.setGreen(g/n);
            colorTemp.setBlue(b/n);
            finalImg.setPixelColor(i,j,colorTemp);
            ++progress;
        }
    }
}

void MainWindow::on_createFlat_clicked()
{
    QString saveFile;
    saveFile=QFileDialog::getSaveFileName(this,"Save image","/","Images (*.png *.jpg)");
    averageImages();
    setImgCenter();
    setDiagDist();
    setVectors();
    finalImg.save(saveFile);
}

void MainWindow::setImgCenter()
{
    xCenter=double(width-1)/2.;
    yCenter=double(height-1)/2.;
}

double MainWindow::getPixelDistance(int x,int y)
{
    double dx(double(x)-xCenter);
    double dy(double(y)-yCenter);

    return qSqrt(dx*dx+dy*dy);
}

void MainWindow::setDiagDist()
{
    diagDist=getPixelDistance(0,0);
}

void MainWindow::setVectors()
{
    QColor colorTemp;
    double gray;
    int k;

    double rValues[diagDist];
    double gValues[diagDist];
    double bValues[diagDist];
    int rNumber[diagDist];
    int gNumber[diagDist];
    int bNumber[diagDist];

    for (int i=0;i<diagDist;++i)
    {
        rValues[i]=0;
        gValues[i]=0;
        bValues[i]=0;
        rNumber[i]=0;
        gNumber[i]=0;
        bNumber[i]=0;
    }
    for (int i=0;i<width;++i) {
        for (int j=0;j<height;++j) {
            if (progress100!=progress*100/totalProgress)
            {
                progress100=progress*100/totalProgress;
                emit newProgress(progress100);
            }
            k=getPixelDistance(i,j);
            rValues[k]+=finalImg.pixelColor(i,j).redF();
            gValues[k]+=finalImg.pixelColor(i,j).greenF();
            bValues[k]+=finalImg.pixelColor(i,j).blueF();
            ++rNumber[k];
            ++gNumber[k];
            ++bNumber[k];
            ++progress;
        }
    }
    for (int i=0;i<diagDist;++i)
    {
        rValues[i]/=double(rNumber[i]);
        gValues[i]/=double(gNumber[i]);
        bValues[i]/=double(bNumber[i]);
    }
    for (int i=0;i<width;++i) {
        for (int j=0;j<height;++j) {
            if (progress100!=progress*100/totalProgress)
            {
                progress100=progress*100/totalProgress;
                emit newProgress(progress100);
            }
            k=getPixelDistance(i,j);
            colorTemp.setRgbF(rValues[k],gValues[k],bValues[k]);
            gray=colorTemp.lightness();
            colorTemp.setRgb(gray,gray,gray);
            finalImg.setPixelColor(i,j,colorTemp);
            ++progress;
        }
    }
    ui->progressBar->setValue(100);
}

void MainWindow::setProgress(int progress)
{
    QEventLoop loop;
    ui->progressBar->setValue(progress);
    ui->progressBar->update();
    loop.processEvents();
}

void MainWindow::on_applyFlat_clicked()
{
    QString FFfile=QFileDialog::getOpenFileName(this,"Select the Flat Field image","/","Images (*.png *.jpg)");
    QStringList ImgFiles=QFileDialog::getOpenFileNames(this,"Select the image to process","/","Images (*.png *.jpg)");
    QString saveFile=QFileDialog::getSaveFileName(this,"Save result as...","/","Images (*.png *.jpg)");

    QImage FFimg(FFfile);
    QImage Imgs[ImgFiles.length()];
    int i(0);
    for (auto& imgFile:ImgFiles)
    {
        Imgs[i].load(imgFile);
        if (FFimg.width()!=Imgs[i].width() or FFimg.height()!=Imgs[i].height())
        {
            return;
        }
        ++i;
    }

    QImage resultImg(QPixmap(FFimg.width(),FFimg.height()).toImage());



    totalProgress=FFimg.width()*FFimg.height()*2*ImgFiles.length();
    progress=0;
    progress100=0;

    double average(0);
    QFileInfo fileInfo(saveFile);
    int k(0);

    for (auto& Img:Imgs)
    {
        for (int i=0;i<Img.width();++i) {
            for (int j=0;j<Img.height();++j) {
                if (progress100!=progress*100/totalProgress)
                {
                    progress100=progress*100/totalProgress;
                    emit newProgress(progress100);
                }
                average+=FFimg.pixelColor(i,j).redF();
                ++progress;
            }
        }
        QColor colorTemp;
        average/=double(Img.width()*Img.height());
        for (int i=0;i<Img.width();++i) {
            for (int j=0;j<Img.height();++j) {
                if (progress100!=progress*100/totalProgress)
                {
                    progress100=progress*100/totalProgress;
                    emit newProgress(progress100);
                }
                colorTemp.setRedF(Img.pixelColor(i,j).redF()/FFimg.pixelColor(i,j).redF()*average);
                colorTemp.setGreenF(Img.pixelColor(i,j).greenF()/FFimg.pixelColor(i,j).greenF()*average);
                colorTemp.setBlueF(Img.pixelColor(i,j).blueF()/FFimg.pixelColor(i,j).blueF()*average);
                resultImg.setPixelColor(i,j,colorTemp);
                ++progress;
            }
        }
        ++k;
        QDir dir(fileInfo.absoluteDir());
        resultImg.save(dir.absolutePath()+"/"+fileInfo.baseName()+"_"+QString::number(k)+"."+fileInfo.completeSuffix());
        qDebug() << dir.absolutePath()+"/"+fileInfo.baseName()+"_"+QString::number(k)+"."+fileInfo.completeSuffix();
    }
    emit newProgress(100);

}
