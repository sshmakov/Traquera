#include "preview.h"
#include "ui_preview.h"
#include "previewfactory.h"
#include "highlighter.h"

#include <QtGui>
#include <QAxObject>
#include <QAxWidget>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QVBoxLayout>
#include <QUuid>
#include <propsys.h>
#include <Shlwapi.h>
#include <ShObjIdl.h>

const char *CLASS_IPreviewHandler = "{8895b1c6-b41f-4c1c-a562-0d564250836f}";


PreviewActiveX::PreviewActiveX(QWidget *parent) :
    Preview(parent)
{
    //view = ui->axWidget;
    view = new QAxObject(this);
    mode = NoPreview;
    //QLayout *lay = new QVBoxLayout(this);
    //lay->addWidget(view);
}

PreviewActiveX::~PreviewActiveX()
{
    //delete view;
}

#define STR_IInitializeWithFile "b7d14566-0509-4cce-a71f-0a554233bd9b"


bool PreviewActiveX::setSourceFile(const QString &fileName)
{
    clear();
    QString ext;
    QFileInfo info(fileName);
    QString filePath = QDir::toNativeSeparators(info.absoluteFilePath());
    ext = info.suffix();
    if(ext.isEmpty())
        return false;
    ext = "." + ext;
    QString regPath = "HKEY_CLASSES_ROOT\\" + ext + "\\shellex\\" + CLASS_IPreviewHandler;
    QSettings set(regPath, QSettings::NativeFormat);
    if(!set.contains("."))
        return false;
    QString classId = set.value(".").toString();
    if(classId.isEmpty())
        return false;
    if(!view->setControl(classId))
        return false;
    IInitializeWithFile *iwfile;
    IInitializeWithStream *iwstream;
    IStream * iis;
    if(!view->queryInterface(QUuid(IID_IInitializeWithFile), (void**)&iwfile))
    {
        iwfile->Initialize((LPCWSTR)filePath.utf16(), STGM_READ);
        iwfile->Release();
    }
    else if (!view->queryInterface(QUuid(IID_IInitializeWithStream),(void**)&iwstream))
    {

        if(SHCreateStreamOnFileW((LPCWSTR)filePath.utf16(), STGM_READ, &iis))
        {
            iwstream->Release();
            return false;
        }
        HRESULT res = iwstream->Initialize(iis, STGM_READ);
        iwstream->Release();
        iis->Release();
        if(res)
            return false;
    }
    IPreviewHandler *handler=0;
    if(view->queryInterface(IID_IPreviewHandler, (void**)&handler))
        return false;
    RECT r;
    connect(view,SIGNAL(destroyed()),this,SLOT(viewDestroyed()));
    r.top =0;
    r.left =0;
    r.bottom = this->height();
    r.right = this->width();
    handler->SetWindow(this->winId(),&r);
    handler->DoPreview();
    handler->SetFocus();
    handler->Release();
    return true;
}

QString PreviewActiveX::previewClass(const QString& ext)
{
    QString regPath = "HKEY_CLASSES_ROOT\\" + ext + "\\shellex\\" + CLASS_IPreviewHandler;
    QSettings set(regPath, QSettings::NativeFormat);
    if(!set.contains("."))
        return false;
    return set.value(".").toString();
}

void PreviewActiveX::clear()
{
    if(!view->isNull())
    {
        IPreviewHandler *handler=0;
        if(!view->queryInterface(IID_IPreviewHandler, (void**)&handler))
        {
            if(handler)
            {
                handler->Unload();
                handler->AddRef();
                handler->Release();
            }
        }
        view->clear();
//        if(iis)
//        {
//            iis->Release();
//            iis=0;
//        }
//        if(iwfile)
//        {
//            iwfile->Release();
//            iwfile=0;
//        }
//        if(iwstream)
//        {
//            iwstream->Release();
//            iwstream=0;
//        }
    }
}

bool PreviewActiveX::isActive()
{
    return !view->isNull();
}

void PreviewActiveX::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if(!isActive())
        return;
    if(view->isNull())
        return;
    IPreviewHandler *handler=0;
    if(view->queryInterface(IID_IPreviewHandler, (void**)&handler))
        return;
    if(handler)
    {
        RECT r;
        r.top =0;
        r.left =0;
        r.bottom = this->height();
        r.right = this->width();
        handler->SetRect(&r);
        handler->Release();
    }

}

void PreviewActiveX::viewDestroyed()
{
}


