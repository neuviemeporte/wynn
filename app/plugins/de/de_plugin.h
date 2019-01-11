#ifndef DE_PLUGIN_H
#define DE_PLUGIN_H

#include "ui_panel.h"
#include "ui_details.h"

#include <dict_plugin.h>
#include <german.h>

class DePlugin : public QObject, public DictionaryPlugin
{
	Q_OBJECT

#if defined( DE_PLUGIN_LIB ) && QT_VERSION >= 0x050000
	Q_PLUGIN_METADATA(IID "wynn.DePlugin")
#endif

	Q_INTERFACES(DictionaryPlugin)

private:
    static const QString SETT_COLW_DE, SETT_COLW_EN, DICT_BEO;

	Ui::DePanelUI panelUI_;
	Ui::DeDetailsUI detailsUI_;
	class QDialog *detailsDialog_;
	BeoDict dict_;
	QList<BeoResult> results_;
	bool deQuery_, enQuery_;
	int colGermanWidth_, colEnglishWidth_;

public:
	DePlugin();
	virtual ~DePlugin();

	virtual QString name() const { return QString("Wynn German plugin v1.0"); }
	virtual QString language() const { return QString("German"); }
	virtual QString description() const;

	virtual int resultItemCount() const;
	virtual int resultColumnCount() const;
	virtual QString resultData(const int itemIndex, const int columnIndex) const;
	virtual QString resultHeaderData(const int column) const;

	virtual void showResultDetails(const int resultIndex);
	virtual QStringList getResultForDatabase(const int resultIndex);

	virtual void preSetup(QWidget *parent, class QTSLogger *debugLog);
	virtual void setup();
	virtual void activate();

signals:
	void searchStart();
	void searchDone();
	void message(const QString &msg);
	void columnWidthsNotify(const QList<int> &widths);

public slots:
	void columnResize(int curPlugIdx, int colIdx, int oldSize, int newSize);

private slots:
	void doSearch();

protected:
	void postMessage(const QString &msg);
	void saveSettings();
	void loadSettings();
};

#endif // DE_PLUGIN_H
