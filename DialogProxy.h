#ifndef DIALOGPROXY_H
#define DIALOGPROXY_H

#include <QDialog>

namespace Ui {
class DialogProxy;
}

class DialogProxy : public QDialog
{
	Q_OBJECT

public:
	explicit DialogProxy(QWidget *parent = 0);
	~DialogProxy();

public slots:
	void CheckProxySettings(bool checked);

private slots:
	void on_buttonBox_accepted();

	void on_buttonBox_rejected();

	void on_checkBox_toggled(bool checked);

private:
	void EnableAll(bool enable);

private:
	Ui::DialogProxy *ui;
};

#endif // DIALOGPROXY_H
