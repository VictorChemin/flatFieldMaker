#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QFileDialog>
#include <QDebug>
#include <QVector>
#include <QImage>
#include <QtMath>
#include <QColor>
#include <QEventLoop>
#include <QIcon>
#include <QtGui>
#include <QFileInfo>
#include <QDir>
#include <QTimer>
#include <image.h>
#include <flatfield.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void averageImages();
    void initializeFinalImage();
    void setImgCenter();
    double getPixelDistance(int x,int y);
    void setDiagDist();
    void setVectors();
    void blur();

    void setProgress(int progress);

private slots:
    void on_selectImages_clicked();
    void on_deleteImages_clicked();
    void on_createFlat_clicked();
    void on_applyFlat_clicked();

private:
    Ui::MainWindow *ui;
    QStringList files;
    QVector<Image> images;
    FlatField finalImg;

    int imageCount;

    int width;
    int height;

    double xCenter;
    double yCenter;
    int diagDist;

    int progress;

    QTimer *timer;

signals:
    void newProgress(int percent);
};
#endif // MAINWINDOW_H
