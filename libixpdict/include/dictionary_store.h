#ifndef IXP_DICT_STORE_H
#define IXP_DICT_STORE_H

#include <QList>
#include <QFile>
#include <QDataStream>

/// An abstract dictionary class. 
template <typename T> class Dictionary
{
protected:
	QList<T> entries_;
	bool error_;

public:
	Dictionary() : error_(false) {}
 
	int entryCount() const { return entries_.size(); }
	bool error() const { return error_; }

	const T& entry(int index) const { return entries_.at(index); }
	T& entry(int index) { return entries_[index]; }

	void loadData(const QString &path);
	bool saveData(const QString &path);

protected:
	void addEntry(const T &entry) { entries_.append(entry); }
};

template <typename T> void Dictionary<T>::loadData(const QString &path)
{
	QFile f(path);
	if (!f.open(QIODevice::ReadOnly)) 
	{
		//QLOGX("Unable to open file for reading: '" << path << "'");
		error_ = true;
		return;
	}
	QDataStream in(&f);
	in >> entries_;
	if (in.status() != QDataStream::Ok) 
	{
		//QLOGX("Error reading dictionary file, status: " << in.status());
		error_ = true;
		return;
	}
	//QLOGX("Read " << entries_.size() << " items");
}

template <typename T> bool Dictionary<T>::saveData(const QString &path)
{
	QFile f(path);
	if (!f.open(QIODevice::WriteOnly)) 
	{ 
		//QLOGX("Unable to open file for writing: '" << path << "'");
		return false; 
	}
	QDataStream out(&f);
	out << entries_;
	if (out.status() != QDataStream::Ok)
	{
		//QLOGX("Error saving dictionary file, status: " << out.status());
		return false;
	}
	return true;
}


#endif //IXP_DICT_STORE_H