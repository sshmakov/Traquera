#include "tqsearchbox.h"
#include <QtGui>
#include <QDebug>

//#define TQPOPUP

class TQSearchBoxPrivate
{
public:
    QWidget *frame;
    QListView *list;
    QStandardItemModel *internalModel;
    QAbstractItemModel *model;
    QSortFilterProxyModel *proxy;
    QWidget *popup;
//    QModelIndex currentIndex;
    QRect popupGeometry(int screen) const
    {
    #ifdef Q_WS_WIN
        return QApplication::desktop()->screenGeometry(screen);
    #elif defined Q_WS_X11
        if (X11->desktopEnvironment == DE_KDE)
            return QApplication::desktop()->screenGeometry(screen);
        else
            return QApplication::desktop()->availableGeometry(screen);
    #else
            return QApplication::desktop()->availableGeometry(screen);
    #endif
    }
};

TQSearchBox::TQSearchBox(QWidget *parent)
    : QLineEdit(parent), d(new TQSearchBoxPrivate())
{
    d->list = new QListView();
    d->list->setObjectName("listView");
    d->list->installEventFilter(this);
    d->list->setSelectionBehavior(QAbstractItemView::SelectRows);
    d->list->setSelectionMode(QAbstractItemView::SingleSelection);


#ifndef TQPOPUP
    d->frame = new QFrame(this,
                          Qt::Window
                          | Qt::ToolTip
//                          | Qt::FramelessWindowHint
//                          | Qt::Popup
                          );
    d->frame->installEventFilter(this);
    d->frame->setObjectName("frame");
    QHBoxLayout *lay = new QHBoxLayout(d->frame);
    lay->setMargin(0);


//    d->frame->setFocusPolicy(Qt::NoFocus);
//    d->list->setFocusProxy(d->frame);
    lay->addWidget(d->list);
    d->popup = d->frame;
#else
    d->list->setWindowFlags(d->list->windowFlags() | Qt::Popup );
    d->popup = d->list;
#endif


    d->internalModel = new QStandardItemModel(this);
    d->proxy = new QSortFilterProxyModel(this);
    d->proxy->setSourceModel(d->internalModel);
    d->proxy->setFilterKeyColumn(0);
    d->list->setModel(d->proxy);
    setObjectName("searchBox");

    QStringList lines = QString("asgfhgf;sdfkjhdf;asdfgsfgsgtr;QAECXDCD;GFDS;jhgjhgnv;hhh;ggg;hhhhgf").split(';');
    foreach(QString s, lines)
    {
        d->internalModel->appendRow(new QStandardItem(s));
    }
    connect(this, SIGNAL(textChanged(QString)), SLOT(onTextChanged(QString)));
    this->installEventFilter(this);
    connect(d->list, SIGNAL(activated(QModelIndex)), SLOT(activatedItem(QModelIndex)));
}

TQSearchBox::~TQSearchBox()
{
    delete d->list;
//    delete d->frame;
}

QAbstractItemModel *TQSearchBox::model()
{
    return d->proxy;
}

void TQSearchBox::setModel(QAbstractItemModel *model)
{
    d->model = model;
    if(d->model)
        d->proxy->setSourceModel(d->model);
    else
        d->proxy->setSourceModel(d->internalModel);
}

static inline void debugEvent(QObject *obj, QEvent *ev, const QString &place)
{
#ifndef TQDEBUGEVENT
    static bool first = true;
    static QMetaEnum en;
    static QMetaObject meta;
    if(first)
    {
        first = false;
        meta = ev->staticMetaObject;
        en = meta.enumerator(meta.indexOfEnumerator("Type"));
    }
    switch(ev->type())
    {
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
    case QEvent::Enter:
    case QEvent::Leave:
    case QEvent::HoverEnter:
    case QEvent::HoverLeave:
    case QEvent::HoverMove:
    case QEvent::DynamicPropertyChange:
    case QEvent::Paint:
        return;
    case QEvent::FocusIn:
    case QEvent::FocusOut:
    {
        QFocusEvent *foc = static_cast<QFocusEvent *>(ev);
        qDebug() << place << obj->objectName() << foc->type() << en.valueToKey(ev->type()) << foc->reason();
        break;
    }
    case QEvent::ShortcutOverride:
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    {
        QKeyEvent *key = static_cast<QKeyEvent *>(ev);
        qDebug() << place << obj->objectName() << ev->type() << en.valueToKey(ev->type()) << QKeySequence(key->key()).toString();
        break;
    }
    default:
        qDebug() << place << obj->objectName() << ev->type() << en.valueToKey(ev->type());
    }
#endif
}

