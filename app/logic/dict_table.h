#pragma once

#include <QAbstractTableModel>

class DictionaryPlugin;

class DictionaryModel : public QAbstractTableModel
{
protected:
	const DictionaryPlugin *source_;

public:
	DictionaryModel(QObject *parent = 0);

	void setSource(const DictionaryPlugin *source);
	void beginReset();
	void endReset();

    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
};

