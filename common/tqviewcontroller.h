#ifndef TQVIEWCONTROLLER_H
#define TQVIEWCONTROLLER_H

#include <QObject>
//#include <QtGui>
#include <tqplugin_global.h>
#include <tqbase.h>

QMetaMethod notifyMethod(QObject *object, const char *property);
QByteArray notifySignature(QObject *object, const char *property);
bool isMethodValid(const QMetaMethod &method);

class QWidget;
class QAbstractItemView;

class TQPLUGIN_SHARED TQViewController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *view READ viewObj)
    Q_PROPERTY(QObject *currentRecord READ currentRecord NOTIFY currentRecordChanged)
    Q_PROPERTY(QObjectList selectedRecords READ selectedRecords NOTIFY selectedRecordsChanged)
    Q_PROPERTY(QObject *project READ project)
    Q_PROPERTY(QList<int> selectedIds READ selectedIds NOTIFY selectedRecordsChanged)
private:
//    TQVCPrivate *d;
public:
    explicit TQViewController(QObject *parent = 0);
    ~TQViewController();
    virtual QWidget *view() const;
    virtual TQRecord *currentRecord() const;
    virtual QObjectList selectedRecords() const;
    virtual QList<int> selectedIds() const;
    virtual QAbstractItemView *tableView() const;
    Q_INVOKABLE virtual bool flag(const QString &flagName) const;
    virtual const TQAbstractRecordTypeDef* recordDef() const;
    TQAbstractProject *project() const;
    virtual bool canInstantEdit() const;
    Q_INVOKABLE virtual bool beginModifySection();
    Q_INVOKABLE virtual void submitModifySection();
    Q_INVOKABLE virtual void cancelModifySection();
private:
    QObject *viewObj() const;

signals:
    /* from view to clients */
    void currentRecordChanged(TQRecord *record);
    void selectedRecordsChanged();
    /* from tab to view */
    void detailTabTitleChanged(QWidget *tab, const QString &title);
    void recordModeChanged(int newMode);

public slots:
    virtual void addDetailTab(QWidget *tab, const QString &title, const QIcon &icon);
    virtual void addDetailWidgets(QWidget *topWidget, QWidget *pageWidget, const QString &title, const QIcon &icon);
private slots:
    /* receive from view */
    virtual void onSelectedRecordsChanged();
    virtual void onDetailTitleChanged();
//    friend class QueryView;
};

Q_DECLARE_METATYPE(TQViewController *)


#endif // TQVIEWCONTROLLER_H
