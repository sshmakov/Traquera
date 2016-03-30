#ifndef TQVIEWCONTROLLER_H
#define TQVIEWCONTROLLER_H

#include <QObject>
#include <QtGui>
#include <tqplugin_global.h>
#include <tqbase.h>

QMetaMethod notifyMethod(QObject *object, const char *property);
QByteArray notifySignature(QObject *object, const char *property);
bool isMethodValid(const QMetaMethod &method);

class TQPLUGIN_SHARED TQViewController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QWidget *view READ view)
    Q_PROPERTY(TQRecord *currentRecord READ currentRecord NOTIFY currentRecordChanged)
    Q_PROPERTY(QObjectList selectedRecords READ selectedRecords NOTIFY selectedRecordsChanged)
private:
//    TQVCPrivate *d;
public:
    explicit TQViewController(QObject *parent = 0);
    ~TQViewController();
    virtual QWidget *view() const;
    virtual TQRecord *currentRecord() const;
    virtual QObjectList selectedRecords() const;
    virtual QAbstractItemView *tableView() const;
    virtual bool flag(const QString &flagName) const;
    virtual const TQAbstractRecordTypeDef* recordDef() const;

signals:
    /* from view to clients */
    void currentRecordChanged(TQRecord *record);
    void selectedRecordsChanged();
    /* from tab to view */
    void detailTabTitleChanged(QWidget *tab, const QString &title);

public slots:
    virtual void addDetailTab(QWidget *tab, const QString &title, const QIcon &icon = QIcon());
    virtual void addDetailWidgets(QWidget *topWidget, QWidget *pageWidget=0, const QString &title=QString(), const QIcon &icon = QIcon());
private slots:
    /* receive from view */
    virtual void onSelectedRecordsChanged();
    virtual void onDetailTitleChanged();
//    friend class QueryView;
};

Q_DECLARE_METATYPE(TQViewController *)


#endif // TQVIEWCONTROLLER_H
