#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <math.h>

//* кнопка поворота
//* поворот и смещение
//* сквозная ось для всех срезов
// поворот плоскости нарезки
// сравниваем каждое z гистологии с расчетным из другого метода
// (метод вычитания изображений)
//* автоматическое совмещение по контуру железы
//* обнуление поворота после сохранения
//* ставить оси в центр масс
// выходное количество срезов равно срезам гистологии
// поворот во время наложения (только текущего изображения), edtkbxtybt dj dhtvz yfkj;tybz
//* преобразование в полутоновое на входе
// привязка частей массива к изображению
// ось на мрт и узи по краю картинки/стола
// ось на гисте по нижнему краю железы


QString listfilesindir(QString path);
QImage setOpacity(QImage* image, qreal opacity);
QPoint pos1,pos2 = {0, 0};
int countImg = 0, axesX = 0, axesY = 0;
ImageList Imglist[100];
ImageList* A;
int currentImageItem, currentBoardPixel=0, currentBoardDepth=1, currentTumorPixel = 0;
bool exist = 0;
int board[100000][3], board_UI[100000][3],board_MRI[100000][3],board_histo[100000][3], pixel_UI_b = 1, pixel_MRI_b = 1, pixel_histo_b = 1;
int tumor[100000][3], tumor_UI[100000][3],tumor_MRI[100000][3],tumor_histo[100000][3], pixel_UI_t = 1, pixel_MRI_t = 1, pixel_histo_t = 1;
float angle_histo = 0;
int mass_center_UI[3], mass_center_MRI[3], mass_center_histo[3], z_slice = 0, step=0;
void axes(int x, int y);
double distance2d(int zero_x, int zero_y, int board_x, int board_y);
double distance3d(int zero_x, int zero_y, int zero_z, int board_x, int board_y, int board_z);
QPoint blue_min = {1000,1000}, blue_max = {0,0};
int blue_line[1000][2], blue_count = 0;
QPoint line(QPoint p1, QPoint p2, int x);
float angle(int line1x, int line1y, int line1z, int line2x, int line2y, int line2z);
float plane(QVector3D n, QVector3D m, char what, int a, int b);
float angle_calc = 0;
QVector3D norm_UI, norm_MRI, m_UI, m_MRI;
bool UI_exist = 0,MRI_exist = 0,histo_exist = 0;
int layer = 0;




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    filePath="0";
//    ui->graphicsView->setScene(scene);
    scene = new mouseEvent(); // Инициализируем графическую сцену
    scene1=new QGraphicsScene();
    scene2 = new boardScene();
    ui->graphicsView->setScene(scene1);  // Устанавливаем графическую сцену

    ui->pushButton_2->setCheckable(true);
    ui->pushButton_3->setCheckable(true);
    ui->pushButton_4->setCheckable(true);
    ui->pushButton_6->setCheckable(true);
    ui->pushButton_axes->setCheckable(true);
    ui->pushButton_UI->setCheckable(true);
    ui->pushButton_MRI->setCheckable(true);
    ui->pushButton_histo->setCheckable(true);

    timer = new QTimer();       // Инициализируем таймер
    connect(timer, &QTimer::timeout, this, &MainWindow::slotTimer);
    timer->start(100);
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked() //upload
{
    ui->pushButton_2->setChecked(false);
    ui->pushButton_3->setChecked(false);
    ui->pushButton_4->setChecked(false);
    ui->pushButton_6->setChecked(false);
    ui->pushButton_axes->setChecked(false);
    ui->pushButton_MRI->setChecked(false);
    ui->pushButton_UI->setChecked(false);
    ui->pushButton_histo->setChecked(false);

    ImageList ImgList[countImg];
    QString usersfilename = QFileDialog::getOpenFileName();
    QDir dir = QFileInfo(usersfilename).dir();


    qDebug() << dir; qDebug() << '\n';
    qDebug() << usersfilename;
    filePath = usersfilename;

        Imglist[countImg].setImg(QImage(filePath));
        Imglist[countImg].setDir(QFileInfo(usersfilename).dir());
        Imglist[countImg].setName(usersfilename);
        Imglist[countImg].setCrop(QImage(filePath).convertToFormat(QImage::Format_Grayscale16));
        Imglist[countImg].setBound(Imglist[countImg].getCrop());
        Imglist[countImg].setFinish(Imglist[countImg].getCrop());
        Imglist[countImg].setPos({0,0});

//        scene1=new QGraphicsScene();
//        scene1->addPixmap(QPixmap::fromImage(Imglist[countImg].getImg()));
//        ui->graphicsView->setScene(scene1);

      //  delete scene;
        //scene=nullptr;

         QListWidgetItem *item = new QListWidgetItem;
         item->setText(Imglist[countImg].getName());
         ui->listWidget->addItem(item);
         ui ->listWidget->show();

         for (int i = 0;i<=countImg; ++i) {

         qDebug()<<i<<" "<< Imglist[i].getName()<< " " << Imglist[i].getDir();
}
         countImg ++;
}




void MainWindow::on_listWidget_itemSelectionChanged()
{

    if (scene1!=nullptr){
            delete scene1;
        scene1 = nullptr;
            }
    if (scene!=nullptr){
        delete scene;
        scene = nullptr;
      }
      QString item = ui -> listWidget -> currentItem() -> text();
      qDebug() << item;
      exist = 0;
      for(int i = 0; i <= countImg; ++i){
      if(item == Imglist[i].getName()){
          ui->horizontalScrollBar->setValue(0);
          scene=new mouseEvent();
          scene->addPixmap(QPixmap::fromImage(Imglist[i].getFinish()));
          ui->graphicsView->setScene(scene);
          qDebug() << Imglist[i].getName() << " " <<i;
          currentImageItem = i;
          exist = 1;
          }
      }
      //scene->currentImg(A);
      qDebug() << Imglist[currentImageItem].getImg().height()<<" "<<Imglist[currentImageItem].getImg().width();
      qDebug()<< currentImageItem;

}


void MainWindow::on_verticalScrollBar_valueChanged(int value)
{
        QTransform matrix;
        qreal m = qreal(value)/50;
        matrix.scale(m, m);
        ui->graphicsView->setTransform(matrix);

}



void MainWindow::on_pushButton_2_clicked()  // crop
{
    ui->pushButton_2->setChecked(true);
    ui->pushButton_3->setChecked(false);
    ui->pushButton_4->setChecked(false);
    ui->pushButton_6->setChecked(false);
    ui->pushButton_axes->setChecked(false);
    ui->pushButton_MRI->setChecked(false);
    ui->pushButton_UI->setChecked(false);
    ui->pushButton_histo->setChecked(false);

    if (scene1!=nullptr){
        delete scene1;
        scene1=nullptr;
    }
    if (scene2!=nullptr){
            delete scene2;
            scene2=nullptr;
        }

    if(scene!=nullptr){
    QImage A = Imglist[currentImageItem].getCrop();
    QImage* p = &A;
    QImage B = p->copy(scene->getRect());
    qDebug() << " /";
    Imglist[currentImageItem].setCrop(B);
    qDebug() << " / /"<<p;

    delete scene;
    scene=nullptr;
    scene=new mouseEvent();
    scene->addPixmap(QPixmap::fromImage(Imglist[currentImageItem].getCrop()));
    ui->graphicsView->setScene(scene);
    Imglist[currentImageItem].setBound(B);
    Imglist[currentImageItem].setFinish(B);
    Imglist[currentImageItem].setZero(Imglist[currentImageItem].getCrop().height()/2, Imglist[currentImageItem].getCrop().width()/2);
    }
}

void MainWindow::slotTimer()
{
    timer->stop();
    scene->setSceneRect(0,0, ui->graphicsView->width(), ui->graphicsView->height());
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    timer->start(100);
}

void MainWindow::onClippedImage(const QImage& croppedImg)
{
}

void MainWindow::on_pushButton_3_clicked() //delete
{   ui->pushButton_3->setChecked(true);
    ui->pushButton_2->setChecked(false);
    ui->pushButton_4->setChecked(false);
    ui->pushButton_6->setChecked(false);
    ui->pushButton_axes->setChecked(false);
    ui->pushButton_MRI->setChecked(false);
    ui->pushButton_UI->setChecked(false);
    ui->pushButton_histo->setChecked(false);

    if(ui->checkBox->checkState() || countImg <= 1){

        for(int i = 0; i <= countImg; ++i){
              Imglist[i].setImg(Imglist[i].deleteImg());
              Imglist[i].setCrop(Imglist[i].deleteImg());
              Imglist[i].setBound(Imglist[i].deleteImg());
              Imglist[i].setFinish(Imglist[i].deleteImg());
              Imglist[i].setPos({0,0});
              Imglist[i].setName(" ");
          }
        currentImageItem = 0;
        currentBoardPixel=0;
        currentBoardDepth=1;
        currentTumorPixel = 0;
        countImg = 0;
        ui ->listWidget->clear();
        ui ->listWidget->show();
    }
    else
    {

    for(int i = currentImageItem; i <= countImg; ++i){
          Imglist[i].setImg(Imglist[i+1].getImg());
          Imglist[i].setDir(Imglist[i+1].getDir());
          Imglist[i].setName(Imglist[i+1].getName());
          Imglist[i].setCrop(Imglist[i+1].getCrop());
          Imglist[i].setBound(Imglist[i+1].getBound());
          Imglist[i].setFinish(Imglist[i+1].getFinish());
          Imglist[i].setZero(Imglist[i+1].getZero().x(),Imglist[i+1].getZero().y());
          Imglist[i].setPos(Imglist[i+1].getPos());
                                                    //перезаписываемый массив
      }
    if(exist){
      countImg--;
    }

     ui ->listWidget->clear();
  for(int i = 0; i < countImg; ++i){
      QListWidgetItem *item = new QListWidgetItem;
      item->setText(Imglist[i].getName());
      ui->listWidget->addItem(item);
      ui ->listWidget->show();
}
}
}


