#ifndef PREVIEW_H
#define PREVIEW_H

#include  <windows.h>
#include <ole2.h>
#include <QWidget>
#include <QAxWidget>


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
private:
    QAxObject *view;
    
public:
    explicit Preview(QWidget *parent = 0);
    ~Preview();

    bool setSourceFile(const QString &file);
    void clear();
    bool isActive();
protected:
    virtual void resizeEvent(QResizeEvent *event);
private:
    Ui::Preview *ui;
};

#endif // PREVIEW_H
