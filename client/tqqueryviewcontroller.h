#ifndef TQQUERYVIEWCONTROLLER_H
#define TQQUERYVIEWCONTROLLER_H

#include <QObject>
#include <QtGui>
#include <tqbase.h>
#include "tqviewcontroller.h"

class TQQVCPrivate;

class TQQueryViewController : public TQViewController
{
    Q_OBJECT
//    Q_PROPERTY(QWidget *view READ view)
//    Q_PROPERTY(TQRecord *currentRecord READ currentRecord NOTIFY currentRecordChanged)
//    Q_PROPERTY(QObjectList selectionRecords READ selectionRecords NOTIFY selectedRecordsChanged)
private:
    TQQVCPrivate *d;
public:
    explicit TQQueryViewController(QObject *parent);
    ~TQQueryViewController();
private:
    void emitCurrentRecordChanged(TQRecord *record);
//    QWidget *view() const;
//    TQRecord *currentRecord() const;
//    QObjectList selectionRecords() const;

signals:
    /* from view to clients */
//    void currentRecordChanged(TQRecord *record);
//    void selectedRecordsChanged();
    /* from tab to view */
//    void detailTabTitleChanged(QWidget *tab, const QString &title);

public slots:
//    void addDetailTab(QWidget *tab, const QString &title, const QIcon &icon = QIcon());
//    void addDetailWidgets(QWidget *topWidget, QWidget *pageWidget=0, const QString &title=QString(), const QIcon &icon = QIcon());
private slots:
    /* receive from view */
//    void onSelectionRecordsChanged();
//    void onDetailTitleChanged();
    friend class QueryPage;


signals:
    
public slots:
    
};

Q_DECLARE_METATYPE(TQQueryViewController *)

#endif // TQQUERYVIEWCONTROLLER_H
