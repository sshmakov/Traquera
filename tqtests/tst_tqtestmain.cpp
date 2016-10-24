#include <QSortFilterProxyModel>
#include <QStandardItem>
#include <QString>
#include <QtTest>

#include "../client/unionmodel.h"

class TQTestMain : public QObject
{
    Q_OBJECT

public:
    TQTestMain();
    QList<QStandardItem *> makeTestItems(const QString &items);
    QString rowsToString(const QAbstractItemModel *model, const QModelIndex &parent = QModelIndex());

private Q_SLOTS:
    void testUnion();
};

TQTestMain::TQTestMain()
{
}

QList<QStandardItem *> TQTestMain::makeTestItems(const QString &items)
{
    QStringList lines = items.split(',');
    QList<QStandardItem *> res;
    foreach(QString s, lines)
    {
        QStandardItem *item = new QStandardItem(s);
        res.append(item);
    }
    return res;
}

QString TQTestMain::rowsToString(const QAbstractItemModel *model, const QModelIndex &parent)
{
    QStringList lines;
    for(int r=0; r<model->rowCount(parent); r++)
    {
        QModelIndex index = model->index(r,0,parent);
        QString s = index.data().toString();
        if(model->hasChildren(index))
        {
            s += ":{" + rowsToString(model, index) + "}";
        }
        lines.append(s);
    }
    return lines.join(",");
}

void TQTestMain::testUnion()
{
    QStandardItemModel src;
    src.appendColumn(makeTestItems("0,1,2,3,4,5,6"));
    UnionModel un;
    un.appendSourceModel(&src, "test");
    QModelIndex parent = un.index(0,0);
    QCOMPARE(parent.data().toString(), QString("test"));
    QCOMPARE(un.rowCount(parent), 7);
    un.removeRow(2,parent);
    QCOMPARE(rowsToString(&un), QString("test:{0,1,3,4,5,6}"));
    QStandardItemModel src2;
    src2.appendColumn(makeTestItems("a0,b1,a2,a3,b4"));
    QSortFilterProxyModel proxy;
    proxy.setSourceModel(&src2);
    proxy.setFilterRegExp("a");
    QCOMPARE(rowsToString(&proxy), QString("a0,a2,a3"));
    un.appendSourceModel(&proxy, "proxy");
    QCOMPARE(rowsToString(&un), QString("test:{0,1,3,4,5,6},proxy:{a0,a2,a3}"));
    src.removeRow(0);
    QCOMPARE(rowsToString(&un), QString("test:{1,3,4,5,6},proxy:{a0,a2,a3}"));
    parent = un.index(1,0);
    un.removeRow(1, parent);
    QCOMPARE(rowsToString(&un), QString("test:{1,3,4,5,6},proxy:{a0,a3}"));

}

QTEST_MAIN(TQTestMain)

#include "tst_tqtestmain.moc"
