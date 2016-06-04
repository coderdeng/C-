#include "loginDialog.h"
#include <qgridlayout.h>
#include <qboxlayout.h>

loginDialog::loginDialog(QWidget *parent)
	: QDialog(parent),
	_user_lbl(new QLabel("username:")),
	_psw_lbl(new QLabel("password:")),
	_user_line(new QLineEdit()),
	_psw_line(new QLineEdit()),
	_ok_btn(new QPushButton("login")),
	_cancel_btn(new QPushButton("cancel"))
{
	QHBoxLayout *userLayout = new QHBoxLayout;
	QHBoxLayout *pswLayout = new QHBoxLayout;
	QHBoxLayout *btnLayout = new QHBoxLayout;
	userLayout->addWidget(_user_lbl, 0);
	userLayout->addWidget(_user_line, 1);
	pswLayout->addWidget(_psw_lbl, 0);
	pswLayout->addWidget(_psw_line, 1);
	btnLayout->addWidget(_ok_btn, 0);
	btnLayout->addWidget(_cancel_btn, 1);
	QVBoxLayout *vboxLayout = new QVBoxLayout;
	vboxLayout->addItem(userLayout);
	vboxLayout->addItem(pswLayout);
	vboxLayout->addItem(btnLayout);
	setLayout(vboxLayout);

	connect(_ok_btn, SIGNAL(clicked()), this, SLOT(accept()));
	connect(_cancel_btn, SIGNAL(clicked()), this, SLOT(reject()));
}

loginDialog::~loginDialog()
{

}

std::string loginDialog::getUser()
{
	return _user_line->text().toStdString();
}
std::string loginDialog::getPsw()
{
	return _psw_line->text().toStdString();
}
