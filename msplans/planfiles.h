#ifndef PLANFILES_H
#define PLANFILES_H

#include <QAbstractItemModel>

struct Plan
{
    QString file;
    bool loaded;
    bool readOnly;
    QString name;
};

class PrjItemModel;
class MainWindow;
class PlansPlugin;

class PlanFilesModel: public QAbstractItemModel
{
    Q_OBJECT
protected:
    //MainWindow *mainWindow;
    PlansPlugin *plugin;
public:
    QList<Plan> plans;
    QMap<QString, PrjItemModel *> models;

    //PlanFilesModel(MainWindow *parent);
    PlanFilesModel(PlansPlugin *main);
    virtual int	columnCount(const QModelIndex & parent = QModelIndex() ) const;
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole ) const;
    virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    virtual int	rowCount(const QModelIndex & parent = QModelIndex() ) const;
    virtual QModelIndex	index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    int addPlan(const QString & plan, bool readOnly);
    virtual bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
    PrjItemModel *loadPlan(int row);
    bool unloadPlan(int row);
    bool isPlanLoaded(int row);
    void triggerLoadPlan( int row);
    QString planFileName(int row) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
    virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
};

#endif // PLANFILES_H
