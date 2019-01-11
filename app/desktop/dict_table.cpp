#include "global.h"
#include "dict_table.h"
#include "dict_plugin.h"
#include "ixplog_active.h"

#include <QChar>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QList>
#include <QFont>
#include <QSize>

// ==============================================================================================
// ============================== DictionaryModel
// ==============================================================================================
DictionaryModel::DictionaryModel(QObject *parent) : QAbstractTableModel(parent),
	source_(NULL)
{
}

void DictionaryModel::setSource(const DictionaryPlugin *source)
{
	beginResetModel();
	source_ = source;
	endResetModel();
}

void DictionaryModel::beginReset()
{
	beginResetModel();
}

void DictionaryModel::endReset()
{
	endResetModel();
}

int DictionaryModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return (source_ ? source_->resultItemCount() : 0);
}

int DictionaryModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return (source_ ? source_->resultColumnCount() : 0);
}

QVariant DictionaryModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()) return QVariant();

	if (role == Qt::TextAlignmentRole) 
	{
		return int(Qt::AlignLeft | Qt::AlignVCenter);
	}
	else if (role == Qt::DisplayRole)
	{
		return (source_ ? source_->resultData(index.row(), index.column()) : QString());
	}
	else if (role == Qt::FontRole)
	{
		return (source_ ? source_->font() : QFont());
	}

	return QVariant();
}

QVariant DictionaryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole) 
	{
		if (orientation == Qt::Horizontal)
		{
			return (source_ ? source_->resultHeaderData(section) : QString());
		}
		else if (orientation == Qt::Vertical) 
		{
			return section+1;
		}
	}
	
	return QVariant();
}
