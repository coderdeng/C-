#include "dchat.h"
#include <qdebug.h>
#include <qstringlistmodel.h>

DChat::DChat(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	init();
}

DChat::~DChat()
{

}

bool DChat::init()
{
	///注册
	QAction *regAct = new QAction(QString::fromLocal8Bit("注册"), this);
	connect(regAct, SIGNAL(triggered()), this, SLOT(doRegister()));
	ui.menuBar->addAction(regAct);
	///登录
	QAction *logAct = new QAction(QString::fromLocal8Bit("登录"), this);
	connect(logAct, SIGNAL(triggered()), this, SLOT(doLogin()));
	ui.menuBar->addAction(logAct);
	///显示在线用户
	connect(ui.user_view, SIGNAL(itemClicked(QListWidgetItem *)),
		this, SLOT(choose(QListWidgetItem*)));
	///发送消息
	connect(ui.send_button, SIGNAL(clicked()), this, SLOT(sendMsg()));
	cm = clientManager();
	return true;
}

void DChat::showOnlineUser()
{
	std::vector<std::string> _old_user;
	while (true) {
		QStringList list;
		std::vector<std::string> userVec = cm.getOnlineUser();
		if (_old_user != userVec) {
			_old_user = userVec;
			qDebug("online number%d", userVec.size());
			for (std::vector<std::string>::iterator it = userVec.begin();
				it != userVec.end(); ++it) {
				list << QString::fromLocal8Bit((*it).c_str());
			}
			ui.user_view->clear();
			ui.user_view->addItems(list);
		}
		boost::this_thread::sleep(boost::posix_time::seconds(10));
	}
}

void DChat::showRecvMsg()
{
	while (true) {
		std::string msg(cm.checkGetMsg());
		std::string smsg("<p align = \"left\">" + msg + "</p>");
		if (msg == "")
			boost::this_thread::sleep(boost::posix_time::seconds(2));
		else
			ui.recvBrowser->append(QString::fromLocal8Bit(smsg.c_str()));
	}
}

void DChat::sendMsg()
{
	std::string msg(ui.textEdit->toPlainText().toStdString());
	ui.recvBrowser->append(tr(std::string("<p align = \"right\">" + msg + "</p>").c_str()));
	if(talk_to != "")
		cm.do_sendto(talk_to, msg);
}

void DChat::doRegister()
{
	regDia = new QDialog(ui.centralWidget);
	regDia->setModal(true);
	regDia->show();
}

void DChat::doLogin()
{
	logDia = new loginDialog(ui.centralWidget);
	logDia->setModal(true);
	logDia->show();
	if (logDia->exec() == QDialog::Accepted) {
		qDebug(logDia->getUser().c_str());
		qDebug(logDia->getPsw().c_str());
		bool logined = cm.do_login(logDia->getUser(), logDia->getPsw());
		if (logined) {
			boost::thread(boost::bind(&DChat::showOnlineUser, this));
			//showOnlineUser();
			boost::thread(boost::bind(&DChat::showRecvMsg, this));
		}
	}
}

void DChat::choose(QListWidgetItem *item)
{
	//ui.recvBrowser->append("choose");
	talk_to = item->text().toStdString();
	ui.talk_to_lbl->setText(QString::fromLocal8Bit(talk_to.c_str()));
}