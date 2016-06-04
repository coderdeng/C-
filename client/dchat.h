#ifndef DCHAT_H
#define DCHAT_H

#include <QtWidgets/QMainWindow>
#include <qdialog.h>
#include "ui_dchat.h"
#include "clientManager.h"
#include "loginDialog.h"

class DChat : public QMainWindow
{
	Q_OBJECT

public:
	DChat(QWidget *parent = 0);
	~DChat();

private:
	Ui::DChatClass ui;
	QDialog *regDia;
	loginDialog *logDia;
	clientManager cm;
	std::string talk_to;

	bool init();
	void showOnlineUser();
	void showRecvMsg();

private slots:
	void sendMsg();
	void doRegister();
	void doLogin();
	void choose(QListWidgetItem *item);
};

#endif // DCHAT_H