bool TQSearchBox::event(QEvent *ev)
{
    debugEvent(this, ev, "event");
    if(ev->type() == QEvent::ShortcutOverride)
    {
        QKeyEvent *key = static_cast<QKeyEvent *>(ev);
        if(key->key() == Qt::Key_Down && key->modifiers() == Qt::NoModifier)
        {
//            focusList();
//            d->frame->activateWindow();
            key->accept();
            return false;
        }
    }
    return QLineEdit::event(ev);
}

bool TQSearchBox::eventFilter(QObject *obj, QEvent *ev)
{
    debugEvent(obj, ev, "eventFilter");
    switch(ev->type())
    {
    case QEvent::FocusIn:
    case QEvent::FocusOut:
    {
        QFocusEvent *foc = static_cast<QFocusEvent *>(ev);
        if(obj == this && foc->lostFocus() && foc->reason() == Qt::PopupFocusReason)
        {
//            ev->accept();
//            return true;
        }
        return false;
    }
    case QEvent::ShortcutOverride:
    case QEvent::KeyPress:
    {
        QKeyEvent *key = static_cast<QKeyEvent *>(ev);
        if(key->modifiers() != Qt::NoModifier)
            return false;
        if(obj == d->list)
        {
            switch(key->key())
            {
            case Qt::Key_Up:
                qDebug() << d->list->currentIndex().row();
                if(d->list->currentIndex().row() <= 0)
                {
//                    ev->accept();
//                    hidePopup();
                    focusEdit();
                    return true;
                }
                break;
            case Qt::Key_Escape:
                hidePopup();
                ev->accept();
                return true;
            }
        }
        else if(obj == this)
        {
            switch(key->key())
            {
            case Qt::Key_Return:
            case Qt::Key_Enter:
                onTextChanged(text());
                ev->accept();
                return false;
            case Qt::Key_Escape:
                hidePopup();
                return false;
            case Qt::Key_Down:
                focusList();
                //            d->frame->activateWindow();
//                key->accept();
                return true;
            }
        }
    }
    }
    return false;
}

