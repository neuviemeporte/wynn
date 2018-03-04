#ifndef DATABASE_MODEL_H_
#define DATABASE_MODEL_H_

#include <QAbstractTableModel>
#include "database.h"

/// Abstract class for other models to derive from.
class DatabaseModel : public QAbstractTableModel
{
	Q_OBJECT

protected:
	Database *data_;
	QStringList headers_;
	DbEntry::QuizDirection curType_;

public:
	DatabaseModel(QObject *parent = 0);
	~DatabaseModel() {}

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const { Q_UNUSED(parent); return (data_ ? 7 : 0); }

	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	void setDatabase(Database *data);
	void setQuizType(const DbEntry::QuizDirection type);

private slots:
	void beginInsert();
	void endInsert();
	void beginRemove(int index);
	void endRemove();

protected:
	const DbEntry& getItem(const QModelIndex &index) { return data_->entry(index.row()); }
};

#endif // DATABASE_MODEL_H_