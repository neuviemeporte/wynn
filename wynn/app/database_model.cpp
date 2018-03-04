#include "database_model.h"
#include "global.h"
#include "ixplog_active.h"

#include <QFont>


// ==============================================================================================
// ============================== DatabaseModel
// ==============================================================================================

DatabaseModel::DatabaseModel(QObject *parent) : QAbstractTableModel(parent), 
	data_(NULL), curType_(DbEntry::DIR_UNDEF)
{
	headers_ << tr("Index") << tr("Item") << tr("Description") << tr("Level") << tr("Failures") << tr("Updated") << tr("Created");
}

int DatabaseModel::rowCount(const QModelIndex &/*parent*/) const
{
	if (data_) return data_->entryCount();
	else return 0;
}

QVariant DatabaseModel::headerData(int section, Qt::Orientation orientation, 
								   int role) const
{
	if (role != Qt::DisplayRole) return QVariant();

    switch(orientation)
    {
        case Qt::Horizontal: return headers_.at(section);
        case Qt::Vertical:   return section + 1;
        default: return QVariant();
    }

    return QVariant();
}

QVariant DatabaseModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || data_ == NULL) return QVariant();

	if (role == Qt::TextAlignmentRole) return int(Qt::AlignLeft | Qt::AlignVCenter);
	else if (role == Qt::DisplayRole)
	{
		const int 
			col = index.column(),
			row = index.row();
		const DbEntry &entry = data_->entry(row);
		QString value;
		switch(col)
		{
        case 0:
            value = QString::number(row + 1);
            break;
        case 1:
            value = entry.item();
            break;
        case 2:
            value = entry.description();
            break;
        case 3:
            // todo: na razie nie ma sposobu na zmiane curType w modelu
            value = QString::number(entry.level(curType_));
            break;
        case 4:
            value = QString::number(entry.fails(curType_));
            break;
        case 5:
            value = (entry.updateStamp() >= 0 ? data_->dateCreated().addSecs(entry.updateStamp()).toString("d.MM.yyyy") : tr("never"));
            break;
        case 6:
            value = data_->dateCreated().addSecs(entry.createStamp()).toString("d.MM.yyyy");
            break;
        default:
            value = "";
            break;
		}
		return value;
	}
	// for proxy model sorting, otherwise would sort based on strings which is not desirable
	else if (role == Qt::UserRole)
	{
		const int 
			col = index.column(),
			row = index.row();
		const DbEntry &entry = data_->entry(row);
		QVariant value;
		switch(col)
		{
        case 0:
            value = row;
            break;
        case 1:
            value = entry.item();
            break;
        case 2:
            value = entry.description();
            break;
        case 3:
            value = entry.points(curType_);
            break;
        case 4:
            value = entry.fails(curType_);
            break;
        case 5:
            value = data_->dateCreated().addSecs(entry.updateStamp());
            break;
        case 6:
            value = data_->dateCreated().addSecs(entry.createStamp());
            break;
        default:
            value = QVariant();
            break;
		}
		return value;
	}
	else if (role == Qt::FontRole)
	{
		// todo: figure out a way to return a proper font, perhaps add info on source plugin to every entry?
		//if (index.column() == 1 || index.column() == 2) 
		//{
		//	const DictionaryPlugin *plugin = data_->plugin();
		//	return (plugin ? plugin->font() : QFont());
		//}
		//else 
			return QFont();
	}

	return QVariant();
}

void DatabaseModel::setDatabase(Database *data)
{
	QString name = data ? data->name() : "NULL";
	QLOGX("Pointing model to new database: '" << name << "'");
	QLOGINC;
	beginResetModel();
	if (data_)
	{
		disconnect(data_, 0, this, 0);
	}
	data_ = data;
	if (data_)
	{
		connect(data_, SIGNAL(beginInsert()),    this, SLOT(beginInsert()));
		connect(data_, SIGNAL(endInsert()),      this, SLOT(endInsert()));
        connect(data_, SIGNAL(beginRemove(int)), this, SLOT(beginRemove(int)));
		connect(data_, SIGNAL(endRemove()),      this, SLOT(endRemove()));
	}
	endResetModel();
	QLOGDEC;
}

void DatabaseModel::setQuizType(const DbEntry::QuizDirection type) 
{ 
	QLOGX("Setting quiz type : " << type);
	// todo: in the future emitting dataChanged() to indicate that the values in two columns have changed may be more efficient
	// than resetting the whole model.
	beginResetModel();
	curType_ = type; 
	endResetModel();
}

void DatabaseModel::beginInsert() 
{ 
	QLOGX("inserting 1 row");
	beginInsertRows(QModelIndex(), rowCount(), rowCount()); 
}

void DatabaseModel::endInsert() 
{ 
	QLOGX("finishing");
	endInsertRows(); 
}

void DatabaseModel::beginRemove(int index) 
{ 
	QLOGX("removing 1 row");
	beginRemoveRows(QModelIndex(), index, index); 
}

void DatabaseModel::endRemove() 
{ 
	QLOGX("finishing");
	endRemoveRows(); 
}