void MainWindow::on_pushButton_4_clicked() //bound
{
    ui->pushButton_4->setChecked(true);
    ui->pushButton_3->setChecked(false);
    ui->pushButton_2->setChecked(false);
    ui->pushButton_6->setChecked(false);
    ui->pushButton_axes->setChecked(false);
    ui->pushButton_MRI->setChecked(false);
    ui->pushButton_UI->setChecked(false);
    ui->pushButton_histo->setChecked(false);

    if (scene!=nullptr){
        delete scene;
        scene=nullptr;
    }
    scene2=new boardScene();
    Imglist[currentImageItem].tumor_update = 0;
    scene2->currentPen(Qt::red);
    scene2->addPixmap(QPixmap::fromImage(Imglist[currentImageItem].getCrop()));
    ui->graphicsView->setScene(scene2);
}


void MainWindow::on_pushButton_5_clicked() //save board
{
if(ui->pushButton_6 -> isChecked()){
    ui->pushButton_6->setChecked(false);
    ui->pushButton_3->setChecked(false);
    ui->pushButton_4->setChecked(false);
    ui->pushButton_2->setChecked(false);
    ui->pushButton_axes->setChecked(false);
    ui->pushButton_MRI->setChecked(false);
    ui->pushButton_UI->setChecked(false);
    ui->pushButton_histo->setChecked(false);

    if(scene2 != nullptr){
    scene2->clearSelection();                                                  // Selections would also render to the file
    scene2->setSceneRect(scene2->itemsBoundingRect());                          // Re-shrink the scene to it's bounding contents
    QImage image(scene2->sceneRect().size().toSize(), QImage::Format_ARGB32);  // Create the image with the exact size of the shrunk scene
    image.fill(Qt::transparent);                                              // Start all pixels transparent
    QPixmap pix;
    pix = QPixmap::fromImage(Imglist[currentImageItem].getFinish());

    qDebug()<< "saving...";
    QPainter painter(&image);
    scene2->render(&painter);

    image.save("без поворота.png", "PNG");

    QTransform trans;
    trans.rotate(ui->horizontalScrollBar->value());
    QImage pic = image.transformed(trans);
 //   Imglist[currentImageItem].setCrop(pic);
    Imglist[currentImageItem].setBound(pic);
    Imglist[currentImageItem].setFinish(pic);
    pic.save("поворот", "PNG");

    int d=ui->spinBox->value(), tmp_count = 0;
    float massX=0.0, massY=0.0;
    QImage B = Imglist[currentImageItem].getBound(), C=B;
    C.fill(Qt::transparent);
    for (int i=0;i<B.width();++i){
        for (int j=0;j<B.height();++j){
            QRgb srgb0 = B.pixel(i,j);
       //     QRgb srgb1=qRgb(0,0,255);
             if(srgb0==qRgb(255,0,0))
             {board[currentBoardPixel][0] = i;
              board[currentBoardPixel][1] = j;
              board[currentBoardPixel][2] = currentBoardDepth;
qDebug()<<currentBoardPixel<<" "<<board[currentBoardPixel][0]<<" "<< board[currentBoardPixel][1]<<" "<< board[currentBoardPixel][2];
              currentBoardPixel++;
              C.setPixel(i,j,srgb0);
              massX += i;
              massY += j;
              tmp_count ++;
             }
             else{
                 if(srgb0==qRgb(0,255,0)){
                     tumor[currentTumorPixel][0] = i;
                     tumor[currentTumorPixel][1] = j;
                     tumor[currentTumorPixel][2] = currentBoardDepth;
                     qDebug()<<currentTumorPixel<<" "<<tumor[currentTumorPixel][0]<<" "<< tumor[currentTumorPixel][1]<<" "<<tumor[currentTumorPixel][2];
                     currentTumorPixel++;
                     C.setPixel(i,j,srgb0);
                 }
                 else C.setPixel(i,j,srgb0);
             }
           }}

        currentBoardDepth += d;

        Imglist[currentImageItem].setZero(int(massX/tmp_count), int(massY/tmp_count));

qDebug()<<Imglist[currentImageItem].getZero()<< " "<<massX<<massY;

        scene1=new QGraphicsScene();
        scene1->addPixmap(QPixmap::fromImage(C));
        ui->graphicsView->setScene(scene1);
        ui->pushButton_4->setChecked(false);
        ui->horizontalScrollBar->setValue(0);
    }
}
}


void MainWindow::on_spinBox_valueChanged()
{}


void MainWindow::on_pushButton_6_clicked()   // tumor
{
    if(ui->pushButton_4->isChecked()){
    ui->pushButton_3->setChecked(false);
    ui->pushButton_4->setChecked(false);
    ui->pushButton_2->setChecked(false);
    ui->pushButton_axes->setChecked(false);
    ui->pushButton_MRI->setChecked(false);
    ui->pushButton_UI->setChecked(false);
    ui->pushButton_histo->setChecked(false);

    if (scene!=nullptr){
        delete scene;
        scene=nullptr;
    }
    scene2->currentPen(Qt::green);

    qDebug()<<ui->pushButton_6->isChecked();

    if(Imglist[currentImageItem].tumor_update){
    QImage B = Imglist[currentImageItem].getBound();

    delete scene2;
    scene2 = new boardScene();
    scene2->currentPen(Qt::green);
    scene2->addPixmap(QPixmap::fromImage(B));
    ui->graphicsView->setScene(scene2);
    qDebug()<<ui->pushButton_6->isChecked()<<"update";
    ui->pushButton_6->setChecked(false);

    }
    else {
        scene2->clearSelection();                                                  // Selections would also render to the file
        scene2->setSceneRect(scene2->itemsBoundingRect());                          // Re-shrink the scene to it's bounding contents
        QImage image(scene2->sceneRect().size().toSize(), QImage::Format_ARGB32);
        image.fill(Qt::transparent);
        QPainter painter(&image);
        scene2->render(&painter);
        Imglist[currentImageItem].setBound(image);

        Imglist[currentImageItem].tumor_update = true;
    ui->pushButton_6->setChecked(true);

    }
}
}


void MainWindow::on_horizontalScrollBar_valueChanged(int value)
{

    ui->label_3->setNum(value);

    if(ui->pushButton_axes->isChecked()&&currentImageItem>0)
    { qDebug()<<"axes checked";

        delete scene1;
        scene1 = nullptr;
        scene1=new QGraphicsScene();
        scene1->addPixmap(QPixmap::fromImage(Imglist[currentImageItem].getFinish()))/*->transform().rotate(value)*/;

    QImage image(scene1->sceneRect().size().toSize(), QImage::Format_ARGB32);  // Create the image with the exact size of the shrunk scene
    image.fill(Qt::transparent);                                              // Start all pixels transparent
    QPixmap pix;
    pix = QPixmap::fromImage(Imglist[currentImageItem].getFinish());
    QPainter painter(&image);
    scene1->render(&painter);

    image.save("без поворота.png", "PNG");

    QTransform trans;
    trans.rotate(value);
    QImage pic = image.transformed(trans);
    pic.save("поворот", "PNG");


    QImage image1 = Imglist[currentImageItem-1].getFinish();
    image1 = image1.convertToFormat(QImage::Format_ARGB32);
     QImage* ptr = &image1;
     QImage image2;
    image2 = setOpacity(ptr, float(ui->horizontalScrollBar_2->value())/100);
    pos1 = Imglist[currentImageItem-1].getPos() ;
    pos2 =  Imglist[0].getZero() - Imglist[currentImageItem].getZero();
    qDebug()<< pos1<<pos2;
    delete scene1;
    scene1 = nullptr;
    scene1=new QGraphicsScene();
    scene1->addPixmap(QPixmap::fromImage(pic))->setPos(pos2);
    scene1->addPixmap(QPixmap::fromImage(image2))->setPos(pos1);


    ui->graphicsView->setScene(scene1);
}

    else{
    qreal m = qreal(ui->verticalScrollBar->value())/50;
        ui->graphicsView->rotate(-value);
        QTransform transform;
        transform.scale(m,m);
        transform.rotate(value);
        ui->graphicsView->setTransform(transform);
    }

}


void MainWindow::on_label_3_linkActivated()
{
}


void MainWindow::on_pushButton_axes_clicked()
{
    ui->pushButton_6->setChecked(false);
    ui->pushButton_3->setChecked(false);
    ui->pushButton_4->setChecked(false);
    ui->pushButton_2->setChecked(false);
    ui->pushButton_axes->setChecked(true);
    ui->pushButton_MRI->setChecked(false);
    ui->pushButton_UI->setChecked(false);
    ui->pushButton_histo->setChecked(false);

    pos2 = {0,0};
    pos1 = Imglist[currentImageItem].getZero();
    axesX = pos1.x();
    axesY = pos1.y();

    qDebug()<<"checked..."<<ui->pushButton_4->isChecked();

    if (scene!=nullptr){
            delete scene;
            scene=nullptr;
        }
        if (scene2!=nullptr){
                delete scene2;
                scene2=nullptr;
            }

    axes(axesX, axesY);

    scene1=new QGraphicsScene();
    scene1->addPixmap(QPixmap::fromImage(Imglist[currentImageItem].getFinish()));

    if(currentImageItem>0){
    QImage image = Imglist[currentImageItem-1].getFinish();
    image = image.convertToFormat(QImage::Format_ARGB32);
     QImage* ptr = &image;
     QImage image2;
    image2 = setOpacity(ptr, float(ui->horizontalScrollBar_2->value())/100);
    pos1 = Imglist[currentImageItem-1].getPos() ;
    pos2 =  Imglist[0].getZero() - Imglist[currentImageItem].getZero();
    qDebug()<< pos1<<pos2;
    delete scene1;
    scene1 = nullptr;
    scene1=new QGraphicsScene();
    scene1->addPixmap(QPixmap::fromImage(Imglist[currentImageItem].getFinish()))->setPos(pos2);  // текущая в нулевом положении и перемещается
    scene1->addPixmap(QPixmap::fromImage(image2))->setPos(pos1);                                // предыдущая в полупрозрачном режиме, в заданном положении
    }

    ui->graphicsView->setScene(scene1);
}


                void axes(int x, int y)
                {
                    QImage B = Imglist[currentImageItem].getBound();
                    for (int i=0;i<B.width();++i){
                        for (int j=0;j<B.height();++j){
                            QRgb srgb0 = B.pixel(i,j);
                            QRgb srgb1=qRgb(255,255,0);
                             if(i == x || j == y)
                             {
                              B.setPixel(i,j,srgb1);
                             }
                             else{
                                 B.setPixel(i,j,srgb0);

                                 }
                    }}
                    Imglist[currentImageItem].setFinish(B);

                }

