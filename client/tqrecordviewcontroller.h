#ifndef TQRECORDVIEWCONTROLLER_H
#define TQRECORDVIEWCONTROLLER_H


#include <QtCore>
#include <QtGui>
#include <tqbase.h>
#include "tqviewcontroller.h"

class TQRVCPrivate;
class TTRecordWindow;

class TQRecordViewController : public TQViewController
{
    Q_OBJECT
    Q_PROPERTY(QWidget *view READ view)
//    Q_PROPERTY(TQRecord *record READ record NOTIFY recordChanged)
private:
    TQRVCPrivate *d;
public:
    explicit TQRecordViewController(QObject *parent);
    ~TQRecordViewController();
    TTRecordWindow *recView() const;
    virtual const TQAbstractRecordTypeDef* recordDef() const;
    TQRecord *currentRecord() const;
    QObjectList selectedRecords() const;
private slots:
    void onViewRecordChanged(TQRecord *record);

    /*
    QWidget *view() const;
    TQRecord *currentRecord() const;
protected:
    void setView(QWidget *view);
    
signals:
    void recordChanged(TQRecord *record);
    void detailTabTitleChanged(QWidget *tab, const QString &title);

public slots:
    void addDetailTab(QWidget *tab, const QString &title, const QIcon &icon = QIcon());
    void addDetailWidgets(QWidget *topWidget, QWidget *pageWidget=0, const QString &title=QString(), const QIcon &icon = QIcon());
private slots:
    void onViewRecordChanged(TQRecord *record);
    void onDetailTitleChanged();
    */
    friend class TTRecordWindow;
};

Q_DECLARE_METATYPE(TQRecordViewController *)

#endif // TQRECORDVIEWCONTROLLER_H