void TQSearchBox::showPopup()
{
    QRect listRect = QRect(QPoint(0,0),size());
    QRect screen = d->popupGeometry(QApplication::desktop()->screenNumber(this));
    QPoint below = mapToGlobal(listRect.bottomLeft());
    int belowHeight = screen.bottom() - below.y();
    QPoint above = mapToGlobal(listRect.topLeft());
    int aboveHeight = above.y() - screen.y();
    bool boundToScreen = !window()->testAttribute(Qt::WA_DontShowOnScreen);

    d->popup->move(below);
    d->popup->show();

    /*
    const bool usePopup = style->styleHint(QStyle::SH_ComboBox_Popup, &opt, this);
    {
        int listHeight = 0;
        int count = 0;
        QStack<QModelIndex> toCheck;
        toCheck.push(view()->rootIndex());
#ifndef QT_NO_TREEVIEW
        QTreeView *treeView = qobject_cast<QTreeView*>(view());
        if (treeView && treeView->header() && !treeView->header()->isHidden())
            listHeight += treeView->header()->height();
#endif
        while (!toCheck.isEmpty()) {
            QModelIndex parent = toCheck.pop();
            for (int i = 0; i < d->model->rowCount(parent); ++i) {
                QModelIndex idx = d->model->index(i, d->modelColumn, parent);
                if (!idx.isValid())
                    continue;
                listHeight += view()->visualRect(idx).height() + container->spacing();
#ifndef QT_NO_TREEVIEW
                if (d->model->hasChildren(idx) && treeView && treeView->isExpanded(idx))
                    toCheck.push(idx);
#endif
                ++count;
                if (!usePopup && count >= d->maxVisibleItems) {
                    toCheck.clear();
                    break;
                }
            }
        }
        listRect.setHeight(listHeight);
    }

    {
        // add the spacing for the grid on the top and the bottom;
        int heightMargin = 2*container->spacing();

        // add the frame of the container
        int marginTop, marginBottom;
        container->getContentsMargins(0, &marginTop, 0, &marginBottom);
        heightMargin += marginTop + marginBottom;

        //add the frame of the view
        view()->getContentsMargins(0, &marginTop, 0, &marginBottom);
        marginTop += static_cast<QAbstractScrollAreaPrivate *>(QObjectPrivate::get(view()))->top;
        marginBottom += static_cast<QAbstractScrollAreaPrivate *>(QObjectPrivate::get(view()))->bottom;
        heightMargin += marginTop + marginBottom;

        listRect.setHeight(listRect.height() + heightMargin);
    }

    // Add space for margin at top and bottom if the style wants it.
    if (usePopup)
        listRect.setHeight(listRect.height() + style->pixelMetric(QStyle::PM_MenuVMargin, &opt, this) * 2);

    // Make sure the popup is wide enough to display its contents.
    if (usePopup) {
        const int diff = d->computeWidthHint() - width();
        if (diff > 0)
            listRect.setWidth(listRect.width() + diff);
    }

    //we need to activate the layout to make sure the min/maximum size are set when the widget was not yet show
    container->layout()->activate();
    //takes account of the minimum/maximum size of the container
    listRect.setSize( listRect.size().expandedTo(container->minimumSize())
                      .boundedTo(container->maximumSize()));

    // make sure the widget fits on screen
    if (boundToScreen) {
        if (listRect.width() > screen.width() )
            listRect.setWidth(screen.width());
        if (mapToGlobal(listRect.bottomRight()).x() > screen.right()) {
            below.setX(screen.x() + screen.width() - listRect.width());
            above.setX(screen.x() + screen.width() - listRect.width());
        }
        if (mapToGlobal(listRect.topLeft()).x() < screen.x() ) {
            below.setX(screen.x());
            above.setX(screen.x());
        }
    }

    if (usePopup) {
        // Position horizontally.
        listRect.moveLeft(above.x());

#ifndef Q_WS_S60
        // Position vertically so the curently selected item lines up
        // with the combo box.
        const QRect currentItemRect = view()->visualRect(view()->currentIndex());
        const int offset = listRect.top() - currentItemRect.top();
        listRect.moveTop(above.y() + offset - listRect.top());
#endif


        // Clamp the listRect height and vertical position so we don't expand outside the
        // available screen geometry.This may override the vertical position, but it is more
        // important to show as much as possible of the popup.
        const int height = !boundToScreen ? listRect.height() : qMin(listRect.height(), screen.height());
        listRect.setHeight(height);

        if (boundToScreen) {
            if (listRect.top() < screen.top())
                listRect.moveTop(screen.top());
            if (listRect.bottom() > screen.bottom())
                listRect.moveBottom(screen.bottom());
        }
#ifdef Q_WS_S60
        if (screen.width() < screen.height()) {
            // in portait, menu should be positioned above softkeys
            listRect.moveBottom(screen.bottom());
        } else {
            TRect staConTopRect = TRect();
            AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EStaconTop, staConTopRect);
            listRect.setWidth(screen.height());
            //by default popup is centered on screen in landscape
            listRect.moveCenter(screen.center());
            if (staConTopRect.IsEmpty() && AknLayoutUtils::CbaLocation() != AknLayoutUtils::EAknCbaLocationBottom) {
                // landscape without stacon, menu should be at the right
                (opt.direction == Qt::LeftToRight) ? listRect.setRight(screen.right()) :
                                                     listRect.setLeft(screen.left());
            }
        }
#endif
    } else if (!boundToScreen || listRect.height() <= belowHeight) {
        listRect.moveTopLeft(below);
    } else if (listRect.height() <= aboveHeight) {
        listRect.moveBottomLeft(above);
    } else if (belowHeight >= aboveHeight) {
        listRect.setHeight(belowHeight);
        listRect.moveTopLeft(below);
    } else {
        listRect.setHeight(aboveHeight);
        listRect.moveBottomLeft(above);
    }

#ifndef QT_NO_IM
    if (QInputContext *qic = inputContext())
        qic->reset();
#endif
    QScrollBar *sb = view()->horizontalScrollBar();
    Qt::ScrollBarPolicy policy = view()->horizontalScrollBarPolicy();
    bool needHorizontalScrollBar = (policy == Qt::ScrollBarAsNeeded || policy == Qt::ScrollBarAlwaysOn)
                                   && sb->minimum() < sb->maximum();
    if (needHorizontalScrollBar) {
        listRect.adjust(0, 0, 0, sb->height());
    }
    container->setGeometry(listRect);

#ifndef Q_WS_MAC
    const bool updatesEnabled = container->updatesEnabled();
#endif

#if defined(Q_WS_WIN) && !defined(QT_NO_EFFECTS)
    bool scrollDown = (listRect.topLeft() == below);
    if (QApplication::isEffectEnabled(Qt::UI_AnimateCombo)
        && !style->styleHint(QStyle::SH_ComboBox_Popup, &opt, this) && !window()->testAttribute(Qt::WA_DontShowOnScreen))
        qScrollEffect(container, scrollDown ? QEffects::DownScroll : QEffects::UpScroll, 150);
#endif

// Don't disable updates on Mac OS X. Windows are displayed immediately on this platform,
// which means that the window will be visible before the call to container->show() returns.
// If updates are disabled at this point we'll miss our chance at painting the popup
// menu before it's shown, causing flicker since the window then displays the standard gray
// background.
#ifndef Q_WS_MAC
    container->setUpdatesEnabled(false);
#endif

    container->raise();
    container->show();
    container->updateScrollers();
    view()->setFocus();

    view()->scrollTo(view()->currentIndex(),
                     style->styleHint(QStyle::SH_ComboBox_Popup, &opt, this)
                             ? QAbstractItemView::PositionAtCenter
                             : QAbstractItemView::EnsureVisible);

#ifndef Q_WS_MAC
    container->setUpdatesEnabled(updatesEnabled);
#endif

    container->update();
#ifdef QT_KEYPAD_NAVIGATION
    if (QApplication::keypadNavigationEnabled())
        view()->setEditFocus(true);
#endif
    */
}

