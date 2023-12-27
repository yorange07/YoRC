#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "server.h"
#include "User.h"
#include "service_widgets.h"
#include <QtConcurrent/QtConcurrent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, SIGNAL( customContextMenuRequested(const QPoint &)), this, SLOT(on_allUsersView_customContextMenuRequested(const QPoint &)));
        connect(this, SIGNAL( customContextMenuRequested(const QPoint &)), this, SLOT(on_onlineUsersView_customContextMenuRequested(const QPoint &)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

Server* server = new Server();


void MainWindow::on_startButton_clicked()
{
    QtConcurrent::run([=]() { server -> start(12345); });
    ui -> logWindow->append("Start server");

    ui -> allUsersView -> clear();
    ui -> onlineUsersView -> clear();

    for (int i = 0; i < server -> getBaseUserList().size(); i++){
        QTreeWidgetItem* id = new QTreeWidgetItem(ui->allUsersView);
        id -> setText(0,server -> getBaseUserList()[i][0]);
        QTreeWidgetItem* login = new QTreeWidgetItem(ui->allUsersView);
        login -> setText(1,server -> getBaseUserList()[i][1]);
        QTreeWidgetItem* password = new QTreeWidgetItem(ui->allUsersView);
        password -> setText(2,server -> getBaseUserList()[i][2]);
        QTreeWidgetItem* name = new QTreeWidgetItem(ui->allUsersView);
        name -> setText(3,server -> getBaseUserList()[i][3]);
    }

    ui -> startButton -> setEnabled(0);
    ui -> stopButton -> setEnabled(1);
}

void MainWindow::on_stopButton_clicked()
{
   server -> stop();
   ui -> logWindow->append("Stop server");

   ui -> allUsersView -> clear();
   ui -> onlineUsersView -> clear();

   ui -> allCount -> setText("All Users Count:");
   ui -> onlineCount_2 -> setText("Online Users Count:");

   ui -> startButton -> setEnabled(1);
}

void MainWindow::on_refreshButton_clicked()
{
    ui -> allUsersView -> clear();
    ui -> onlineUsersView -> clear();
    ui -> allCount -> setText("All Users Count:");
    ui -> onlineCount_2 -> setText("Online Users Count:");

    for (int i = 0; i < server -> getBaseUserList().size(); i++){
        QTreeWidgetItem* item = new QTreeWidgetItem(ui->allUsersView);
        item -> setText(0,server -> getBaseUserList()[i][0]);
        item -> setText(1,server -> getBaseUserList()[i][1]);
        item -> setText(2,server -> getBaseUserList()[i][2]);
        item -> setText(3,server -> getBaseUserList()[i][3]);
    }

    ui -> allCount -> setText("All Users Count:" + QString::number(server -> getBaseUserList().size()));

    for (int i = 0; i < server -> getAllUsers().size(); i++){
        // Создание нового элемента дерева
        QTreeWidgetItem* item = new QTreeWidgetItem(ui->onlineUsersView);

        // Установка значения для колонки 'User ID' (преобразование int в QString)
        item->setText(0, QString::number(server->getAllUsers()[i].getUserId()));

        // Установка значения для колонки 'Login' (преобразование std::string в QString)
        item->setText(1, QString::fromStdString(server->getAllUsers()[i].getUserLogin()));

        // Установка значения для колонки 'Password' (преобразование std::string в QString)
        item->setText(2, QString::fromStdString(server->getAllUsers()[i].getUserPassword()));

        // Установка значения для колонки 'Name' (преобразование std::string в QString)
        item->setText(3, QString::fromStdString(server->getAllUsers()[i].getUserName()));

        // Установка значения для колонки 'Socket' (преобразование int в QString)
        item->setText(4, QString::number(server->getAllUsers()[i].getUserSocket()));
    }

    ui -> onlineCount_2 -> setText("Online Users Count:" + QString::number(server -> getAllUsers().size()));
}

void MainWindow::on_allUsersView_customContextMenuRequested(const QPoint &pos)
{
    QMenu menu(this);
    //auto infoA = menu.addAction(tr("Send a service message"));
    auto infoB = menu.addAction(tr("Make moderator"));
    //auto infoC = menu.addAction(tr("Kick"));
    auto infoD = menu.addAction(tr("Ban"));
    if ((ui -> allUsersView -> hasFocus())){
            //connect(infoA, &QAction::triggered, this, [this](){ sendServiceMessage(ui -> allUsersView -> currentItem()); update(); });
            connect(infoB, &QAction::triggered, this, [this](){ makeUserModerator(ui -> allUsersView -> currentItem()); update(); });
            //connect(infoC, &QAction::triggered, this, [this](){ kickUser(ui -> allUsersView -> currentItem()); update(); });
            connect(infoD, &QAction::triggered, this, [this](){ banUser(ui -> allUsersView -> currentItem()); update(); });
            menu.exec(QCursor::pos());
    }
}

void MainWindow::on_onlineUsersView_customContextMenuRequested(const QPoint &pos)
{
    QMenu menu(this);
    auto infoA = menu.addAction(tr("Send a service message"));
    //auto infoB = menu.addAction(tr("Make moderator"));
    auto infoC = menu.addAction(tr("Kick"));
    //auto infoD = menu.addAction(tr("Ban"));
    if ((ui -> onlineUsersView -> hasFocus())){
            connect(infoA, &QAction::triggered, this, [this](){ sendServiceMessage(ui -> onlineUsersView -> currentItem()); update(); });
            //connect(infoB, &QAction::triggered, this, [this](){ makeUserModerator(ui -> allUsersView -> currentItem()); update(); });
            connect(infoC, &QAction::triggered, this, [this](){ kickUser(ui -> onlineUsersView -> currentItem()); update(); });
            //connect(infoD, &QAction::triggered, this, [this](){ banUser(ui -> allUsersView -> currentItem()); update(); });
            menu.exec(QCursor::pos());
    }
}

void MainWindow::sendServiceMessage(QTreeWidgetItem *item)//Метод отправки служебного сообщения клиенту
{
    QString str;
    str = item -> text(4);
    SendServiceMessage* Message = new  SendServiceMessage(0,str,server);
    Message -> show();

};

void MainWindow::makeUserModerator(QTreeWidgetItem *item)//Метод возведения клиента в модераторы
{
    QString str;
    str = item -> text(0);
    int id = str.toInt();
    server -> modClient(id, 1);

};

void MainWindow::kickUser(QTreeWidgetItem *item)//Метод кика клиента
{
    QString str;
    str = item -> text(4);
    int clientSocket = str.toInt();
    server -> logoutClient(clientSocket);

};

void MainWindow::banUser(QTreeWidgetItem *item)//Метод бана клиента
{
    QString str;
    str = item -> text(0);
    int id = str.toInt();
    server -> banClient(id, 1);
};

void MainWindow::on_actionUser_info_triggered()
{
    AboutRef* aboutref = new AboutRef;
    aboutref -> show();
    QFile aboutApp("Using Help.txt");
    aboutref -> setHelpText(aboutApp);
}

void MainWindow::on_actionAbout_triggered()
{
    AboutRef* aboutref = new AboutRef;
    aboutref -> show();
    QFile aboutApp("About.txt");
    aboutref -> setHelpText(aboutApp);
}

void MainWindow::on_actionServer_start_triggered()
{
    QtConcurrent::run([=]() { server -> start(12345); });
    ui -> logWindow->append("Start server");

    ui -> allUsersView -> clear();
    ui -> onlineUsersView -> clear();

    for (int i = 0; i < server -> getBaseUserList().size(); i++){
        QTreeWidgetItem* item = new QTreeWidgetItem(ui->allUsersView);
        item -> setText(0,server -> getBaseUserList()[i][0]);
        item -> setText(1,server -> getBaseUserList()[i][1]);
        item -> setText(2,server -> getBaseUserList()[i][2]);
        item -> setText(3,server -> getBaseUserList()[i][3]);
    }

    ui -> startButton -> setEnabled(0);
    ui -> stopButton -> setEnabled(1);
}

void MainWindow::on_actionServer_stop_triggered()
{
    server -> stop();
    ui -> logWindow->append("Stop server");

    ui -> allUsersView -> clear();
    ui -> onlineUsersView -> clear();

    ui -> startButton -> setEnabled(1);
}
