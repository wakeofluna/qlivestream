#include "config.h"
#include "debug_network_messages.h"
#include "ui_debug_network_messages.h"

#include <QAbstractButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVariantList>
#include <QVariantMap>

namespace
{

void recursiveAdd(QTreeWidgetItem * pItem, QVariant const & pValue)
{
	if (pValue.type() == QVariant::Map)
	{
		QVariantMap lMap = pValue.toMap();
		QStringList lItems = lMap.keys();
		lItems.sort(Qt::CaseInsensitive);

		for (QString & s : lItems)
		{
			QTreeWidgetItem * lItem = new QTreeWidgetItem(pItem);
			lItem->setText(0, s);
			recursiveAdd(lItem, lMap.value(s));
		}
	}
	else if (pValue.type() == QVariant::List)
	{
		QVariantList lList = pValue.toList();

		for (int i = 0; i < lList.size(); ++i)
		{
			QTreeWidgetItem * lItem = new QTreeWidgetItem(pItem);
			lItem->setText(0, QString("[%1]").arg(i));
			recursiveAdd(lItem, lList.at(i));
		}
	}
	else
	{
		pItem->setText(1, pValue.toString());
	}
}

}


namespace forms
{

DebugNetworkMessages::DebugNetworkMessages(QWidget *parent) : QWidget(parent, Qt::Window)
{
	ui = new Ui::DebugNetworkMessages();
	ui->setupUi(this);
	connect(ui->btnClear, &QAbstractButton::clicked, this, &DebugNetworkMessages::clear);

#ifdef DEBUG
	ui->chkCapture->setChecked(true);
#endif
}

DebugNetworkMessages::~DebugNetworkMessages()
{
	delete ui;
}

bool DebugNetworkMessages::isCapturing() const
{
	return ui->chkCapture->isChecked();
}

void DebugNetworkMessages::setCapturing(bool pCapture)
{
	if (ui->chkCapture->isChecked() == pCapture)
		return;

	ui->chkCapture->setChecked(pCapture);
	emit capturingChanged();
}

void DebugNetworkMessages::addMessage(QString pTag, QVariant const & pMessage)
{
	QTreeWidgetItem * lRoot = new QTreeWidgetItem(ui->treeMessages);
	lRoot->setText(0, pTag);
	lRoot->setText(1, "OK");

	if (pMessage.type() == QVariant::Map)
		recursiveAdd(lRoot, pMessage);
	else
	{
		QTreeWidgetItem * lItem = new QTreeWidgetItem(lRoot);
		lItem->setText(0, "Reply");
		recursiveAdd(lItem, pMessage);
	}

	ui->btnClear->setEnabled(true);
}

void DebugNetworkMessages::addError(QString pTag, QString const & pMessage)
{
	QTreeWidgetItem * lRoot = new QTreeWidgetItem(ui->treeMessages);
	lRoot->setText(0, pTag);
	lRoot->setText(1, QString("Error: %1").arg(pMessage));
	ui->btnClear->setEnabled(true);
}

void DebugNetworkMessages::clear()
{
	ui->treeMessages->clear();
	ui->btnClear->setEnabled(false);
	emit cleared();
}

} // namespace forms
