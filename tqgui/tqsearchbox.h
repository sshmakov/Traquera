#ifndef TQSEARCHBOX_H
#define TQSEARCHBOX_H

#include "tqgui_global.h"
#include <QLineEdit>
#include <QObject>
#include <QWidget>

class QAbstractItemModel;
class QModelIndex;

class TQSearchBoxPrivate;
class TQGUISHARED_EXPORT TQSearchBox : public QLineEdit
{
    Q_OBJECT
private:
    TQSearchBoxPrivate *d;
public:
    TQSearchBox(QWidget *parent = 0);
    ~TQSearchBox();
    QAbstractItemModel *model();
    void setModel(QAbstractItemModel *model);
protected:
    bool event(QEvent *ev);
    bool eventFilter(QObject *obj, QEvent *ev);
public:
    virtual void showPopup();
    virtual void hidePopup();
    virtual void setFilterText(const QString &text);
//    QModelIndex currentIndex() const;
protected:
    void focusList();
    void focusEdit();
    void focusOutEvent(QFocusEvent *foc);
    bool isPopup();
    virtual void popupItemSelected(const QModelIndex &index);
signals:
    void modelIndexSelected(const QModelIndex &index);
protected slots:
    virtual void onTextChanged(const QString &text);
    void activatedItem(const QModelIndex &index);

};

#endif // TQSEARCHBOX_H