void MainWindow::on_pushButton_left_clicked()
{
//    axesX --;
//    axes(axesX, axesY);

    scene1=new QGraphicsScene();
    scene1->addPixmap(QPixmap::fromImage(Imglist[currentImageItem].getFinish()));

    if(currentImageItem>0){
    QImage image = Imglist[currentImageItem-1].getFinish();
    image = image.convertToFormat(QImage::Format_ARGB32);
     QImage* ptr = &image;
     QImage image2;
    image2 = setOpacity(ptr, float(ui->horizontalScrollBar_2->value())/100);
    pos2.setX(pos2.x()-1);
 //   center(pos1, pos2);
    qDebug()<< pos1<<pos2;
    delete scene1;
    scene1 = nullptr;
    scene1=new QGraphicsScene();
    scene1->addPixmap(QPixmap::fromImage(Imglist[currentImageItem].getFinish()))->setPos(pos2);
    scene1->addPixmap(QPixmap::fromImage(image2))->setPos(pos1);
    }

    ui->graphicsView->setScene(scene1);
}


void MainWindow::on_pushButton_right_clicked()
{
//    axesX ++;
//    axes(axesX, axesY);

    scene1=new QGraphicsScene();
    scene1->addPixmap(QPixmap::fromImage(Imglist[currentImageItem].getFinish()));

    if(currentImageItem>0){
    QImage image = Imglist[currentImageItem-1].getFinish();
    image = image.convertToFormat(QImage::Format_ARGB32);
     QImage* ptr = &image;
     QImage image2;
    image2 = setOpacity(ptr, float(ui->horizontalScrollBar_2->value())/100);
    pos2.setX(pos2.x()+1);
 //   center(pos1, pos2);
    qDebug()<< pos1<<pos2;
    delete scene1;
    scene1 = nullptr;
    scene1=new QGraphicsScene();
    scene1->addPixmap(QPixmap::fromImage(Imglist[currentImageItem].getFinish()))->setPos(pos2);
    scene1->addPixmap(QPixmap::fromImage(image2))->setPos(pos1);
    }

    ui->graphicsView->setScene(scene1);
}


void MainWindow::on_pushButton_up_clicked()
{
//    axesY --;
//    axes(axesX, axesY);

    scene1=new QGraphicsScene();
    scene1->addPixmap(QPixmap::fromImage(Imglist[currentImageItem].getFinish()));

    if(currentImageItem>0){
    QImage image = Imglist[currentImageItem-1].getFinish();
    image = image.convertToFormat(QImage::Format_ARGB32);
     QImage* ptr = &image;
     QImage image2;
    image2 = setOpacity(ptr, float(ui->horizontalScrollBar_2->value())/100);
    pos2.setY(pos2.y()-1);
//    center(pos1, pos2);
    qDebug()<< pos1<<pos2;
    delete scene1;
    scene1 = nullptr;
    scene1=new QGraphicsScene();
    scene1->addPixmap(QPixmap::fromImage(Imglist[currentImageItem].getFinish()))->setPos(pos2);
    scene1->addPixmap(QPixmap::fromImage(image2))->setPos(pos1);
    }

    ui->graphicsView->setScene(scene1);
}


void MainWindow::on_pushButton_down_clicked()
{

//    axesY ++;
//    axes(axesX, axesY);

    scene1=new QGraphicsScene();
    scene1->addPixmap(QPixmap::fromImage(Imglist[currentImageItem].getFinish()));

    if(currentImageItem>0){
    QImage image = Imglist[currentImageItem-1].getFinish();
    image = image.convertToFormat(QImage::Format_ARGB32);
     QImage* ptr = &image;
     QImage image2;
    image2 = setOpacity(ptr, float(ui->horizontalScrollBar_2->value())/100);
    pos2.setY(pos2.y()+1);
//    center(pos1, pos2);
    qDebug()<< pos1<<pos2;
    delete scene1;
    scene1 = nullptr;
    scene1=new QGraphicsScene();
    scene1->addPixmap(QPixmap::fromImage(Imglist[currentImageItem].getFinish()))->setPos(pos2);
    scene1->addPixmap(QPixmap::fromImage(image2))->setPos(pos1);
    }

    ui->graphicsView->setScene(scene1);
}

void MainWindow::on_pushButton_7_clicked()  //save position
{
    ui->pushButton_2->setChecked(false);
    ui->pushButton_3->setChecked(false);
    ui->pushButton_4->setChecked(false);
    ui->pushButton_6->setChecked(false);
    ui->pushButton_axes->setChecked(false);
    ui->pushButton_MRI->setChecked(false);
    ui->pushButton_UI->setChecked(false);
    ui->pushButton_histo->setChecked(false);

    if(currentImageItem>0){
    QImage pix;
    pix = Imglist[currentImageItem].getFinish();

    QTransform trans;
    trans.rotate(ui->horizontalScrollBar->value());
    pix = pix.transformed(trans,Qt::SmoothTransformation);
 //   Imglist[currentImageItem].setCrop(pic);
    Imglist[currentImageItem].setBound(pix);
    Imglist[currentImageItem].setFinish(pix);
    pix.save("поворот1", "PNG");

    int z_old=1, imagelist_count=0;
    Imglist[currentImageItem].setPos(pos2/*-Imglist[currentImageItem-1].getPos()*/);
    qDebug()<<pos1<<pos2<<Imglist[currentImageItem].getPos();
//    Imglist[currentImageItem].setZero(Imglist[currentImageItem].getZero().x()+Imglist[currentImageItem].getPos().x(),Imglist[currentImageItem].getZero().y()+Imglist[currentImageItem].getPos().y());
qDebug()<<"=="<<Imglist[0].getZero()<<Imglist[1].getZero()<<Imglist[2].getZero();
    QFile f("output_board.csv");
    if(f.open( QIODevice::WriteOnly)){
        QTextStream ts(&f);
        ts<<"Bound"<<";"<<"x"<<";"<<"y"<<";"<<"z"<<";"<<"Zero_y"<<";"<<"Zero_x"<<"\n";
            for (int i=0;i<currentBoardPixel;++i){

                    if(z_old != board[i][2]){
                        imagelist_count++;
                    }

                    if(imagelist_count>0){
                        if(Imglist[imagelist_count].getPos().x()>=0) {ts<<i<<";"<<board[i][0]+Imglist[imagelist_count].getPos().x()<<";";}
                        else {ts<<i<<";"<<board[i][0]+Imglist[imagelist_count].getPos().x()<<";";}
qDebug()<<imagelist_count<<Imglist[imagelist_count].getPos();
                        if(Imglist[imagelist_count].getPos().y()>=0){   ts<<board[i][1]+Imglist[imagelist_count].getPos().y()<<";";}
                        else {ts<<board[i][1]+Imglist[imagelist_count].getPos().y()<<";";}
                    ts<<board[i][2]<<";"<<Imglist[imagelist_count].getZero().y()<<";"<<Imglist[imagelist_count].getZero().x()<<"\n";
                       }

                    if(imagelist_count==0){
                    ts<<i<<";"<<board[i][0]<<";";
                    ts<<board[i][1]<<";";
                    ts<<board[i][2]<<";"<<Imglist[imagelist_count].getZero().y()<<";"<<Imglist[imagelist_count].getZero().x()<<"\n";
                    }
                    z_old = board[i][2];
             }
        f.close();
    }

    z_old=tumor[1][2];
    imagelist_count=(tumor[1][2] - 1) / ui->spinBox->value();
    QFile t("output_tumor.csv");
    if(t.open(QIODevice::WriteOnly)){
        QTextStream ts(&t);
        ts<<"Tumor"<<";"<<"x"<<";"<<"y"<<";"<<"z"<<"\n";
            for (int i=0;i<currentTumorPixel;++i){
                if(z_old != tumor[i][2]){
                    imagelist_count++;
                }

                if(imagelist_count>=(tumor[1][2] - 1) / ui->spinBox->value()){
                ts<<i<<";"<<tumor[i][0]+Imglist[imagelist_count].getPos().x()<<";";
                ts<<tumor[i][1]+Imglist[imagelist_count].getPos().y()<<";";
                ts<<tumor[i][2]<<"\n";
                }

                z_old = tumor[i][2];
             }
        t.close();
    }
}
}

void MainWindow::on_horizontalScrollBar_2_valueChanged(int value) //transparency
{
    scene1=new QGraphicsScene();
    scene1->addPixmap(QPixmap::fromImage(Imglist[currentImageItem].getFinish()));

    if(currentImageItem>0){

    QImage image = Imglist[currentImageItem-1].getFinish();
    image = image.convertToFormat(QImage::Format_ARGB32);
     QImage* ptr = &image;
     QImage image2;
    image2 = setOpacity(ptr, float(value)/100);
    pos1 = Imglist[currentImageItem-1].getPos() ;
    pos2 =  Imglist[0].getZero() - Imglist[currentImageItem].getZero();
    qDebug()<< pos1<<pos2;
    delete scene1;
    scene1 = nullptr;
    scene1=new QGraphicsScene();
    scene1->addPixmap(QPixmap::fromImage(Imglist[currentImageItem].getFinish()))->setPos(pos2);  // текущая в нулевом положении и перемещается
    scene1->addPixmap(QPixmap::fromImage(image2))->setPos(pos1);                                // предыдущая в полупрозрачном режиме, в заданном положении
    }

    ui->graphicsView->setScene(scene1);
}


            QImage setOpacity(QImage* ptr, qreal opacity){
            QImage image = * ptr;

            QImage newImg(image.size(), QImage::Format_ARGB32);
            newImg.fill(Qt::transparent);
            QPainter painter(&newImg);
            painter.setOpacity(opacity);
            painter.drawImage(QRect(0, 0, image.width(), image.height()), image);

            return newImg;
            }



