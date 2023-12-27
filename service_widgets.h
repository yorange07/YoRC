#ifndef SERVICE_WIDGETS_H
#define SERVICE_WIDGETS_H

#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QLabel>
#include <QFile>
#include <QPixmap>
#include <QIcon>
#include <QPushButton>
#include <QFileDialog>
#include <QLineEdit>
#include  "server.h"


//___________________________________________________________________
class AboutRef: public QDialog
{
Q_OBJECT
public:
    AboutRef(QWidget* parent = 0): QDialog(parent){


        this->resize(500,500);

        about_text_edit = new QTextEdit;
        setWindowTitle("Info");

        about_text_edit -> setReadOnly(true);

        QVBoxLayout *mainLayout = new QVBoxLayout();
        mainLayout -> addWidget(about_text_edit);

        setLayout(mainLayout);
    };

    void setHelpText(QFile& file)
    {

        if ((file.exists())&&(file.open(QIODevice::ReadOnly)))
        about_text_edit -> setText(file.readAll());
        file.close();
    };

    void setHelpStr(QString& str)
    {
        about_text_edit -> setText(str);
    };

private:
    QTextEdit *about_text_edit;
};


//___________________________________________________________________


class SendServiceMessage: public QDialog
{
Q_OBJECT
public:

    std::string SIGNAL_REQUEST = "6";

    SendServiceMessage(QWidget* parent = 0, QString addressSocket = 0, Server* s = 0): QDialog(parent), _addressSocket(addressSocket), _s(s){


        this->resize(50,100);



        label = new QLabel(tr("Message for Client"));

        edit = new QLineEdit();

        send = new QPushButton(tr("Send"));

        connect(send, SIGNAL(clicked()), this, SLOT(sendClicked()));


        QGridLayout *fLayout = new QGridLayout();
        fLayout -> addWidget(label);
        QVBoxLayout *sLayout = new QVBoxLayout();
        sLayout -> addWidget(edit);
        sLayout -> addWidget(send);



        QVBoxLayout *mainLayout = new QVBoxLayout();
        mainLayout -> addLayout(fLayout);
        mainLayout -> addLayout(sLayout);

        setLayout(mainLayout);

    };


private slots:
    void sendClicked()
    {
        QString qstr = edit->text();
        std::string str = qstr.toStdString();
        int addressSocket = _addressSocket.toInt();
        Message serviceMessage(SIGNAL_REQUEST, { str });

        _s -> sendMessageToClient(addressSocket, serviceMessage);
        this -> close();

    };

private:


    QString _addressSocket;
    Server* _s;
    QLabel *label;
    QLineEdit *edit;
    QPushButton *send;


};

#endif // SERVICE_WIDGETS_H
