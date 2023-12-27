#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QTreeWidget>

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
    void on_startButton_clicked();

    void on_stopButton_clicked();

    void on_refreshButton_clicked();

    void on_allUsersView_customContextMenuRequested(const QPoint &pos);

    void on_onlineUsersView_customContextMenuRequested(const QPoint &pos);

    void sendServiceMessage(QTreeWidgetItem *item);
    void makeUserModerator(QTreeWidgetItem *item);
    void kickUser(QTreeWidgetItem *item);
    void banUser(QTreeWidgetItem *item);

    void on_actionUser_info_triggered();

    void on_actionAbout_triggered();

    void on_actionServer_start_triggered();

    void on_actionServer_stop_triggered();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
