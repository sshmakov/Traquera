#ifndef TQCONNECTWIDGET_H
#define TQCONNECTWIDGET_H

#include <QWidget>
#include <QAbstractItemModel>

#include <tqbase.h>

namespace Ui {
class TQConnectWidget;
}

class TQConnectModel;
class QSqlTableModel;
class QSqlRecord;


class TQConnectWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString connectString READ connectString WRITE setConnectString)
    Q_PROPERTY(QString connectSaveString READ connectSaveString WRITE setConnectString)
public:
    TQConnectModel *model;
    QSqlTableModel *sqlModel;
    int connectionId, tableRow;

    enum FieldPos {
        FieldId,
        FieldProject,
        FieldDbClass,
        FieldDbType,
        FieldDbServer,
        FieldUser,
        FieldPassword,
        FieldDbOsUser,
        FieldDbmsUser,
        FieldDbmsPass,
        FieldAutoLogin
    };

public:
    explicit TQConnectWidget(QWidget *parent = 0);
    ~TQConnectWidget();
//    ConnectParams currentParams() const;
//    void setParams(const ConnectParams &params);
    QString connectString() const;
    QString connectSaveString() const;
    void setConnectString(const QString &str);
    void loadSettings();

signals:
    void buttonClicked(int result);

private slots:
//    void slotCurrentRowChanged(QModelIndex current, QModelIndex previous);
    void on_btnOpen_clicked();
    void on_btnCancel_clicked();


    void on_btnDBMS_clicked();
    void setDBType(const QString &type);
    void setProject(const QString &project);

    void on_btnProjects_clicked();

    void on_trustedUserBox_toggled(bool checked);

    void on_btnSave_clicked();

protected:
//    static void recordToParams(const QSqlRecord &rec, ConnectParams &params);
//    static void paramsToRecord(const ConnectParams &params, QSqlRecord &rec);

private:
    Ui::TQConnectWidget *ui;
};

class TQConnectModel:public QAbstractItemModel
{
    Q_OBJECT
protected:
    QSqlTableModel *sqlModel;
public:
    TQConnectModel(QObject *parent=0);
    ~TQConnectModel();
    void open();

    /* override */
    virtual int	rowCount(const QModelIndex & parent = QModelIndex() ) const;
    virtual int	columnCount(const QModelIndex & parent = QModelIndex() ) const;
    virtual QModelIndex	index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole ) const;
    virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    virtual bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
    virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;

};

#endif // TQCONNECTWIDGET_H
