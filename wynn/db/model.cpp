#include <QFont>
#include "model.h"
#include "ixplog_active.h"

extern class QTSLogger APP_LOG;
extern class QTSLogger *APP_LOGSTREAM;
#define QLOGSTREAM APP_LOGSTREAM
#define QLOGTAG "[WYNN_DB]"

namespace wynn {
namespace db {

Model::Model(QObject *parent) : QAbstractTableModel(parent),
    data_(nullptr),
    headers_{ tr("Index"), tr("Item"), tr("Description"), tr("Level"), tr("Failures"), tr("Updated"), tr("Created") },
    curType_(DIR_UNDEF)
{
}

int Model::rowCount(const QModelIndex &) const {
    if (data_) return data_->entryCount();
    else return 0;
}

QVariant Model::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    switch(orientation) {
    case Qt::Horizontal: return headers_.at(section);
    case Qt::Vertical:   return section + 1;
    default: return QVariant();
    }

    return QVariant();
}

QVariant Model::data(const QModelIndex &index, int role) const {
    if ( !index.isValid() || !data_ )
        return QVariant();

    if ( role == Qt::TextAlignmentRole ) {
        return int(Qt::AlignLeft | Qt::AlignVCenter);
    }
    else if ( role == Qt::DisplayRole ) {
        const int col = index.column(), row = index.row();
        const Entry &entry = data_->entry(row);
        QString value;
        switch(col) {
        case INDEX: value = QString::number(row + 1); break;
        case ITEM:  value = entry.item(); break;
        case DESC:  value = entry.description(); break;
        // TODO: currently no way to change curType in model
        case LEVEL: value = QString::number(entry.level(curType_)); break;
        case FAILS: value = QString::number(entry.fails(curType_)); break;
        case UPDATED:
            value = (entry.updateStamp() >= 0 ? data_->dateCreated().addSecs(entry.updateStamp()).toString("d.MM.yyyy") : tr("never"));
            break;
        case CREATED:
            value = data_->dateCreated().addSecs(entry.createStamp()).toString("d.MM.yyyy");
            break;
        default: break;
        }
        return value;
    }
    // for proxy model sorting, otherwise would sort based on strings which is not desirable
    else if (role == Qt::UserRole) {
        const int col = index.column(), row = index.row();
        const Entry &entry = data_->entry(row);
        QVariant value;
        switch(col) {
        case INDEX:   value = row; break;
        case ITEM:    value = entry.item(); break;
        case DESC:    value = entry.description(); break;
        case LEVEL:   value = entry.points(curType_); break;
        case FAILS:   value = entry.fails(curType_); break;
        case UPDATED: value = data_->dateCreated().addSecs(entry.updateStamp()); break;
        case CREATED: value = data_->dateCreated().addSecs(entry.createStamp()); break;
        default: break;
        }
        return value;
    }
    else if (role == Qt::FontRole) {
        // TODO: figure out a way to return a proper font, perhaps add info on source plugin to every entry?
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

void Model::setDatabase(Database *data) {
    const QString name = data ? data->name() : "NULL";
    QLOGX("Pointing model to new database: '" << name << "'");
    beginResetModel();
    if (data_) {
        disconnect(data_, 0, this, 0);
    }
    data_ = data;
    if (data_) {
        connect(data_, SIGNAL(beginInsert()),    this, SLOT(beginInsert()));
        connect(data_, SIGNAL(endInsert()),      this, SLOT(endInsert()));
        connect(data_, SIGNAL(beginRemove(int)), this, SLOT(beginRemove(int)));
        connect(data_, SIGNAL(endRemove()),      this, SLOT(endRemove()));
    }
    endResetModel();
}

void Model::setQuizType(const QuizDirection type) {
    QLOGX("Setting quiz type : " << type);
    // TODO: in the future emitting dataChanged() to indicate that the values in two columns have changed may be more efficient
    // than resetting the whole model.
    beginResetModel();
    curType_ = type;
    endResetModel();
}

void Model::beginInsert() {
    QLOGX("inserting 1 row");
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
}

void Model::endInsert() {
    QLOGX("finishing");
    endInsertRows();
}

void Model::beginRemove(int index) {
    QLOGX("removing 1 row");
    beginRemoveRows(QModelIndex(), index, index);
}

void Model::endRemove() {
    QLOGX("finishing");
    endRemoveRows();
}

} // namespace db
} // namespace wynn
