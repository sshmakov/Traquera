#include "preview.h"
#include "ui_preview.h"

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


Preview::Preview(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Preview)
{
    ui->setupUi(this);
    //view = ui->axWidget;
    view = new QAxObject(this);
    mode = NoPreview;
    //QLayout *lay = new QVBoxLayout(this);
    //lay->addWidget(view);
}

Preview::~Preview()
{
    //delete view;
    delete ui;
}

#define STR_IInitializeWithFile "b7d14566-0509-4cce-a71f-0a554233bd9b"

bool Preview::setSourceFile(const QString &file)
{
    clear();
    QString ext;
    QFileInfo info(file);
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
    IInitializeWithFile *iwfile=0;
    IInitializeWithStream *iwstream=0;
    IStream * iis;
    if(!view->queryInterface(QUuid(IID_IInitializeWithFile), (void**)&iwfile))
        iwfile->Initialize((LPCWSTR)filePath.utf16(), STGM_READ);
    else if (!view->queryInterface(QUuid(IID_IInitializeWithStream),(void**)&iwstream))
    {
        if(SHCreateStreamOnFileW((LPCWSTR)filePath.utf16(), STGM_READ, &iis))
            return false;
        if(iwstream->Initialize(iis, STGM_READ))
            return false;
    }
    IPreviewHandler *handler=0;
    if(view->queryInterface(IID_IPreviewHandler, (void**)&handler))
        return false;
    RECT r;
    r.top =0;
    r.left =0;
    r.bottom = this->height();
    r.right = this->width();
    handler->SetWindow(this->winId(),&r);
    handler->DoPreview();
    handler->SetFocus();
    /*
  if FPreviewHandler.QueryInterface(IInitializeWithFile, FileInit) = 0 then
  begin
    FileInit.Initialize(StringToOleStr(FFileName), STGM_READ);
    FInStreamMode := False;
    FLoaded := True;
  end else
  if FPreviewHandler.QueryInterface(IInitializeWithStream, StreamInit) = 0 then
  begin
    try
      FFileStream := TFileStream.Create(FFileName, fmOpenRead);
    except on
      E: EFOpenError do
      begin
        MessageDlg(E.Message, mtError, [mbOK], 0);
        Result := False;
        Exit;
      end;
    end;
    FIStream := TStreamAdapter.Create(FFileStream, soOwned) as IStream;
    StreamInit.Initialize(FIStream, STGM_READ);
    FInStreamMode := True;
    FLoaded := True;
  end else
  begin // Cannot load file
    Result := False;
    FPreviewHandler.Unload;
    Exit;
  end;

  ARect := Rect(0, 0, AParentControl.Width, AParentControl.Height);
  Parent := AParentControl;
  Align := alClient;
  FPreviewHandler.SetWindow(Self.Handle, ARect);
  FPreviewHandler.SetRect(ARect);
  FPreviewHandler.DoPreview;
  FPreviewHandler.SetFocus;*/

    return true;
}

void Preview::clear()
{
    if(!view->isNull())
    {
        IPreviewHandler *handler=0;
        if(!view->queryInterface(IID_IPreviewHandler, (void**)&handler))
        {
            if(handler)
                handler->Unload();
            view->clear();
        }
    }
}

bool Preview::isActive()
{
    return !view->isNull();
}

void Preview::resizeEvent(QResizeEvent *event)
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
    }

}
