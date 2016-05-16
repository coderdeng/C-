#pragma once
#include <qdialog.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <string>

class loginDialog : public QDialog
{
	Q_OBJECT;

public:
	loginDialog(QWidget *parent);
	~loginDialog();

	std::string getUser();
	std::string getPsw();
	
private:
	QLabel *_user_lbl;
	QLabel *_psw_lbl;
	QLineEdit *_user_line;
	QLineEdit *_psw_line;

	QPushButton *_ok_btn;
	QPushButton *_cancel_btn;
};