Preview::Preview(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::Preview)
{
    ui->setupUi(this);
}

Preview::~Preview()
{
    delete ui;
}


PreviewTxt::PreviewTxt(QWidget *parent)
    : Preview(parent)
{
    QVBoxLayout *lay = new QVBoxLayout();
    lay->setContentsMargins(0, 3, 0, 0);
    setLayout(lay);
    encSelector = new QComboBox(this);
    QList<QByteArray> codecs = QTextCodec::availableCodecs();
    QMap<QString,QString> codecNames;
    foreach(const QByteArray &buf, codecs)
    {
        QString codec = QString::fromUtf8(buf.constData());
        codecNames.insert(codec.toLower(), codec);
    }
    encSelector->addItems(codecNames.values());
    int i = codecNames.values().indexOf(QTextCodec::codecForLocale()->name());
    encSelector->setCurrentIndex(i);
    QHBoxLayout *toolLay = new QHBoxLayout();
    toolLay->setContentsMargins(3,0,3,0);
    toolLay->addWidget(new QLabel(tr("кодировка"),this));
    toolLay->addWidget(encSelector);
    toolLay->addStretch();
    lay->addLayout(toolLay);
    editor = new QPlainTextEdit(this);
    editor->setReadOnly(true);
    highlighter = new Highlighter(editor->document());
    lay->addWidget(editor);
    //layout()->addWidget(editor);
    connect(encSelector, SIGNAL(currentIndexChanged(QString)), SLOT(encodingChange(QString)));
    setVisible(true);
}

PreviewTxt::~PreviewTxt()
{
}

bool PreviewTxt::setSourceFile(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
        return false;
    buf = file.readAll();
    editor->setPlainText(QTextCodec::codecForName(encSelector->currentText().toLatin1())->toUnicode(buf));
    return true;
}

void PreviewTxt::clear()
{
    editor->clear();
}

void PreviewTxt::setSyntax(const QString &syntax)
{
    highlighter->setSyntax(syntax);
}

void PreviewTxt::encodingChange(const QString &newCodecName)
{
    if(newCodecName.isEmpty())
        return;
    editor->setPlainText(QTextCodec::codecForName(encSelector->currentText().toLatin1())->toUnicode(buf));
//    editor->set
}

// MasterPreview

MasterPreview::MasterPreview(QWidget *parent)
    :Preview(parent), view(0)
{
    lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    factory = new PreviewFactory(this);
}

MasterPreview::~MasterPreview()
{
}

bool MasterPreview::setSourceFile(const QString &fileName)
{
    clear();
    view = factory->createPreview(fileName,this);
    if(view)
    {
        lay->addWidget(view);
        return view->setSourceFile(fileName);
    }
    return false;
}

void MasterPreview::clear()
{
    if(view)
    {
        view->clear();
        delete view;
        view = 0;
    }

}


PreviewImage::PreviewImage(QWidget *parent)
{
}

PreviewImage::~PreviewImage()
{
    clear();
}

bool PreviewImage::setSourceFile(const QString &fileName)
{
    return image.load(fileName);
}

void PreviewImage::clear()
{
    image = QImage();
    resized = QImage();
}

void PreviewImage::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
//    int marginH=10, marginV=10;
//    QRectF src(0,0,image.width(),image.height());
//    qreal aspect,w = image.width(),h = image.height();
//    qreal pw = width() - 2 * marginH;
//    qreal ph = height() - 2 * marginV;
//    if(image.height() && image.width())
//        aspect = src.width()/src.height();
//    else
//        aspect = 1;
//    if(w>pw)
//    {
//        w = pw;
//        h = w / aspect;
//    }
//    if(h>ph)
//    {
//        h = ph;
//        w = h * aspect;
//    }
//    QRectF target(0,0,w,h);
//    QPointF c(this->rect().center());
//    target.moveCenter(c);
//    p.drawImage(target,image,src);

    if(resized.isNull() && !image.isNull())
        if(image.width() > width() || image.height() > height())
            resized = image.scaled(size(),Qt::KeepAspectRatio, Qt::SmoothTransformation);
        else
            resized = image;

    QRectF target = resized.rect();
    QPointF c(this->rect().center());
    target.moveCenter(c);
    p.drawImage(target,resized,resized.rect());
}

void PreviewImage::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    resized = QImage();
//    if(!image.isNull())
//        resized = image.scaled(size(),Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

