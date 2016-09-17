/****************************************************************************
**
** Copyright (C) 2010 Richard Steffen and/or its subsidiary(-ies).
** All rights reserved.
** Contact: rsteffen@messbild.de, rsteffen@uni-bonn.de
**
** Observe the License Information
**
****************************************************************************/

#ifndef __MULTIBOXCOMBO_H__
#define __MULTIBOXCOMBO_H__

#include <iostream>
using namespace std;

#include <QComboBox>
#include <QListWidget>
#include <QVBoxLayout>
#include <QStylePainter>

class TQMultiComboBox: public QComboBox
{
    Q_OBJECT
public:

    /// Constructor
    TQMultiComboBox(QWidget *widget = 0);

    virtual ~TQMultiComboBox();

    /// the main display text
    void setDisplayText(QString text);

    /// get the main display text
    QString displayText() const;

    /// add a item to the list
    int addItem(const QString& text, bool checked);

    void clear();

    /// custom paint
    virtual void paintEvent(QPaintEvent *e);

    /// set the height of the popup
    void setPopupHeight(int h);

    /// replace standard QComboBox Popup
    void showPopup();
    void hidePopup();

    /// replace neccessary data access
    int count();
    void setCurrentIndex(int index);
    QString currentText();
    QString itemText(int row);
    Qt::CheckState itemCheckState(int row);
    void setItemChecked(int row, Qt::CheckState state);

signals:
    /// item changed
    void itemChanged();

public slots:

    /// react on changes of the item checkbox
    void scanItemSelect(QListWidgetItem* item);

    /// the init style
    void initStyleOption(QStyleOptionComboBox *option) const;

protected:

    /// the height of the popup
    int popheight_;

    /// lower/upper screen bound
    int screenbound_;

    /// hold the main display text
    QString m_DisplayText_;

    /// popup frame
    QFrame popframe_;

    /// multi selection list in the popup frame
    QListWidget vlist_;

};

#endif
