#ifndef TQFIELDDELEGATE_H
#define TQFIELDDELEGATE_H

#include <QObject>
#include <QWidget>
#include "tqplug.h"

class QStyleOptionViewItem;

class TQFieldDelegate : public QObject
{
    Q_OBJECT
public:
    explicit TQFieldDelegate(QObject *parent = 0);
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem & option, const TQAbstractRecordTypeDef *recordDef, int vid) const;
    virtual void	paint(QPainter * painter, const QStyleOptionViewItem & option, const TQRecord *record, int vid) const;
    virtual void	setEditorData(QWidget * editor, const TQRecord *record, int vid) const;
    virtual void	setModelData(QWidget * editor, TQRecord *record, int vid) const;
    virtual QSize	sizeHint(const QStyleOptionViewItem & option, const TQRecord *record, int vid) const;
    virtual void	updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const TQRecord *record, int vid) const;
signals:

public slots:
};

#endif // TQFIELDDELEGATE_H
