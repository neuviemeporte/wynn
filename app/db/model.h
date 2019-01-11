#ifndef DATABASE_MODEL_H_
#define DATABASE_MODEL_H_

#include <QAbstractTableModel>
#include "database.h"

namespace wynn {
namespace db {

/// Abstract class for other models to derive from.
class Model : public QAbstractTableModel
{
	Q_OBJECT

protected:
    enum Column { INDEX, ITEM, DESC, LEVEL, FAILS, UPDATED, CREATED };
	Database *data_;
	QStringList headers_;
    QuizDirection curType_;

public:
    Model(QObject *parent = nullptr);
    ~Model() {}

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const {
        Q_UNUSED(parent);
        return (data_ ? 7 : 0);
    }

	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	void setDatabase(Database *data);
    void setQuizType(const QuizDirection type);

private slots:
	void beginInsert();
	void endInsert();
	void beginRemove(int index);
	void endRemove();

protected:
    const Entry& getItem(const QModelIndex &index) { return data_->entry(index.row()); }
};

} // namespace db
} // namespace wynn

#endif // DATABASE_MODEL_H_