void TQSearchBox::hidePopup()
{
    d->popup->hide();
}

void TQSearchBox::setFilterText(const QString &text)
{
    d->proxy->setFilterWildcard(text);
}

//QModelIndex TQSearchBox::currentIndex() const
//{
//    return d->currentIndex;
//}

void TQSearchBox::focusList()
{
    if(!d->popup->isVisible())
        showPopup();
    if(isPopup())
        d->popup->setFocus();
    else
        d->popup->activateWindow();
    d->list->setCurrentIndex(d->list->model()->index(0,0));
}

void TQSearchBox::focusEdit()
{
    if(isPopup())
    {
//        setFocus(Qt::PopupFocusReason);
        hidePopup();
    }
    else
    {
        d->list->selectionModel()->clear();
        d->list->clearFocus();
        activateWindow();
    }
    QEventLoop eventLoop;
    QTimer::singleShot(60, &eventLoop, SLOT(quit()));
    eventLoop.exec();
}

void TQSearchBox::focusOutEvent(QFocusEvent *foc)
{
    if(foc->reason() != Qt::PopupFocusReason && foc->reason() != Qt::ActiveWindowFocusReason)
        hidePopup();
    QLineEdit::focusOutEvent(foc);
}

bool TQSearchBox::isPopup()
{
    bool isTool = ((int)d->popup->windowFlags() & Qt::Tool) == Qt::Tool;
    bool isToolTip = ((int)d->popup->windowFlags() & Qt::ToolTip) == Qt::ToolTip;
    return !isTool && !isToolTip;
}

void TQSearchBox::popupItemSelected(const QModelIndex &index)
{
    setText(index.data().toString());
    hidePopup();
    d->list->clearSelection();
}

void TQSearchBox::onTextChanged(const QString &text)
{
    if(!d->popup->isVisible())
        showPopup();
    setFilterText(text);
}

void TQSearchBox::activatedItem(const QModelIndex &index)
{
    QModelIndex sourceIndex = index;
    if(index.model() == d->proxy)
        sourceIndex = d->proxy->mapToSource(index);
    popupItemSelected(sourceIndex);
}

