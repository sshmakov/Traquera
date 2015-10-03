#ifndef TQHISTORY_H
#define TQHISTORY_H

#include <tqmodels.h>

struct TQHistoryItem
{
    QString projectName;
    QString queryName;
    bool isQuery;
    int rectype;
    QString foundIds;
    QString removedIds;
    QString addedIds;
    QDateTime createDateTime;
};

class TQHistory: public BaseRecModel<TQHistoryItem>
{
    Q_OBJECT
    //Q_PROPERTY(bool unique READ unique WRITE setUnique)
protected:
    TQAbstractProject *prj;
    bool unique;
public:
    TQHistory(QObject *parent = 0);
    virtual ~TQHistory();
    void setProject(TQAbstractProject *project);
    void setUnique(bool value);
    //virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
    //virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole ) const;
    void removeLast();
protected:
    virtual QVariant displayColData(const TQHistoryItem & rec, int col) const;
public slots:
    void openedModel(const TQRecModel *model);
    void append(const TQHistoryItem &rec);
};



#endif // TQHISTORY_H
