#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      imageCount(0)
{
    ui->setupUi(this);
    ui->progressBar->setAlignment(Qt::AlignCenter);
    setWindowIcon(QIcon(":/icon.ico"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

// images selection
void MainWindow::on_selectImages_clicked()
{
    files+=QFileDialog::getOpenFileNames(this,"Select one or multiple images","/","Images (*.png *.jpg)");
    images.clear();
    for (auto& file : files) {
        images.append(Image(file));
        ++imageCount;
    }
    if (files.length()==1) ui->deleteImages->setText("Delete 1 image");
    else ui->deleteImages->setText("Delete "+QString::number(files.length())+" images");
}

// delete images
void MainWindow::on_deleteImages_clicked()
{
    files.clear();
    images.clear();
    imageCount=0;
    ui->deleteImages->setText("Delete 0 image");
}

// create final image
void MainWindow::initializeFinalImage()
{
    width=0;
    height=0;
    for (auto& image : images) {
        if (image.width()>width) width=image.width();
        if (image.height()>height) height=image.height();
    }
    finalImg.setImage(width,height);
    finalImg.setProgressBar(ui->progressBar);
}

//-------------------------------------------------------------------------------------------------

// flat field creation
void MainWindow::on_createFlat_clicked()
{
    QString saveFile;
    saveFile=QFileDialog::getSaveFileName(this,"Save image","/","Images (*.png *.jpg)");
    initializeFinalImage();
    finalImg.resetVectors();
    finalImg.addImages(images);
    finalImg.smoothVectors(0.2);
    finalImg.computeFlatField();
    finalImg.getImage().save(saveFile);
}



void MainWindow::on_applyFlat_clicked()
{
    QString FFfile=QFileDialog::getOpenFileName(this,"Select the Flat Field image","/","Images (*.png *.jpg)");
    QStringList ImgFiles=QFileDialog::getOpenFileNames(this,"Select the image to process","/","Images (*.png *.jpg)");
    QString saveFile=QFileDialog::getSaveFileName(this,"Save result as...","/","Images (*.png *.jpg)");

    if (FFfile=="" or ImgFiles.size()==0 or saveFile=="")
    {
        return;
    }

    QFileInfo fileInfo(saveFile);
    QDir dir(fileInfo.absoluteDir());

    Image FFimg(FFfile);
    QVector<Image> Imgs;

    int i(0);
    for (auto& imgFile:ImgFiles)
    {
        Imgs.push_back(Image(imgFile));
        if (FFimg.width()<Imgs[i].width() or FFimg.height()<Imgs[i].height())
        {
            return;
        }
        ++i;
    }

    int k(0);
    finalImg.setFlatField(FFimg);
    finalImg.setProgressBar(ui->progressBar);
    for (auto& resultImg:finalImg.applyFFtoImages(Imgs))
    {
        ++k;
        resultImg.save(dir.absolutePath()+"/"+fileInfo.baseName()+"_"+QString::number(k)+"."+fileInfo.completeSuffix());
    }
}
