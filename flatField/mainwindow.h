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

private slots:
    void on_selectImages_clicked();
    void on_deleteImages_clicked();
    void on_createFlat_clicked();

    void setProgress(int percent);

    void on_applyFlat_clicked();

private:
    Ui::MainWindow *ui;
    QStringList files;
    QVector<QImage> images;
    QImage finalImg;

    int width;
    int height;

    double xCenter;
    double yCenter;
    int diagDist;

    int progress100;
    int progress;
    int totalProgress;

signals:
    void newProgress(int percent);
};
#endif // MAINWINDOW_H
