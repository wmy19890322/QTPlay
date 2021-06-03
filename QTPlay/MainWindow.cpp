#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Dexter.h"
#include <QMessageBox>
#include <QMediaPlayer>
#include <QFileDialog>
#include <QVideoWidget>
#include <QVBoxLayout>
#include <QTime>

#define SLIDER_MAX_LEN      1000

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // 初始化变量
    m_nVideoTime = 0;
    m_bSliderDown = false;
    m_nSliderValue = 0;

    // 设置控件
    ui->setupUi(this);
    ui->horizontalSlider->setMinimum(0);
    ui->horizontalSlider->setMaximum(SLIDER_MAX_LEN);
    ui->horizontalSlider->setValue(m_nSliderValue);
    connect(ui->openGLWidget, SIGNAL(SendTimeData(int64_t)), this, SLOT(ShowTime(int64_t)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 将子窗口发送来的数据显示到主窗口的textedit上
void MainWindow::ShowTime(int64_t curTime)
{
    QString str = TimeToString(curTime) + " / " + TimeToString(m_nVideoTime);
    ui->label_time->setText(str);
    if(!m_bSliderDown)
    {
        m_nSliderValue = (int)((double)curTime / (double)m_nVideoTime * (double)SLIDER_MAX_LEN);
        ui->horizontalSlider->setValue( m_nSliderValue );
    }
}

// 播放点击
void MainWindow::on_buttonPlay_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                        tr("选择视频文件"),
                                                        ".",
                                                        tr("video file(*.mp4);;"
                                                           "all file(*.*)"));

    if(fileName.length() > 0)
    {
        m_nVideoTime = ui->openGLWidget->Play(fileName.toStdString().c_str());
        QString str = "00:00:00.000 / " + TimeToString(m_nVideoTime);
        ui->label_time->setText(str);
        m_nSliderValue = 0;
        ui->horizontalSlider->setValue(m_nSliderValue);
    }
}


void MainWindow::on_horizontalSlider_sliderMoved(int position)
{

}


void MainWindow::on_horizontalSlider_sliderPressed()
{
    m_bSliderDown = true;
}


void MainWindow::on_horizontalSlider_sliderReleased()
{
    m_bSliderDown = false;
    ui->openGLWidget->Seek( (float)m_nSliderValue / (float)SLIDER_MAX_LEN );
}


void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    qDebug() << "value: " << value;
    if(m_nSliderValue != value) {
        m_nSliderValue = value;
    }
}