void MainWindow::on_checkBox_stateChanged()
{
    ui->checkBox->setCheckable(true);
}


void MainWindow::on_pushButton_UI_clicked()
{

        UI_exist = true;
    QFile f("output_UI_b.csv");
    if(f.exists()){
        f.remove();
    }
    QFile b("output_board.csv");
    b.copy("output_UI_b.csv");

    QFile t("output_UI_t.csv");
    if(t.exists()){
        t.remove();
    }
    QFile u("output_tumor.csv");
    u.copy("output_UI_t.csv");

    int z_old=1;
    int imagelist_count=0;
    for (int i=0;i<currentBoardPixel;++i){
            pixel_UI_b = currentBoardPixel;
            if(z_old != board[i][2]){
                imagelist_count++;
            }

            if(imagelist_count>0){
                board_UI[i][0] = board[i][0]+Imglist[imagelist_count].getPos().x();
//    qDebug()<<imagelist_count<<Imglist[imagelist_count].getPos();
                board_UI[i][1] = board[i][1]+Imglist[imagelist_count].getPos().y();
                board_UI[i][2] = board[i][2];
               }

            if(imagelist_count==0){
            board_UI[i][0] = board[i][0];
            board_UI[i][1] = board[i][1];
            board_UI[i][2] = board[i][2];

            }
            z_old = board[i][2];
     }

    QPoint shift_UI = {0,0};
    for (int i=0;i<currentBoardPixel;++i){
        if(board_UI[i][0]< shift_UI.x()) {shift_UI.setX(board_UI[i][0]);}
        if(board_UI[i][1]< shift_UI.y()) {shift_UI.setY(board_UI[i][1]);}
        qDebug()<<"--="<<shift_UI;
    }
    shift_UI.setX(abs(shift_UI.x()));
    shift_UI.setY(abs(shift_UI.y()));

    for (int i=0;i<currentBoardPixel;++i){
            pixel_UI_b = currentBoardPixel;
            if(z_old != board[i][2]){
                imagelist_count++;
            }
            if(imagelist_count>0){
                board_UI[i][0] += shift_UI.x();
//    qDebug()<<imagelist_count<<Imglist[imagelist_count].getPos();
                board_UI[i][1] += shift_UI.y();
               }

            if(imagelist_count==0){
            board_UI[i][0] += shift_UI.x();
            board_UI[i][1] += shift_UI.y();
            }
            z_old = board[i][2];
     }

    z_old=tumor[1][2];
    imagelist_count=(tumor[1][2] - 1) / ui->spinBox->value();

    for (int i=0;i<currentTumorPixel;++i){
        pixel_UI_t = currentTumorPixel;
        if(z_old != tumor[i][2]){
            imagelist_count++;
        }

        if(imagelist_count>=(tumor[1][2] - 1) / ui->spinBox->value()){
           tumor_UI[i][0] = tumor[i][0]+Imglist[imagelist_count].getPos().x()+shift_UI.x();
        qDebug()<<imagelist_count<<Imglist[imagelist_count].getPos();
           tumor_UI[i][1] = tumor[i][1]+Imglist[imagelist_count].getPos().y()+shift_UI.y();
        tumor_UI[i][2] = tumor[i][2];
            }
        z_old = tumor[i][2];
     }

    ui->pushButton_4->setChecked(false);
    ui->pushButton_3->setChecked(false);
    ui->pushButton_2->setChecked(false);
    ui->pushButton_6->setChecked(false);
    ui->pushButton_axes->setChecked(false);
    ui->pushButton_MRI->setChecked(false);
    ui->pushButton_UI->setChecked(true);
    ui->pushButton_histo->setChecked(false);

    if (scene!=nullptr){
        delete scene;
        scene=nullptr;
    }

    QImage slice_UI(1000,1000, QImage::Format_ARGB32);
    slice_UI.fill(Qt::white);
    QRgb srgb = qRgb(255,0,0);
    for(int i = 0; i < currentBoardPixel; i++){
    slice_UI.setPixel(board_UI[i][1], board_UI[i][2], srgb);
    }
    scene2=new boardScene();
    scene2->currentPen(Qt::blue);
    scene2->addPixmap(QPixmap::fromImage(slice_UI));
    ui->graphicsView->setScene(scene2);
}


void MainWindow::on_pushButton_MRI_clicked()
{

        MRI_exist = true;
    QFile f("output_MRI_b.csv");
    if(f.exists()){
        f.remove();
    }
    QFile b("output_board.csv");
    b.copy("output_MRI_b.csv");

    QFile t("output_MRI_t.csv");
    if(t.exists()){
        t.remove();
    }
    QFile u("output_tumor.csv");
    u.copy("output_MRI_t.csv");


    int z_old=1;
    int imagelist_count=0;
    for (int i=0;i<currentBoardPixel;++i){
            pixel_MRI_b = currentBoardPixel;
            if(z_old != board[i][2]){
                imagelist_count++;
            }

            if(imagelist_count>0){
                board_MRI[i][0] = board[i][0]+Imglist[imagelist_count].getPos().x();
//    qDebug()<<imagelist_count<<Imglist[imagelist_count].getPos();
                board_MRI[i][1] = board[i][1]+Imglist[imagelist_count].getPos().y();
                board_MRI[i][2] = board[i][2];
               }

            if(imagelist_count==0){
            board_MRI[i][0] = board[i][0];
            board_MRI[i][1] = board[i][1];
            board_MRI[i][2] = board[i][2];

            }
            z_old = board[i][2];
     }

    QPoint shift_MRI = {0,0};
    for (int i=0;i<currentBoardPixel;++i){
        if(board_MRI[i][0]< shift_MRI.x()) {shift_MRI.setX(board_MRI[i][0]);}
        if(board_MRI[i][1]< shift_MRI.y()) {shift_MRI.setY(board_MRI[i][1]);}
        qDebug()<<"--="<<shift_MRI;
    }
    shift_MRI.setX(abs(shift_MRI.x()));
    shift_MRI.setY(abs(shift_MRI.y()));

    for (int i=0;i<currentBoardPixel;++i){
            pixel_MRI_b = currentBoardPixel;
            if(z_old != board[i][2]){
                imagelist_count++;
            }
            if(imagelist_count>0){
                board_MRI[i][0] += shift_MRI.x();
//    qDebug()<<imagelist_count<<Imglist[imagelist_count].getPos();
                board_MRI[i][1] += shift_MRI.y();
               }

            if(imagelist_count==0){
            board_MRI[i][0] += shift_MRI.x();
            board_MRI[i][1] += shift_MRI.y();
            }
            z_old = board[i][2];
     }

    z_old=tumor[1][2];
    imagelist_count=(tumor[1][2] - 1) / ui->spinBox->value();

    for (int i=0;i<currentTumorPixel;++i){
        pixel_MRI_t = currentTumorPixel;
        if(z_old != tumor[i][2]){
            imagelist_count++;
        }

        if(imagelist_count>=(tumor[1][2] - 1) / ui->spinBox->value()){
           tumor_MRI[i][0] = tumor[i][0]+Imglist[imagelist_count].getPos().x()+shift_MRI.x();
        qDebug()<<imagelist_count<<Imglist[imagelist_count].getPos();
           tumor_MRI[i][1] = tumor[i][1]+Imglist[imagelist_count].getPos().y()+shift_MRI.y();
        tumor_MRI[i][2] = tumor[i][2];
           }
        z_old = tumor[i][2];
     }

    ui->pushButton_4->setChecked(false);
    ui->pushButton_3->setChecked(false);
    ui->pushButton_2->setChecked(false);
    ui->pushButton_6->setChecked(false);
    ui->pushButton_axes->setChecked(false);
    ui->pushButton_MRI->setChecked(true);
    ui->pushButton_UI->setChecked(false);
    ui->pushButton_histo->setChecked(false);

    if (scene!=nullptr){
        delete scene;
        scene=nullptr;
    }

    QImage slice(1000,1000, QImage::Format_ARGB32);
    slice.fill(Qt::white);
    QRgb srgb = qRgb(255,0,0);
    for(int i = 0; i < currentBoardPixel; i++){
    slice.setPixel(board_MRI[i][1], board_MRI[i][2], srgb);
    }
    scene2=new boardScene();
    scene2->currentPen(Qt::blue);
    scene2->addPixmap(QPixmap::fromImage(slice));
    ui->graphicsView->setScene(scene2);
}


