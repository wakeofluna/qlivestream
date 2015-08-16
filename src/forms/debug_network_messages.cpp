#include "config.h"
#include "debug_network_messages.h"
#include "ui_debug_network_messages.h"

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
	else if (pValue.type() == QVariant::StringList)
	{
		QStringList lList = pValue.toStringList();

		pItem->setText(1, QString("(%1 lines)").arg(lList.size()));

		for (int i = 0; i < lList.size(); ++i)
		{
			QTreeWidgetItem * lItem = new QTreeWidgetItem(pItem);
			lItem->setText(0, QString("Line %1").arg(i+1));
			lItem->setText(1, lList.at(i));
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
	connect(ui->btnClear, &QPushButton::clicked, this, &DebugNetworkMessages::clear);

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
	addEntry(pTag, "OK", pMessage);
}

void DebugNetworkMessages::addError(QString pTag, QString const & pError, QVariant const & pMessage)
{
	addEntry(pTag, QString("Error: %1").arg(pError), pMessage);
}

void DebugNetworkMessages::clear()
{
	ui->treeMessages->clear();
	ui->btnClear->setEnabled(false);
	emit cleared();
}

void DebugNetworkMessages::addEntry(QString pTag, QString const & pStatus, QVariant const & pMessage)
{
	QTreeWidgetItem * lRoot = new QTreeWidgetItem(ui->treeMessages);
	lRoot->setText(0, pTag);
	lRoot->setText(1, pStatus);

	if (pMessage.type() == QVariant::Map)
		recursiveAdd(lRoot, pMessage);
	else if (!pMessage.isNull())
	{
		QTreeWidgetItem * lItem = new QTreeWidgetItem(lRoot);
		lItem->setText(0, "Reply");
		recursiveAdd(lItem, pMessage);
	}

	ui->btnClear->setEnabled(true);
}

} // namespace forms
