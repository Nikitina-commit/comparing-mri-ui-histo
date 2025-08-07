#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QFileDialog"
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QImage>
#include <QList>
#include <QStringList>
#include <QListWidget>
#include <imagelist.h>
#include <QMessageBox>
#include <QGraphicsSceneMouseEvent>
#include <mouseevent.h>
#include <QWidget>
#include <QString>
#include <QGraphicsItem>
#include <QDebug>
#include <QLabel>
#include <boardscene.h>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsPathItem>
#include <QVector3D>
#include <QVector2D>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

typedef QList<QImage> QImageList;
typedef QList<QGraphicsScene> QmouseEvent;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();


    void on_listWidget_itemSelectionChanged();


    void on_verticalScrollBar_valueChanged(int value);

    void onClippedImage(const QImage& croppedImg); // Слот принимающий обрезанную область приложения

    void on_pushButton_2_clicked();

    void slotTimer();

    void on_pushButton_3_clicked();


    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_spinBox_valueChanged();

    void on_pushButton_6_clicked();

    void on_horizontalScrollBar_valueChanged(int value);

    void on_label_3_linkActivated();

    void on_pushButton_axes_clicked();

    void on_pushButton_left_clicked();

    void on_pushButton_right_clicked();

    void on_pushButton_up_clicked();

    void on_pushButton_down_clicked();


    void on_pushButton_7_clicked();

    void on_horizontalScrollBar_2_valueChanged(int value);

    void on_checkBox_stateChanged();

    void on_pushButton_UI_clicked();

    void on_pushButton_MRI_clicked();

    void on_pushButton_histo_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

private:
    Ui::MainWindow *ui;
    QString filePath;
    QImageList imageList;
    QTimer *timer;      /* Определяем таймер для подготовки актуальных размеров
                         * графической сцены
                         * */
    mouseEvent *scene;  // Объявляем кастомную графическую сцену
    QGraphicsScene *scene1;
    boardScene *scene2;

private:
    /* Переопределяем событие изменения размера окна
     * для пересчёта размеров графической сцены
     * */
    void resizeEvent(QResizeEvent * event);


protected:
    //void mousePressEvent(QMouseEvent* event);
};
#endif // MAINWINDOW_H
