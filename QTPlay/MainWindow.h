#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_buttonPlay_clicked();

    void on_horizontalSlider_sliderMoved(int position);

    void on_horizontalSlider_sliderPressed();

    void on_horizontalSlider_sliderReleased();

    void on_horizontalSlider_valueChanged(int value);

private:
    Ui::MainWindow *    ui;
    int64_t             m_nVideoTime;
    bool                m_bSliderDown;
    int                 m_nSliderValue;


public slots:
    // 显示时间
    void ShowTime(int64_t curTime);
};
#endif // MAINWINDOW_H
