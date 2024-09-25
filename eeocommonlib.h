#ifndef EEOCOMMONLIB_H
#define EEOCOMMONLIB_H

#include "eeocommonlib_global.h"
#include <QVector>
#include <QScreen>
#include <QWidget>

namespace totalcommon
{
    quint64 EEOCOMMONLIB_EXPORT getDirSize(const QString &dirPath);
    EEOCOMMONLIB_EXPORT QScreen *getScreen(int nScreenIndex);
    void EEOCOMMONLIB_EXPORT moveToScreenCenter(QWidget *pSrcWidget, int nDesScreenIndex);

    QImage EEOCOMMONLIB_EXPORT NinePatch(QString& picName, int iLeft, int iTop, int iRight, int iBottom, int DstWidth, int DstHeight);
    QPixmap EEOCOMMONLIB_EXPORT NinePatch(QPixmap& pic, int iLeft, int iTop, int iRight, int iBottom, int DstWidth, int DstHeight);

    EEOCOMMONLIB_EXPORT QScreen* getScreen(QPoint pos, bool* isPosInScreen = nullptr);
    QString EEOCOMMONLIB_EXPORT generateMd5(QString info);
    QString EEOCOMMONLIB_EXPORT HalfToFull(QString strInput);
    QString EEOCOMMONLIB_EXPORT FullToHalf(QString strInput);
    QString EEOCOMMONLIB_EXPORT FileMD5(const QString sPath,const QString appendStr);

    void EEOCOMMONLIB_EXPORT desktopShowInFolder(const QString &file_path);
}
class EEOCOMMONLIB_EXPORT eeocommonlib
{
public:
    eeocommonlib();

public:
    static void getImageFromPath(const QString& strFolder, QVector<QString>& vecFiles, bool bAnimation = false);
    static bool scaleAndClipPixmap(QPixmap& sourcePix, QSize destSize, int radius, QPixmap& destPix);
    static bool scaleAndClipPixmap2(QPixmap& sourcePix, QSizeF destSize, qreal radius, QPixmap& destPix);
};

#endif
