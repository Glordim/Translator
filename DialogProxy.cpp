#include "DialogProxy.h"
#include "ui_DialogProxy.h"

#include <QNetworkProxy>
#include <QNetworkProxyFactory>
#include <QNetworkAccessManager>

DialogProxy::DialogProxy(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DialogProxy)
{
	ui->setupUi(this);

	this->ui->radioButton->setChecked(true);

	//this->on_checkBox_toggled(this->ui->checkBox->isChecked());



	//this->ui->checkBox->setChecked(false);
}

DialogProxy::~DialogProxy()
{
	delete ui;
}

void DialogProxy::on_buttonBox_accepted()
{
	QNetworkProxy proxy;

	QNetworkProxyFactory::setUseSystemConfiguration(false);

	if (this->ui->radioButton->isChecked() == true)
		proxy.setType(QNetworkProxy::NoProxy);
	else if (this->ui->radioButton_2->isChecked() == true)
	{
		proxy.setType(QNetworkProxy::DefaultProxy);
		//QNetworkProxyFactory::setUseSystemConfiguration(true);
		return;
	}
	else if (this->ui->radioButton_3->isChecked() == true)
	{
		proxy.setType(QNetworkProxy::HttpProxy);

		proxy.setHostName(this->ui->lineEdit->text());
		proxy.setPort(this->ui->lineEdit_4->text().toInt());

		if (this->ui->checkBox->isChecked())
		{
			proxy.setUser(this->ui->lineEdit_2->text());
			proxy.setPassword(this->ui->lineEdit_3->text());
		}
	}

	QNetworkProxy::setApplicationProxy(proxy);
	/*
    QNetworkProxyFactory proxyFactory;

    proxyFactory.

    QNetworkAccessManager::setProxyFactory(&proxyFactory);
    */
}

void DialogProxy::on_buttonBox_rejected()
{

}

void DialogProxy::on_checkBox_toggled(bool checked)
{
	this->ui->lineEdit_2->setEnabled(checked);
	this->ui->lineEdit_3->setEnabled(checked);
}

void DialogProxy::CheckProxySettings(bool checked)
{
	(void)checked;

	if (this->ui->radioButton->isChecked() || this->ui->radioButton_2->isChecked()) //No proxy or AutoDetect
	{
		this->EnableAll(false);
	}
	else
	{
		this->EnableAll(true);
		this->on_checkBox_toggled(this->ui->checkBox->isChecked());
	}
}

void DialogProxy::EnableAll(bool enable)
{
	this->ui->lineEdit->setEnabled(enable);
	this->ui->lineEdit_2->setEnabled(enable);
	this->ui->lineEdit_3->setEnabled(enable);
	this->ui->lineEdit_4->setEnabled(enable);

	this->ui->checkBox->setEnabled(enable);
}