void MainWindow::on_pushButton_histo_clicked()
{
    if(histo_exist!=true){
        histo_exist = true;
    QFile f("output_histo_b.csv");
    if(f.exists()){
        f.remove();
    }
    QFile b("output_board.csv");
    b.copy("output_histo_b.csv");

    QFile t("output_histo_t.csv");
    if(t.exists()){
        t.remove();
    }
    QFile u("output_tumor.csv");
    u.copy("output_histo_t.csv");

    int z_old=1;
    int imagelist_count=0;
    for (int i=0;i<currentBoardPixel;++i){
            pixel_histo_b = currentBoardPixel;
            if(z_old != board[i][2]){
                imagelist_count++;
            }

            if(imagelist_count>0){
                board_histo[i][0] = board[i][0]+Imglist[imagelist_count].getPos().x();
//    qDebug()<<imagelist_count<<Imglist[imagelist_count].getPos();
                board_histo[i][1] = board[i][1]+Imglist[imagelist_count].getPos().y();
                board_histo[i][2] = board[i][2];
               }

            if(imagelist_count==0){
            board_histo[i][0] = board[i][0];
            board_histo[i][1] = board[i][1];
            board_histo[i][2] = board[i][2];

            }
            z_old = board[i][2];
     }

    QPoint shift_histo = {0,0};
    for (int i=0;i<currentBoardPixel;++i){
        if(board_histo[i][0]< shift_histo.x()) {shift_histo.setX(board_histo[i][0]);}
        if(board_histo[i][1]< shift_histo.y()) {shift_histo.setY(board_histo[i][1]);}
        qDebug()<<"--="<<shift_histo;
    }
    shift_histo.setX(abs(shift_histo.x()));
    shift_histo.setY(abs(shift_histo.y()));

    for (int i=0;i<currentBoardPixel;++i){
            pixel_histo_b = currentBoardPixel;
            if(z_old != board[i][2]){
                imagelist_count++;
            }
            if(imagelist_count>0){
                board_histo[i][0] += shift_histo.x();
//    qDebug()<<imagelist_count<<Imglist[imagelist_count].getPos();
                board_histo[i][1] += shift_histo.y();
               }

            if(imagelist_count==0){
            board_histo[i][0] += shift_histo.x();
            board_histo[i][1] += shift_histo.y();
            }
            z_old = board[i][2];
     }

    z_old=tumor[1][2];
    imagelist_count=(tumor[1][2] - 1) / ui->spinBox->value();

    for (int i=0;i<currentTumorPixel;++i){
        pixel_histo_t = currentTumorPixel;
        if(z_old != tumor[i][2]){
            imagelist_count++;
        }

        if(imagelist_count>=(tumor[1][2] - 1) / ui->spinBox->value()){
           tumor_histo[i][0] = tumor[i][0]+Imglist[imagelist_count].getPos().x()+shift_histo.x();
      //  qDebug()<<imagelist_count<<Imglist[imagelist_count].getPos();
           tumor_histo[i][1] = tumor[i][1]+Imglist[imagelist_count].getPos().y()+shift_histo.y();
        tumor_histo[i][2] = tumor[i][2];
           }
        z_old = tumor[i][2];
     }
}
    ui->pushButton_4->setChecked(false);
    ui->pushButton_3->setChecked(false);
    ui->pushButton_2->setChecked(false);
    ui->pushButton_6->setChecked(false);
    ui->pushButton_axes->setChecked(false);
    ui->pushButton_MRI->setChecked(false);
    ui->pushButton_UI->setChecked(false);
    ui->pushButton_histo->setChecked(true);

    if (scene!=nullptr){
        delete scene;
        scene=nullptr;
    }

    QImage slice_histo(1000,1000, QImage::Format_ARGB32);
    slice_histo.fill(Qt::white);
    QRgb srgb = qRgb(255,0,0);
    for(int i = 0; i < currentBoardPixel; i++){
    slice_histo.setPixel(board_histo[i][1], board_histo[i][2], srgb);
    }
    scene2=new boardScene();
    scene2->currentPen(Qt::blue);
    scene2->addPixmap(QPixmap::fromImage(slice_histo));
    ui->graphicsView->setScene(scene2);
}

double distance3d(int zero_x, int zero_y, int zero_z, int board_x, int board_y, int board_z)
{
    long double dist;
    dist = sqrtl((board_x-zero_x)*(board_x-zero_x) + (board_y-zero_y)*(board_y-zero_y) + (board_z-zero_z)*(board_z-zero_z));
    return dist;
}

double distance2d(int zero_x, int zero_y, int board_x, int board_y)
{
    double dist;
    dist = sqrtl((board_x-zero_x)*(board_x-zero_x) + (board_y-zero_y)*(board_y-zero_y));
    return dist;
}


