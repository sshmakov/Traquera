#include "ttfileiconprovider.h"

#if defined(Q_WS_WIN)
#  define _WIN32_IE 0x0500
#  include <qt_windows.h>
#  include <commctrl.h>
#  include <objbase.h>
#endif
#include <QPixmapCache>
#include <QDir>

TTFileIconProvider::TTFileIconProvider()
{
}

#ifdef Q_WS_WIN
QIcon TTFileIconProvider::getWinIcon(const QFileInfo &fileInfo) const
{
    QIcon retIcon;
    const QString fileExtension = QLatin1Char('.') + fileInfo.suffix().toUpper();

    QString key;
    if (fileInfo.isFile() && !fileInfo.isExecutable() && !fileInfo.isSymLink() && fileExtension != QLatin1String(".ICO"))
        key = QLatin1String("qt_") + fileExtension;

    QPixmap pixmap;
    if (!key.isEmpty()) {
        QPixmapCache::find(key, pixmap);
    }

    if (!pixmap.isNull()) {
        retIcon.addPixmap(pixmap);
        if (QPixmapCache::find(key + QLatin1Char('l'), pixmap))
            retIcon.addPixmap(pixmap);
        return retIcon;
    }

    /* We don't use the variable, but by storing it statically, we
     * ensure CoInitialize is only called once. */
    static HRESULT comInit = CoInitialize(NULL);
    Q_UNUSED(comInit);

    SHFILEINFO info;
    unsigned long val = 0;

    //Get the small icon
#ifndef Q_OS_WINCE
    val = SHGetFileInfo((const wchar_t *)QDir::toNativeSeparators(fileInfo.filePath()).utf16(), 0, &info,
                        sizeof(SHFILEINFO), SHGFI_ICON|SHGFI_SMALLICON|SHGFI_SYSICONINDEX|SHGFI_ADDOVERLAYS|SHGFI_OVERLAYINDEX
                        |SHGFI_USEFILEATTRIBUTES);
#else
    val = SHGetFileInfo((const wchar_t *)QDir::toNativeSeparators(fileInfo.filePath()).utf16(), 0, &info,
                        sizeof(SHFILEINFO), SHGFI_SMALLICON|SHGFI_SYSICONINDEX);
#endif

    // Even if GetFileInfo returns a valid result, hIcon can be empty in some cases
    if (val && info.hIcon) {
        if (fileInfo.isDir() && !fileInfo.isRoot()) {
            //using the unique icon index provided by windows save us from duplicate keys
            key = QString::fromLatin1("qt_dir_%1").arg(info.iIcon);
            QPixmapCache::find(key, pixmap);
            if (!pixmap.isNull()) {
                retIcon.addPixmap(pixmap);
                if (QPixmapCache::find(key + QLatin1Char('l'), pixmap))
                    retIcon.addPixmap(pixmap);
                DestroyIcon(info.hIcon);
                return retIcon;
            }
        }
        if (pixmap.isNull()) {
#ifndef Q_OS_WINCE
            pixmap = QPixmap::fromWinHICON(info.hIcon);
#else
            pixmap = QPixmap::fromWinHICON(ImageList_GetIcon((HIMAGELIST) val, info.iIcon, ILD_NORMAL));
#endif
            if (!pixmap.isNull()) {
                retIcon.addPixmap(pixmap);
                if (!key.isEmpty())
                    QPixmapCache::insert(key, pixmap);
            }
            else {
              qWarning("TTFileIconProviderPrivate::getWinIcon() no small icon found");
            }
        }
        DestroyIcon(info.hIcon);
    }

    //Get the big icon
#ifndef Q_OS_WINCE
    val = SHGetFileInfo((const wchar_t *)QDir::toNativeSeparators(fileInfo.filePath()).utf16(), 0, &info,
                        sizeof(SHFILEINFO), SHGFI_ICON|SHGFI_LARGEICON|SHGFI_SYSICONINDEX|SHGFI_ADDOVERLAYS|SHGFI_OVERLAYINDEX
                        |SHGFI_USEFILEATTRIBUTES);
#else
    val = SHGetFileInfo((const wchar_t *)QDir::toNativeSeparators(fileInfo.filePath()).utf16(), 0, &info,
                        sizeof(SHFILEINFO), SHGFI_LARGEICON|SHGFI_SYSICONINDEX);
#endif
    if (val && info.hIcon) {
        if (fileInfo.isDir() && !fileInfo.isRoot()) {
            //using the unique icon index provided by windows save us from duplicate keys
            key = QString::fromLatin1("qt_dir_%1").arg(info.iIcon);
        }
#ifndef Q_OS_WINCE
        pixmap = QPixmap::fromWinHICON(info.hIcon);
#else
        pixmap = QPixmap::fromWinHICON(ImageList_GetIcon((HIMAGELIST) val, info.iIcon, ILD_NORMAL));
#endif
        if (!pixmap.isNull()) {
            retIcon.addPixmap(pixmap);
            if (!key.isEmpty())
                QPixmapCache::insert(key + QLatin1Char('l'), pixmap);
        }
        else {
            qWarning("QFileIconProviderPrivate::getWinIcon() no large icon found");
        }
        DestroyIcon(info.hIcon);
    }
    return retIcon;
}
#endif


/*!
  Returns an icon for the file described by \a info.
*/

QIcon TTFileIconProvider::icon(const QFileInfo &info) const
{
    //Q_D(const QFileIconProvider);
#ifdef Q_WS_WIN

//    QIcon platformIcon = qt_guiPlatformPlugin()->fileSystemIcon(info);
//    if (!platformIcon.isNull())
//        return platformIcon;

    QIcon icon = getWinIcon(info);
    if (!icon.isNull())
        return icon;
    if (info.isRoot())
        return QFileIconProvider::icon(info);
    if (info.isFile())
    {
        if(info.exists())
            return QFileIconProvider::icon(info);
        else
            return getWinIcon(info);
    }
#endif
    return QFileIconProvider::icon(info);
}

/*!
  Returns the type of the file described by \a info.
*/
