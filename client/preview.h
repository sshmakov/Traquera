#ifndef PREVIEW_H
#define PREVIEW_H

#include  <windows.h>
#include <ole2.h>
#include <QWidget>
#include <QAxWidget>
#include <QPlainTextEdit>


//struct RECT
//{
//    long int    left,  top,  right, bottom;
//};


    //MIDL_INTERFACE("8895b1c6-b41f-4c1c-a562-0d564250836f")
/*
    class IPreviewHandler
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE QueryInterface() = 0;
        virtual ULONG STDMETHODCALLTYPE AddRef() = 0;
        virtual ULONG STDMETHODCALLTYPE Release() = 0;
        virtual HRESULT STDMETHODCALLTYPE SetWindow(HWND hwnd, const RECT *prc) = 0;

        virtual HRESULT STDMETHODCALLTYPE SetRect(const RECT *prc) = 0;

        virtual HRESULT STDMETHODCALLTYPE DoPreview( void) = 0;

        virtual HRESULT STDMETHODCALLTYPE Unload( void) = 0;

        virtual HRESULT STDMETHODCALLTYPE SetFocus( void) = 0;

        virtual HRESULT STDMETHODCALLTYPE QueryFocus(HWND *phwnd) = 0;

        virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator(MSG *pmsg) = 0;

    };*/

extern const char *CLASS_IPreviewHandler;

MIDL_INTERFACE("b7d14566-0509-4cce-a71f-0a554233bd9b")
TheIInitializeWithFile : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE Initialize(LPCWSTR pszFilePath, DWORD grfMode) = 0;

};


namespace Ui {
class Preview;
}

class QAxBase;
class QAxObject;
class IPreviewHandler;

class Preview : public QWidget
{
    Q_OBJECT
public:
    explicit Preview(QWidget *parent = 0);
    ~Preview();
    virtual bool setSourceFile(const QString &fileName) = 0;
    virtual void clear() = 0;
private:
    Ui::Preview *ui;
};

class PreviewActiveX : public Preview
{
    Q_OBJECT
private:
    QAxObject *view;
    enum Mode {NoPreview, ActiveX, Internal};
    Mode mode;
public:
    explicit PreviewActiveX(QWidget *parent = 0);
    ~PreviewActiveX();

    bool setSourceFile(const QString &fileName);
    void clear();
    bool isActive();
    static QString previewClass(const QString &ext);
protected:
    virtual void resizeEvent(QResizeEvent *event);
protected slots:
    void viewDestroyed();
};

class QTextEdit;
class Highlighter;

class PreviewTxt : public Preview
{
    Q_OBJECT
private:
    QPlainTextEdit *editor;
    Highlighter *highlighter;
public:
    explicit PreviewTxt(QWidget *parent = 0);
    ~PreviewTxt();

    bool setSourceFile(const QString &fileName);
    void clear();
    void setSyntax(const QString &syntax);
};

class PreviewImage : public Preview
{
    Q_OBJECT
private:
    QImage image, resized;
public:
    explicit PreviewImage(QWidget *parent = 0);
    ~PreviewImage();

    bool setSourceFile(const QString &fileName);
    void clear();
    void paintEvent(QPaintEvent *event);
protected:
    virtual void resizeEvent(QResizeEvent *event);
};

class PreviewFactory;

class MasterPreview: public Preview
{
    Q_OBJECT
private:
    QLayout *lay;
    Preview *view;
    PreviewFactory *factory;
public:
    explicit MasterPreview(QWidget *parent = 0);
    ~MasterPreview();

    bool setSourceFile(const QString &fileName);
    void clear();

};

#endif // PREVIEW_H