void MainWindow::on_pushButton_8_clicked() //calculate
{   if(ui->pushButton_UI->isChecked() || ui->pushButton_MRI->isChecked() || ui->pushButton_histo->isChecked()){
    scene2->clearSelection();                                                  // Selections would also render to the file
    scene2->setSceneRect(scene2->itemsBoundingRect());                          // Re-shrink the scene to it's bounding contents
    QImage image(scene2->sceneRect().size().toSize(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    scene2->render(&painter);
//    layer = ui->spinBox_2->value();

    QImage B = image;
//    B.save("sceme.png", "PNG");


    if(ui->pushButton_UI->isChecked()){
        blue_count = 0;
        blue_min = {1000,1000}, blue_max = {0,0};

            for (int i=0;i<B.width();++i){
                for (int j=0;j<B.height();++j){
                    QRgb srgb0 = B.pixel(i,j);
                     if(srgb0==qRgb(0,0,255))
                     {
                         blue_line[blue_count][0] = i;
                         blue_line[blue_count][1] = j;
                         blue_count ++;
                      }}}

            blue_min = {blue_line[blue_count-1][0],blue_line[blue_count-1][1]}, blue_max = {blue_line[0][0],blue_line[0][1]};
            int eps=int(0.8 * blue_count *6), k = 0, old_k = 0, iter = 0;

          while(abs(eps) > 0.8 * blue_count*5 && iter <=100){
              int old_eps = eps;
              old_k = k;
              eps = 0;
            for(int i = 0; i< blue_count; i++){
                if(line(blue_max, blue_min, blue_line[i][0]).x() == 0){
                eps = eps + blue_line[i][1] - line(blue_min, blue_max, blue_line[i][0]).y();
                }
                else eps = eps + blue_line[i][0] - blue_max.x();
            }
            if(abs(eps) > 0.8 * blue_count && abs(old_eps) >= abs(eps)){
                if(old_k < k) {old_k = k; k--;}
                else {old_k = k; k++;}
                 }
            if(abs(eps) > 0.8 * blue_count && abs(old_eps) < abs(eps)){
                if(old_k < k) {old_k = k; k++;}
                else {old_k = k; k--;}
            }
            blue_max.setY(blue_max.y() - old_k + k);
            blue_min.setY(blue_min.y() - old_k + k);
            iter ++;
            if(iter == 100){
                break;
             }
             }
        QPoint center = {(blue_max.x()+blue_min.x())/2, (blue_max.y()+blue_min.y())/2};
        QPoint perp = {center.x()+10, center.y()+10};
        double d = 100.0, old_d = 150;
        for(int i = center.x()-100; i<center.x()+100; i++){
           d = abs(distance2d(blue_max.x(), blue_max.y(), i, center.y()+10) - distance2d(blue_min.x(), blue_min.y(), i, center.y() + 10));
            if(d<1 && d<old_d){
                perp = {i, center.y()+10};
            }
            old_d = d;
          }
            int a=0, b=0;
            float tmp = 1, old_tmp=1;
            for (int i = 1; i < B.height(); i++ ) {
                old_tmp = tmp;
                tmp = line(center, perp, i).y();
                if((tmp>0 && old_tmp<=0)||(tmp<0 && old_tmp>=0)) {a = i;}
                if(tmp>B.height() && old_tmp <= B.height()) {b = i-1;}
                if(i == 599 && b == 0) {
                    if(old_tmp <= tmp) b = 599;
                }       }
            if (a>b) {a += b; b = a; a -= b;}

            float d_old= 10;
            for(int i = 0; i<countImg;i++){
            Imglist[i].setPerp(1, 0);
            Imglist[i].setPerp(2, 0);
            }
          for(int i = 0; i<pixel_UI_b; i++){
              if(board_UI[i][1] > a && board_UI[i][1] <b){
              int x = board_UI[i][1], y = board_UI[i][2], perp_x = x, perp_y = line(center, perp, x).y(), depth = ui->spinBox->value();
              double d = abs(distance2d(perp_x, perp_y, x,y));

              if(d <= 3){
                  if(board_UI[i][2] != board_UI[i-1][2]) {d_old = 10;}
                  if(d<=d_old){
                 Imglist[(board_UI[i][2] - 1)/depth].setPerp(1, x);
                 Imglist[(board_UI[i][2] - 1)/depth].setPerp(2, y);
                 Imglist[(board_UI[i][2] - 1)/depth].setPerp(0, Imglist[0].getZero().x());
                  d_old = d;
                  }        }
          }   }


        for(int i = 0; i < pixel_UI_t; ++i)
         {
             mass_center_UI[0] += tumor_UI[i][0];
             mass_center_UI[1] += tumor_UI[i][1];
             mass_center_UI[2] += tumor_UI[i][2];
         }
         mass_center_UI[0] /= pixel_UI_t;
         mass_center_UI[1] /= pixel_UI_t;
         mass_center_UI[2] /= pixel_UI_t;
            d = 1000;
            QVector3D p{0,0,0};
         for(int i = 0; i<pixel_UI_b; i++){
             p.setY(board_UI[i][1]);
             p.setZ(line(center, perp, board_UI[i][1]).y());
             old_d = d;
             d = abs(distance2d(p.y(), p.z(), mass_center_UI[1], mass_center_UI[2]));
             if(d < old_d){
                 p.setY(p.y());
                 p.setZ(p.z());
             }}
         p.setX(Imglist[0].getZero().x());

         QVector3D v_perp(0, center.x()-perp.x(), center.y() - perp.y());
         QVector3D m(mass_center_UI[0], mass_center_UI[1], mass_center_UI[2]);
         float angle_UI = angle(v_perp.x(), v_perp.y(), v_perp.z(), 0, 0, 1);
         float dist = 0, old_dist = 10;
         if(step!=0){
         for(int i = 1; i<pixel_UI_b ; i++){
             float z = plane(v_perp, m, 'z', board_UI[i][0], board_UI[i][1]);
             float angle_h = angle(mass_center_UI[0] - p.x(), mass_center_UI[1] - p.y(), mass_center_UI[2] - p.z(), board_UI[i][0] - p.x(), board_UI[i][1] - p.y(), z - p.z());
             dist = abs(angle_h - angle_histo);
             if(dist < 0.0001)
               { m.setX(board_UI[i][0]);
                 m.setY(board_UI[i][1]);
                 m.setZ(z);
             }
             old_dist = dist;
   //          qDebug()<<m.z()<<dist<<old_dist;
         }}

         QVector3D v_mass(m.x() - Imglist[0].getZero().x(), m.y()- perp.x(), m.z() - perp.y());
         QVector3D normal(QVector3D::normal(v_perp, v_mass));
         norm_UI = normal;
         m_UI = m;
//         m.setZ(m.z()+((ui->spinBox->value() * ui->spinBox_2->value())/sin(angle_UI)));

      scene2=new boardScene();
          QImage c_slice_UI(1200,1200, QImage::Format_ARGB32);
          QImage slice_UI(1200,1200, QImage::Format_ARGB32);
          QPainter povorot(&slice_UI);
          c_slice_UI.fill(Qt::white);
          QRgb srgb = qRgb(255,0,0);
          dist = 11;
          int count = 0, paint[10000][3];
          for(int i = 0; i<pixel_UI_b; i++){
             float z_plane = plane(normal, m, 'z', board_UI[i][0], board_UI[i][1]);
            old_dist = dist;
             float dist = abs(distance3d(board_UI[i][0], board_UI[i][1],board_UI[i][2], board_UI[i][0], board_UI[i][1], z_plane));

             if(dist<10 || dist<=old_dist){
                float planex = plane(normal, m, 'x', 0, 0);
                float norm_to = abs(distance3d(board_UI[i][0], board_UI[i][1],board_UI[i][2], planex,0,0));
                float alfa = angle(v_perp.x(), v_perp.y(), v_perp.z(), planex - board_UI[i][0], 0 - board_UI[i][1], 0 - board_UI[i][2]);
                int y, x;
                y = norm_to * cos(alfa), x = norm_to * sin(alfa);

qDebug()<<alfa *180 / 3.1415<<norm_to<<x<<y;

                 c_slice_UI.setPixel(x,y, srgb);

                 paint[count][0] = board_UI[i][0];
                 paint[count][1] = board_UI[i][1];
                 paint[count][2] = board_UI[i][2];
                 count ++;
              }
              }

          QVector3D shift = {0,0,0};
          for (int i=0;i<count;++i){
              if(paint[i][0]< shift.x()) {shift.setX(paint[i][0]);}
              if(paint[i][1]< shift.y()) {shift.setY(paint[i][1]);}
              qDebug()<<"--="<<shift;
          }
          shift.setX(abs(shift.x()));
          shift.setY(abs(shift.y()));
            count--;

          for(int j = count; j>=0; j--){
          paint[j][1] += shift.y();
          paint[j][0] += shift.x();
          }
          for(int i = 0; i < count; i++){
            for(int j = i+1; j<=count; j++){
                float d = distance2d(paint[j][0], paint[j][1],paint[i][0], paint[i][1]);
                if( d > 30){
              QLine p({paint[j][0], paint[j][1]},{paint[i][0], paint[i][1]});
              povorot.drawLine(p);
             }}}

           count = 0;
           dist = 11;
          for(int i = 0; i<pixel_UI_t; i++){
              float z_plane = plane(normal, m, 'z', tumor_UI[i][0], tumor_UI[i][1]);
             old_dist = dist;
              float dist = abs(distance3d(tumor_UI[i][0], tumor_UI[i][1],tumor_UI[i][2], tumor_UI[i][0], tumor_UI[i][1], z_plane));
               if(dist<10 || dist<=old_dist){
                  float planex = plane(normal, m, 'x', 0, 0);
                  float norm_to = abs(distance3d(tumor_UI[i][0], tumor_UI[i][1],tumor_UI[i][2], planex,0,0));
                  float alfa = angle(v_perp.x(), v_perp.y(), v_perp.z(), planex - tumor_UI[i][0], 0 - tumor_UI[i][1], 0 - tumor_UI[i][2]);

//                 if(alfa*180/3.1415 > 90) alfa = 3.1415 - alfa ;

                  int y = norm_to * cos(alfa), x = norm_to * sin(alfa);
qDebug()<<alfa *180 / 3.1415<<"norm"<<x<<y;
                  c_slice_UI.setPixel(x,y, qRgb(0,255,0));

                  paint[count][0] = tumor_UI[i][0];
                  paint[count][1] = tumor_UI[i][1];
                  paint[count][2] = tumor_UI[i][2];
                  count ++;
              }
          }

          count--;

          for(int j = count; j>=0; j--){
          paint[j][1] += shift.y();
          paint[j][0] += shift.x();
          }
          povorot.setPen(Qt::green);
          for(int i = 0; i < count; i++){
            for(int j = i+1; j<=count; j++){
                float d = distance2d(paint[j][0], paint[j][1],paint[i][0], paint[i][1]);
                if(d < 150 && d > 50){
              QLine p({paint[j][0], paint[j][1]},{paint[i][0], paint[i][1]});
              povorot.drawLine(p);
             }}}

          scene2->addPixmap(QPixmap::fromImage(slice_UI));
          ui->graphicsView->setScene(scene2);
          c_slice_UI.save("с_slice_UI.png", "PNG");
          slice_UI.save("slice_UI.png", "PNG");
      }


if(ui->pushButton_MRI->isChecked()){

    blue_count = 0;
    blue_min = {1000,1000}, blue_max = {0,0};

        for (int i=0;i<B.width();++i){
            for (int j=0;j<B.height();++j){
                QRgb srgb0 = B.pixel(i,j);
      //          QRgb srgb1= C.pixel(i,j);
                 if(srgb0==qRgb(0,0,255))
                 {
//                     if(i > blue_max.x()) blue_max = {i,j};         // находим минимум и максимум линии - через них проводим прямую
//                     if(i < blue_min.x()) blue_min = {i,j};
                     blue_line[blue_count][0] = i;
                     blue_line[blue_count][1] = j;          // 0 - y, 1 - z
                     blue_count ++;
                  }
        }
        }

        blue_min = {blue_line[blue_count-1][0],blue_line[blue_count-1][1]}, blue_max = {blue_line[0][0],blue_line[0][1]};

        //* находим центр отрезка
            //* через центр под 90гр проводим прямую
            // по этому уравнению прямой находим самый близкий у
            // для этого у рассчитываем средний х
            // с этим х составляем уравнение прямой в 3д
            // смотрим лежит ли на ней центр масс

        // функция для уравнения прямой по 2м точкам (или по усреднению массива синих точек)
        // функция для 2д перпендикулярной прямой
        qDebug()<<"<"<< blue_max<<blue_min;
      int eps=int(0.8 * blue_count *6), k = 0, old_k = 0, iter = 0;
//      qDebug()<<line(blue_min, blue_max, blue_max.x()).y()<<line(blue_min, blue_max, blue_min.x()).y()<<line(blue_min, blue_max, blue_line[20][0]).y()<<blue_line[20][1]<<blue_count;


      while(abs(eps) > 0.8 * blue_count*5 && iter <=100){
          int old_eps = eps;
          old_k = k;
          eps = 0;
        for(int i = 0; i< blue_count; i++){
            if(line(blue_max, blue_min, blue_line[i][0]).x() == 0){
            eps = eps + blue_line[i][1] - line(blue_min, blue_max, blue_line[i][0]).y();
        //    qDebug()<< eps<<blue_line[i][1] <<line(blue_min, blue_max, blue_line[i][0]).y();
            B.setPixel(blue_line[i][0],line(blue_min, blue_max, blue_line[i][0]).y(), qRgb(255,0,0));
            }
            else eps = eps + blue_line[i][0] - blue_max.x();
        }
        if(abs(eps) > 0.8 * blue_count && abs(old_eps) >= abs(eps)){
            if(old_k < k) {old_k = k; k--;}
            else {old_k = k; k++;}

         }
        if(abs(eps) > 0.8 * blue_count && abs(old_eps) < abs(eps)){
            if(old_k < k) {old_k = k; k++;}
            else {old_k = k; k--;}
        }


        blue_max.setY(blue_max.y() - old_k + k);
        blue_min.setY(blue_min.y() - old_k + k);
        iter ++;
        if(iter == 100){
            break;
//            QMessageBox msg;
//            msg.setWindowTitle("Невыполнено");
//            msg.setText("Проведите прямую!");
//            msg.setIcon(QMessageBox::Warning);

//            msg.exec();
        }
 //     qDebug()<<eps<<"<"<< 0.8 * blue_count<<old_eps<<k;
        qDebug()<<"<<"<< blue_max<<blue_min;
      }
//qDebug()<<eps<<"<<"<< 0.8 * blue_count*2<<0.8*blue_count<<k<<iter;

    QPoint center = {(blue_max.x()+blue_min.x())/2, (blue_max.y()+blue_min.y())/2};
    QPoint perp = {center.x()+10, center.y()+10};
    double d = 100.0, old_d = 150;
    for(int i = center.x()-100; i<center.x()+100; i++){
       d = abs(distance2d(blue_max.x(), blue_max.y(), i, center.y()+10) - distance2d(blue_min.x(), blue_min.y(), i, center.y() + 10));
        if(d<1 && d<old_d){
            perp = {i, center.y()+10};
//            qDebug()<<d;
        }
        old_d = d;
      }

        int a=0, b=0;
        float tmp = 1, old_tmp=1;
        for (int i = 1; i < B.height(); i++ ) {
            old_tmp = tmp;
            tmp = line(center, perp, i).y();
            if((tmp>0 && old_tmp<=0)||(tmp<0 && old_tmp>=0)) {a = i;}
            if(tmp>B.height() && old_tmp <= B.height()) {b = i-1;}
            if(i == 599 && b == 0) {
                if(old_tmp <= tmp) b = 599;
            }

            qDebug()<<a<<b<<tmp<<old_tmp;
        }

        if (a>b) {a += b; b = a; a -= b;}
 qDebug()<<a<<b;
        float d_old= 10;
        for(int i = 0; i<countImg;i++){
        Imglist[i].setPerp(1, 0);
        Imglist[i].setPerp(2, 0);
        }
      for(int i = 0; i<pixel_MRI_b; i++){
          if(board_MRI[i][1] > a && board_MRI[i][1] <b){
          int x = board_MRI[i][1], y = board_MRI[i][2], perp_x = x, perp_y = line(center, perp, x).y(), depth = ui->spinBox->value();
          double d = abs(distance2d(perp_x, perp_y, x,y));

          if(d <= 3){
              B.setPixel(perp_x, perp_y, qRgb(0,0,255));
              if(board_MRI[i][2] != board_MRI[i-1][2]) {d_old = 10;}
              qDebug()<<"--"<< line(center, perp, B.width()).y()<< d<< d_old;
              if(d<=d_old){
             Imglist[(board_MRI[i][2] - 1)/depth].setPerp(1, x);
             Imglist[(board_MRI[i][2] - 1)/depth].setPerp(2, y);
             Imglist[(board_MRI[i][2] - 1)/depth].setPerp(0, Imglist[0].getZero().x());
             qDebug()<<(board_MRI[i][2] - 1)/depth<< d;
              B.setPixel( x, y, qRgb(255,255,0));
              d_old = d;
              }
          }
      }
      }
//      qDebug()<</*abs(perp_y - y)<<*/ pixel_MRI_b<<line(center, perp, 0).y();
//    for(int l = 0; l < countImg; l++){
//   B.setPixel( Imglist[l].getPerp(1),Imglist[l].getPerp(2), qRgb(0,255,0));
//    }
 //   B.setPixel((center-perp).x(), (center-perp).y(),qRgb(0,0,0));
 //   B.setPixel(10, 0, qRgb(0,0,0));
    QLine l(center, perp);
    scene2=new boardScene();
    scene2->addPixmap(QPixmap::fromImage(B));
    scene2->addLine(l, QPen(Qt::green));
    ui->graphicsView->setScene(scene2);
//     B.save("sceme.png", "PNG");

//     angle_MRI = angle((center-perp).x(), (center-perp).y(), 0, 1, 0, 0)*180 / 3.1415;
//     qDebug()<<angle_MRI;

    for(int i = 0; i < pixel_MRI_t; ++i)
     {
         mass_center_MRI[0] += tumor_MRI[i][0];
         mass_center_MRI[1] += tumor_MRI[i][1];
         mass_center_MRI[2] += tumor_MRI[i][2];
     }
     mass_center_MRI[0] /= pixel_MRI_t;
     mass_center_MRI[1] /= pixel_MRI_t;
     mass_center_MRI[2] /= pixel_MRI_t;

     d = 1000;
     QVector3D p{0,0,0};
  for(int i = 0; i<pixel_MRI_b; i++){
      p.setY(board_MRI[i][1]);
      p.setZ(line(center, perp, board_MRI[i][1]).y());
      old_d = d;
      d = abs(distance2d(p.y(), p.z(), mass_center_MRI[1], mass_center_MRI[2]));
      if(d < old_d){
          p.setY(p.y());
          p.setZ(p.z());
      }}
  p.setX(Imglist[0].getZero().x());

      QVector3D v_perp(0, center.x()-perp.x(), center.y() - perp.y());
      float angle_MRI = angle(v_perp.x(), v_perp.y(), v_perp.z(), 0, 0, 1);
      QVector3D p_center(Imglist[0].getZero().x(), center.x(), center.y());
      QVector3D m(mass_center_MRI[0], mass_center_MRI[1], mass_center_MRI[2]);
      float dist = 0, old_dist = 10;
      if(step!=0){
      for(int i = 1; i<pixel_MRI_b ; i++){
          float z = plane(v_perp, m, 'z', board_MRI[i][0], board_MRI[i][1]);
          float angle_h = angle(mass_center_MRI[0] - p.x(), mass_center_MRI[1] - p.y(), mass_center_MRI[2] - p.z(), board_MRI[i][0] - p.x(), board_MRI[i][1] - p.y(), z - p.z());
          dist = abs(angle_h - angle_histo);
          if(dist < 0.0001)
            { m.setX(board_MRI[i][0]);
              m.setY(board_MRI[i][1]);
              m.setZ(z);
          }
          old_dist = dist;
  //        qDebug()<<m.z()<<dist<<old_dist;
      }}

      QVector3D v_mass(m.x() - Imglist[0].getZero().x(), m.y() - perp.x(), m.z() - perp.y());
      QVector3D normal(QVector3D::normal(v_perp, v_mass));
      norm_MRI = normal;
      m_MRI = m;
//      m.setZ(m.z()+((ui->spinBox->value() * ui->spinBox_2->value())/sin(angle_MRI)));


scene2=new boardScene();
      QImage c_slice(1200,1200, QImage::Format_ARGB32);
      QImage slice_MRI(1200,1200, QImage::Format_ARGB32);
      QPainter povorot(&slice_MRI);
      c_slice.fill(Qt::white);
      QRgb srgb = qRgb(255,0,0);
      dist = 11;
      int count = 0, paint[10000][3];
      for(int i = 0; i<pixel_MRI_b; i++){
         float z_plane = plane(normal, m, 'z', board_MRI[i][0], board_MRI[i][1]);
        old_dist = dist;
         float dist = abs(distance3d(board_MRI[i][0], board_MRI[i][1],board_MRI[i][2], board_MRI[i][0], board_MRI[i][1], z_plane));
     //    c_slice.setPixel(board_MRI[i][0],board_MRI[i][2] , qRgb(0,255,0));

         if(dist<10 || dist<=old_dist){
            float planex = plane(normal, m, 'x', 0, 0);
            float norm_to = abs(distance3d(board_MRI[i][0], board_MRI[i][1],board_MRI[i][2], planex,0,0));
            float alfa = angle(v_perp.x(), v_perp.y(), v_perp.z(), planex - board_MRI[i][0], 0 - board_MRI[i][1], 0 - board_MRI[i][2]);
            int y, x;
            y = norm_to * cos(alfa), x = norm_to * sin(alfa);
             qDebug()<<alfa *180 / 3.1415<<norm_to<<x<<y;
             c_slice.setPixel(x,y, srgb);

 //            red[count] = i;
             paint[count][0] = x;
             paint[count][1] = y;
             paint[count][2] = board_MRI[i][2];
            count ++;
         }
    //    с_slice.setPixel(board_MRI[i][1], board_MRI[i][2], srgb);
      }

      count--;
      QVector3D shift = {0,0,0};
      for (int i=0;i<count;++i){
          if(paint[i][0]< shift.x()) {shift.setX(paint[i][0]);}
          if(paint[i][1]< shift.y()) {shift.setY(paint[i][1]);}
          qDebug()<<"--="<<shift;
      }
      shift.setX(abs(shift.x()));
      shift.setY(abs(shift.y()));

      for(int j = count; j>=0; j--){
      paint[j][1] += shift.y();
      paint[j][0] += shift.x();
      }
      for(int i = 0; i < count; i++){
        for(int j = i+1; j<=count; j++){
          if(distance2d(paint[j][0], paint[j][1],paint[i][0], paint[i][1]) < 100){
          QLine p({paint[j][0], paint[j][1]},{paint[i][0], paint[i][1]});
          povorot.drawLine(p);
         }}}


       count = 0;
       dist = 11;
      for(int i = 0; i<pixel_MRI_t; i++){
          float z_plane = plane(normal, m, 'z', tumor_MRI[i][0], tumor_MRI[i][1]);
         old_dist = dist;
          float dist = abs(distance3d(tumor_MRI[i][0], tumor_MRI[i][1],tumor_MRI[i][2], tumor_MRI[i][0], tumor_MRI[i][1], z_plane));
      //    c_slice.setPixel(board_MRI[i][0],board_MRI[i][2] , qRgb(0,255,0));
 //        qDebug()<<"tumor"<<dist;
          if(dist<10 || dist<=old_dist){
             float planex = plane(normal, m, 'x', 0, 0);
             float norm_to = abs(distance3d(tumor_MRI[i][0], tumor_MRI[i][1],tumor_MRI[i][2], planex,0,0));
             float alfa = angle(v_perp.x(), v_perp.y(), v_perp.z(), planex - tumor_MRI[i][0], 0 - tumor_MRI[i][1], 0 - tumor_MRI[i][2]);
             int y, x;
             y = norm_to * cos(alfa), x = norm_to * sin(alfa);
              qDebug()<<alfa *180 / 3.1415<<"norm"<<x<<y;
              c_slice.setPixel(x,y, qRgb(0,255,0));
//qDebug()<<count<<i<<x<<y<<dist<<old_dist;
 //             red[count] = i;
              paint[count][0] = x;
              paint[count][1] = y;
              paint[count][2] = tumor_MRI[i][2];
              count ++;
          }
      }
      count--;
//      shift = {0,0,0};
//      for (int i=0;i<count;++i){
//          if(paint[i][0]< shift.x()) {shift.setX(paint[i][0]);}
//          if(paint[i][1]< shift.y()) {shift.setY(paint[i][1]);}
//          qDebug()<<"--="<<shift;
//      }
//      shift.setX(abs(shift.x()));
//      shift.setY(abs(shift.y()));

      for(int j = count; j>=0; j--){
      paint[j][1] += shift.y();
      paint[j][0] += shift.x();
      }
      povorot.setPen(Qt::green);
      for(int i = 0; i < count; i++){
        for(int j = i+1; j<=count; j++){
          if(distance2d(paint[j][0], paint[j][1],paint[i][0], paint[i][1]) < 100){
          QLine p({paint[j][0], paint[j][1]},{paint[i][0], paint[i][1]});
          povorot.drawLine(p);
         }}}


      scene2->addPixmap(QPixmap::fromImage(c_slice));
      ui->graphicsView->setScene(scene2);
      c_slice.save("c_slice_MRI.png", "PNG");
      slice_MRI.save("slice_MRI.png", "PNG");

     }

if(ui->pushButton_histo->isChecked()){
        scene1=new QGraphicsScene();
        blue_count = 0;
        blue_min = {1000,1000}, blue_max = {0,0};
        z_slice = board_histo[pixel_histo_b/2][2];
        step = (board_histo[1][2] + board_histo[pixel_histo_b -1][2])/2;

        for(int i = 0; i<pixel_histo_b; i++){
            if(board_histo[i][1] > blue_max.x()){ blue_max.setX(board_histo[i][1]);}
        }
        blue_max.setY(board_histo[0][2]);
        blue_min.setY(board_histo[pixel_histo_b-1][2]);
        blue_min.setX(blue_max.x());

        QPoint center = {(blue_max.x()+blue_min.x())/2, (blue_max.y()+blue_min.y())/2};
        QPoint perp = {center.x()+10, center.y()};

            float d_old= 10;
            for(int i = 0; i<countImg;i++){
            Imglist[i].setPerp(1, 0);
            Imglist[i].setPerp(2, 0);
            }
          for(int i = 0; i<pixel_histo_b; i++){
              if(board_histo[i][1] > 0 && board_histo[i][1] <B.width()){
              int x = board_histo[i][1], y = board_histo[i][2], perp_x = x, perp_y = line(center, perp, x).y(), depth = ui->spinBox->value();
              double d = abs(distance2d(perp_x, perp_y, x,y));

              if(d <= 3){
                  if(board_histo[i][2] != board_histo[i-1][2]) {d_old = 10;}
                  if(d<=d_old){
                 Imglist[(board_histo[i][2] - 1)/depth].setPerp(1, x);
                 Imglist[(board_histo[i][2] - 1)/depth].setPerp(2, y);
                 Imglist[(board_histo[i][2] - 1)/depth].setPerp(0, Imglist[0].getZero().x());
                  d_old = d;
                  }        }
          }   }


        for(int i = 0; i < pixel_histo_t; ++i)
         {
             mass_center_histo[0] += tumor_histo[i][0];
             mass_center_histo[1] += tumor_histo[i][1];
             mass_center_histo[2] += tumor_histo[i][2];
         }
         mass_center_histo[0] /= pixel_histo_t;
         mass_center_histo[1] /= pixel_histo_t;
         mass_center_histo[2] /= pixel_histo_t;

         QVector3D p_center(Imglist[0].getZero().x(), center.x(), center.y());
          QVector3D v_perp(0, center.x()-perp.x(), center.y() - perp.y()), v_mass(mass_center_histo[0] - Imglist[0].getZero().x(), mass_center_histo[1] - perp.x(), mass_center_histo[2] - perp.y());
          QVector3D h_mass(mass_center_histo[0] - Imglist[0].getZero().x(), mass_center_histo[1] - perp.x(), z_slice - perp.y());
          QVector3D normal(QVector3D::normal(v_perp, h_mass));
          QVector3D m(mass_center_histo[0], mass_center_histo[1], z_slice);
angle_histo = angle(mass_center_histo[0] - p_center.x(), mass_center_histo[2] - p_center.z(), 0, board_histo[pixel_histo_b/2][0] - p_center.x(), board_histo[pixel_histo_b/2][2] - p_center.z(), 0);
if(angle_histo*180/3.1415>90) {angle_histo -= 3.1415;}
qDebug()<<angle_histo*180/3.1415<<mass_center_histo[0]<<mass_center_histo[1]<<mass_center_histo[2]<<normal;

      scene2=new boardScene();
          QImage c_slice_histo(1200,1200, QImage::Format_ARGB32);
          QImage slice_histo(1200,1200, QImage::Format_ARGB32);
          QPainter povorot(&slice_histo);
          c_slice_histo.fill(Qt::white);
          QRgb srgb = qRgb(255,0,0);
          float old_dist = 10, dist = 11;
          int count = 0, paint[10000][3];
          for(int i = 0; i<pixel_histo_b; i++){
             float z_plane = plane(normal, m, 'z', board_histo[i][0], board_histo[i][1]);
            old_dist = dist;
             float dist = abs(distance3d(board_histo[i][0], board_histo[i][1],board_histo[i][2], board_histo[i][0], board_histo[i][1], z_plane));

            if(dist<10 && dist<=old_dist){

                 float plane_z = plane(normal, m, 'z', -50, -250);
                 float norm_to = abs(distance3d(board_histo[i][0], board_histo[i][1],board_histo[i][2], 0,0,plane_z));
                 float alfa = angle(v_perp.x(), v_perp.y(), v_perp.z(), 0 - board_histo[i][0], 0 - board_histo[i][1],plane_z - board_histo[i][2]);
                 if(alfa*180/3.1415 > 90) alfa = alfa - 3.1415 ;
                 int y = norm_to * cos(alfa), x = norm_to * sin(alfa);
 qDebug()<<alfa *180 / 3.1415<<norm_to<<x<<y;
                 c_slice_histo.setPixel(x,y, srgb);

                 paint[count][0] = x;
                 paint[count][1] = y;
                 paint[count][2] = board_histo[i][2];

                 count ++;
             } }
          count--;
          for(int j = count; j>0; j--){
              if(abs(paint[count][2]-paint[j][2])<= ui->spinBox->value()){
              QLine p({paint[count][0], paint[count][1]},{paint[j-1][0], paint[j-1][1]});
              povorot.drawLine(p);
            } }
           count = 0;
           dist = 11;
          for(int i = 0; i<pixel_histo_t; i++){
              float z_plane = plane(normal, m, 'z', tumor_histo[i][0], tumor_histo[i][1]);
             old_dist = dist;
              float dist = abs(distance3d(tumor_histo[i][0], tumor_histo[i][1],tumor_histo[i][2], tumor_histo[i][0], tumor_histo[i][1], z_plane));
               if(dist<10 || dist<=old_dist){
                  float plane_z = plane(normal, m, 'z', -50, -250);
                  float norm_to = abs(distance3d(tumor_histo[i][0], tumor_histo[i][1],tumor_histo[i][2], 0,0, plane_z));
                  float alfa = angle(v_perp.x(), v_perp.y(), v_perp.z(), 0 - tumor_histo[i][0], 0 - tumor_histo[i][1], plane_z - tumor_histo[i][2]);
                  if(alfa*180/3.1415 > 90) alfa = alfa - 3.1415 ;
                  int y = norm_to * cos(alfa), x = norm_to * sin(alfa);
  qDebug()<<alfa *180 / 3.1415<<"norm"<<x<<y;
                  c_slice_histo.setPixel(x,y, qRgb(0,255,0));
                  paint[count][0] = x;
                  paint[count][1] = y;
                  paint[count][2] = tumor_histo[i][2];

                  count ++;
              }
          }
          count--;
          for(int j = count; j>0; j--){
              if(abs(paint[count][2]-paint[j][2])<= ui->spinBox->value()){
              QLine p({paint[count][0], paint[count][1]},{paint[j-1][0], paint[j-1][1]});
              povorot.setPen(Qt::green);
              povorot.drawLine(p);
           } }
          scene2->addPixmap(QPixmap::fromImage(c_slice_histo));
          ui->graphicsView->setScene(scene2);
          c_slice_histo.save("c_slice_histo.png", "PNG");
          slice_histo.save("slice_histo.png", "PNG");
      }

}
}

                    QPoint line(QPoint p1, QPoint p2, int x){
                    float k, b;
                    float x1 = p1.x(), y1 = p1.y(), x2 = p2.x(), y2 = p2.y();
                    //    qDebug()<<p1<<"*"<<p2;
                    QPoint y = {0,0};
                    if(x2!=x1&&y2!=y1) {
                        k = float((y2 - y1))/float((x2-x1));
                        b = y1 - (x1 * (y2-y1))/(x2-x1);
                        y.setY(int(k * x) + b);
                        y.setX(0);
                    //        qDebug()<<(y2 - y1)<<"/"<<(x2-x1);
                    // qDebug()<<k<<"*"<<x<<"+"<<b;
                    }
                    else {
                        if(x2==x1){
                            y.setX(1);
                            y.setY(0);
                        }
                        if(y2==y1){
                            y.setY(y1);
                            y.setX(0);
                        }
                    }
                    return y;
                    }


void MainWindow::on_pushButton_9_clicked() // compare
{
    QImage B(1200,1200, QImage::Format_ARGB32);
   B.fill(Qt::white);
    for(int i = 0; i < pixel_histo_b; i++){
        if(board_histo[i][2]==step){
            B.setPixel(board_histo[i][0],board_histo[i][1], qRgb(255,0,0));
        }
    }

    for(int i = 0; i < pixel_histo_t; i++){
        if(tumor_histo[i][2]==step){
            B.setPixel(tumor_histo[i][0],tumor_histo[i][1], qRgb(0,0,255));
        }
    }
    B.save("гиста.png","PNG");


    if(ui->checkBox_MRI->checkState()){
        scene1=new QGraphicsScene();
        QImage MRI("c_slice_MRI.png");
        scene1->addPixmap(QPixmap::fromImage(MRI));
         QImage* ptr = &B;
         QImage image2;
        image2 = setOpacity(ptr, float(ui->horizontalScrollBar_2->value())/100);
        scene1->addPixmap(QPixmap::fromImage(image2));

    }

    if(ui->checkBox_UI->checkState()){
        scene1=new QGraphicsScene();
        QImage UI("c_slice_UI.png");
        scene1->addPixmap(QPixmap::fromImage(UI));
         QImage* ptr = &B;
         QImage image2;
        image2 = setOpacity(ptr, float(ui->horizontalScrollBar_2->value())/100);
        scene1->addPixmap(QPixmap::fromImage(image2));

    }

    scene1->clearSelection();                                                  // Selections would also render to the file
    scene1->setSceneRect(scene1->itemsBoundingRect());                          // Re-shrink the scene to it's bounding contents
    QImage image(scene1->sceneRect().size().toSize(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    scene1->render(&painter);
    image.save("гиста.png","PNG");
 //   scene1->addPixmap(QPixmap::fromImage(slice));
    ui->graphicsView->setScene(scene1);



}

float angle(int line1x, int line1y, int line1z, int line2x, int line2y, int line2z){
   // в функцию отправляются направляющие вектора = координата конца - координата начала
    float cos = (line1x * line2x + line2y*line1y + line1z * line2z) / (sqrt(line1x*line1x + line1y*line1y + line1z*line1z)*sqrt(line2x*line2x + line2y*line2y + line2z*line2z));
    float a = acos(cos);

    return a;
}

float plane(QVector3D n, QVector3D m, char what, int a, int b){

    float A = n.x(), B = n.y(), C = n.z(), c;
    float xm = m.x(), ym = m.y(), zm = m.z();
    char x = 'x';
    // A(x-xm) + B(y-ym) + C(z-zm) = 0
    if(what == x){
        c = (A*xm - B*(a - ym) - C*(b - zm))/A;
    }
    if(what == 'y'){
        c = (-A*(a - xm) + B*ym - C*(b - zm))/B;
    }
    if(what == 'z'){
        c = (-A*(a - xm) - B*(b - ym) + C*zm)/C;
    }

    return c;
}


