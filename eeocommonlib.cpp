#include "eeocommonlib.h"
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QGuiApplication>
#include <QPainter>
#include <QCryptographicHash>
#include <QDebug>
#include <QProcess>
#include <QDesktopServices>
#include <QUrl>
#include <QPainterPath>
#include <QtMath>

eeocommonlib::eeocommonlib()
{
}

void eeocommonlib::getImageFromPath(const QString& strFolder, QVector<QString>& vecFiles, bool bAnimation)
{
	QDirIterator it(strFolder, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
	while (it.hasNext())
	{
		QString name = it.next();
		QFileInfo info(name);
		if (info.isDir())
		{
			getImageFromPath(name, vecFiles);
		}
		else
		{
            if (bAnimation)
            {
				if (info.suffix().compare("webp", Qt::CaseInsensitive) == 0 || info.suffix().compare("gif", Qt::CaseInsensitive) == 0)
				{
					vecFiles.push_back(name);
				}
            }
			else if (info.suffix() == "jpg" || info.suffix() == "bmp" || info.suffix() == "png" || info.suffix() == "svg")
			{
				vecFiles.push_back(name);
			}
		}
	}
}

bool eeocommonlib::scaleAndClipPixmap(QPixmap& sourcePix, QSize destSize, int radius, QPixmap& destPix)
{
    if (sourcePix.isNull())
    {
        return false;
    }

	int baseHeight = destSize.height();
	int baseWidth = destSize.width();
    qreal scaleBase = baseWidth * 1.0 / baseHeight;
    qreal scalePix = sourcePix.width() * 1.0 / sourcePix.height();
    if ((baseHeight == sourcePix.height() && baseWidth == sourcePix.width()) || qAbs(scaleBase - scalePix) <= 0.000001)
    {
		QSize pixSize(baseWidth, baseHeight);
		QImage clipPixmap(pixSize, QImage::Format_RGBA8888);
		clipPixmap.fill(Qt::transparent);
		QPainter* painter = new QPainter(&clipPixmap);
		painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
		QPainterPath path;
		QRect rect(QPoint(0, 0), pixSize);
		path.addRoundedRect(rect, radius, radius);
		painter->setClipPath(path);
		painter->drawPixmap(rect, sourcePix);
        painter->end();
		destPix = QPixmap::fromImage(clipPixmap);
    }
    else
    {
        QPixmap scaledPixmap(sourcePix);
		qreal pixWidth = sourcePix.width();
		qreal pixHeight = sourcePix.height();
		qreal widthRatio = (qreal)pixWidth / (qreal)baseWidth;
		qreal heightRatio = (qreal)pixHeight / (qreal)baseHeight;
        int newHeight = 0;
        int newWidth = 0;
		if (widthRatio > heightRatio)
		{
            newHeight = baseHeight;
            newWidth = pixWidth * newHeight / pixHeight;

            QImage bigImage(QSize(newWidth, newHeight), QImage::Format_RGBA8888);
            bigImage.fill(Qt::transparent);
			QPainter* scalePainter = new QPainter(&bigImage);
            scalePainter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
            scalePainter->drawPixmap(QPoint(0,0), scaledPixmap);
            scalePainter->end();
			if (baseWidth != newWidth)
			{
				scaledPixmap = QPixmap::fromImage(bigImage.copy(QRect((newWidth - baseWidth) / 2, 0, baseWidth,
					baseHeight)));
			}
            else
            {
                scaledPixmap = QPixmap::fromImage(bigImage);
            }
		}
		else
		{
			newWidth = baseWidth;
            newHeight = pixHeight * newWidth / pixWidth;
			QImage bigImage(QSize(newWidth, newHeight), QImage::Format_RGBA8888);
			bigImage.fill(Qt::transparent);
			QPainter* scalePainter = new QPainter(&bigImage);
			scalePainter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
			scalePainter->drawPixmap(QPoint(0, 0), scaledPixmap);
			scalePainter->end();
			if (baseHeight != newHeight)
			{
				scaledPixmap = QPixmap::fromImage(bigImage.copy(QRect(0, (newHeight - baseHeight) / 2, baseWidth,
					baseHeight)));
			}
			else
			{
				scaledPixmap = QPixmap::fromImage(bigImage);
			}
		}      

        QSize pixSize(baseWidth, baseHeight);
		QImage clipPixmap(pixSize, QImage::Format_RGBA8888);
		clipPixmap.fill(Qt::transparent);
		QPainter* painter = new QPainter(&clipPixmap);
		painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
		QPainterPath path;
		QRect rect(QPoint(0, 0), pixSize);
		path.addRoundedRect(rect, radius, radius);
		painter->setClipPath(path);
		painter->drawPixmap(rect, scaledPixmap);

		destPix = QPixmap::fromImage(clipPixmap);        
    }
    return true;
}

bool eeocommonlib::scaleAndClipPixmap2(QPixmap& sourcePix, QSizeF destSize, qreal radius, QPixmap& destPix)
{
	if (sourcePix.isNull())
	{
		return false;
	}

	qreal baseHeight = destSize.height();
	qreal baseWidth = destSize.width();
	qreal scaleBase = baseWidth * 1.0 / baseHeight;
	qreal scalePix = sourcePix.width() * 1.0 / sourcePix.height();
	if ((baseHeight == sourcePix.height() && baseWidth == sourcePix.width()) || qAbs(scaleBase - scalePix) <= 0.000001)
	{
        QPixmap scaledPix = sourcePix.scaled(QSize(baseWidth, baseHeight), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        if (radius > 0)
        {
            QSizeF pixSize(baseWidth, baseHeight);
            QImage clipPixmap(pixSize.toSize(), QImage::Format_RGBA8888);
            clipPixmap.fill(Qt::transparent);
            QPainter* painter = new QPainter(&clipPixmap);
            painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
            QPainterPath path;
            QRectF rect(QPoint(0, 0), pixSize);
            path.addRoundedRect(rect, radius, radius);
            painter->setClipPath(path);
            painter->drawPixmap(rect.toRect(), scaledPix);
            painter->end();
            destPix = QPixmap::fromImage(clipPixmap);
        }
	}
	else
	{
        QPixmap scaledPix;
		qreal pixWidth = sourcePix.width();
		qreal pixHeight = sourcePix.height();
		qreal widthRatio = (qreal)pixWidth / (qreal)baseWidth;
		qreal heightRatio = (qreal)pixHeight / (qreal)baseHeight;
		int newHeight = 0;
		int newWidth = 0;
		if (widthRatio > heightRatio)
		{
			newHeight = baseHeight;
			newWidth = pixWidth * newHeight / pixHeight;

            scaledPix = sourcePix.scaled(QSize(newWidth, newHeight), Qt::KeepAspectRatio, Qt::SmoothTransformation);
			if (baseWidth != newWidth)
			{
                scaledPix = scaledPix.copy(QRect((newWidth - baseWidth) / 2, 0, baseWidth,
					baseHeight));
			}
		}
		else
		{
			newWidth = baseWidth;
			newHeight = pixHeight * newWidth / pixWidth;
            scaledPix = sourcePix.scaled(QSize(newWidth, newHeight), Qt::KeepAspectRatio, Qt::SmoothTransformation);
			if (baseHeight != newHeight)
			{
                scaledPix = scaledPix.copy(QRect(0, (newHeight - baseHeight) / 2, baseWidth,
					baseHeight));
			}
		}

		QSize pixSize(baseWidth, baseHeight);
		QImage clipPixmap(pixSize, QImage::Format_RGBA8888);
		clipPixmap.fill(Qt::transparent);
		QPainter* painter = new QPainter(&clipPixmap);
		painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
		QPainterPath path;
		QRect rect(QPoint(0, 0), pixSize);
		path.addRoundedRect(rect, radius, radius);
		painter->setClipPath(path);
		painter->drawPixmap(rect, scaledPix);

		destPix = QPixmap::fromImage(clipPixmap);
	}
	return true;
}

quint64 totalcommon::getDirSize(const QString &dirPath)
{
    QDir dir(dirPath);
   quint64 size = 0;
   foreach(QFileInfo fileInfo, dir.entryInfoList(QDir::Files))
   {
       //calculate size
       size += fileInfo.size();
   }

   foreach(QString subDir, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
   {
       //have sub dir
       size += getDirSize(dirPath + QDir::separator() + subDir);
   }

   return size;
}

bool lessScreenXY(QScreen* &a, QScreen* &b)
{
    int aX = a->geometry().x();
    int bX = b->geometry().x();

    if (aX < bX)
    {
        return true;
    }
    else if (aX > bX)
    {
        return false;
    }
    else
    {
        int aY = a->geometry().y();
        int bY = b->geometry().y();
        if (aY < bY)
        {
            return true;
        }
        else if (aY > bY)
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    return true;
}
QScreen *totalcommon::getScreen(int nScreenIndex)
{
    QScreen* pDesScreen = nullptr;
   QVector<QScreen*> screens = QVector<QScreen*>::fromList(QGuiApplication::screens());
    if (screens.empty())
        return pDesScreen;

    std::sort(screens.begin(), screens.end(), lessScreenXY);
    int nCount = screens.size();
    do
    {
        if (nScreenIndex >= nCount)
        {
            --nScreenIndex;
            continue;
        }

        if (nScreenIndex < 0)
            break;

        pDesScreen = screens.at(nScreenIndex--);
    }
    while (pDesScreen == nullptr);

    return pDesScreen;
}

void totalcommon::moveToScreenCenter(QWidget *pSrcWidget, int nDesScreenIndex)
{
    if (pSrcWidget == nullptr)
    {
        return;
    }

    if (pSrcWidget->parent() != nullptr)
        return;

    QScreen *pWidget = totalcommon::getScreen(nDesScreenIndex);
    if (pWidget == nullptr)
    {
        return;
    }

    QRect rect = pWidget->availableGeometry();
    pSrcWidget->move(rect.x() + (rect.width() - pSrcWidget->width()) / 2.0,
        rect.y() + (rect.height() - pSrcWidget->height()) / 2.0);
}

QImage totalcommon::NinePatch(QString &picName, int iLeft, int iTop, int iRight, int iBottom, int DstWidth, int DstHeight)
{
    QImage pix(picName);

    int pixWidth = pix.width();
    int pixHeight = pix.height();

    QImage pix_1 = pix.copy(0, 0, iLeft, iTop);
    QImage pix_2 = pix.copy(iLeft, 0, pixWidth - iLeft - iRight, iTop);
    QImage pix_3 = pix.copy(pixWidth - iRight, 0, iRight, iTop);

    QImage pix_4 = pix.copy(0, iTop, iLeft, pixHeight - iTop - iBottom);
    QImage pix_5 = pix.copy(iLeft, iTop, pixWidth - iLeft - iRight, pixHeight - iTop - iBottom);
    QImage pix_6 = pix.copy(pixWidth - iRight, iTop, iRight, pixHeight - iTop - iBottom);

    QImage pix_7 = pix.copy(0, pixHeight - iBottom, iLeft, iBottom);
    QImage pix_8 = pix.copy(iLeft, pixHeight - iBottom, pixWidth - iLeft - iRight, iBottom);
    QImage pix_9 = pix.copy(pixWidth - iRight, pixHeight - iBottom, iRight, iBottom);

    pix_2 = pix_2.scaled(DstWidth - iLeft - iRight, iTop, Qt::IgnoreAspectRatio);
    pix_4 = pix_4.scaled(iLeft, DstHeight - iTop - iBottom, Qt::IgnoreAspectRatio);
    pix_5 = pix_5.scaled(DstWidth - iLeft - iRight, DstHeight - iTop - iBottom, Qt::IgnoreAspectRatio);
    pix_6 = pix_6.scaled(iRight, DstHeight - iTop - iBottom, Qt::IgnoreAspectRatio);
    pix_8 = pix_8.scaled(DstWidth - iLeft - iRight, iBottom);


    QImage resultImg(DstWidth, DstHeight, QImage::Format_RGBA8888);
    resultImg.fill(Qt::transparent);
    QPainter* painter = new QPainter(&resultImg);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    if (!resultImg.isNull()) {
        painter->drawPixmap(0, 0, QPixmap::fromImage(pix_1));
        painter->drawPixmap(iLeft, 0, QPixmap::fromImage(pix_2));
        painter->drawPixmap(DstWidth - iRight, 0, QPixmap::fromImage(pix_3));

        painter->drawPixmap(0, iTop, QPixmap::fromImage(pix_4));
        painter->drawPixmap(iLeft, iTop, QPixmap::fromImage(pix_5));
        painter->drawPixmap(DstWidth - iRight, iTop, QPixmap::fromImage(pix_6));

        painter->drawPixmap(0, DstHeight - iBottom, QPixmap::fromImage(pix_7));
        painter->drawPixmap(iLeft, DstHeight - iBottom, QPixmap::fromImage(pix_8));
        painter->drawPixmap(DstWidth - iRight, DstHeight - iBottom, QPixmap::fromImage(pix_9));
        painter->end();
    }
    return resultImg;
}

QPixmap totalcommon::NinePatch(QPixmap &pic, int iLeft, int iTop, int iRight, int iBottom, int DstWidth, int DstHeight)
{
    QPixmap pix(pic);

    int pixWidth = pix.width();
    int pixHeight = pix.height();

    QPixmap pix_1 = pix.copy(0, 0, iLeft, iTop);
    QPixmap pix_2 = pix.copy(iLeft, 0, pixWidth - iLeft - iRight, iTop);
    QPixmap pix_3 = pix.copy(pixWidth - iRight, 0, iRight, iTop);

    QPixmap pix_4 = pix.copy(0, iTop, iLeft, pixHeight - iTop - iBottom);
    QPixmap pix_5 = pix.copy(iLeft, iTop, pixWidth - iLeft - iRight, pixHeight - iTop - iBottom);
    QPixmap pix_6 = pix.copy(pixWidth - iRight, iTop, iRight, pixHeight - iTop - iBottom);

    QPixmap pix_7 = pix.copy(0, pixHeight - iBottom, iLeft, iBottom);
    QPixmap pix_8 = pix.copy(iLeft, pixHeight - iBottom, pixWidth - iLeft - iRight, iBottom);
    QPixmap pix_9 = pix.copy(pixWidth - iRight, pixHeight - iBottom, iRight, iBottom);

    pix_2 = pix_2.scaled(DstWidth - iLeft - iRight, iTop, Qt::IgnoreAspectRatio);
    pix_4 = pix_4.scaled(iLeft, DstHeight - iTop - iBottom, Qt::IgnoreAspectRatio);
    pix_5 = pix_5.scaled(DstWidth - iLeft - iRight, DstHeight - iTop - iBottom, Qt::IgnoreAspectRatio);
    pix_6 = pix_6.scaled(iRight, DstHeight - iTop - iBottom, Qt::IgnoreAspectRatio);
    pix_8 = pix_8.scaled(DstWidth - iLeft - iRight, iBottom);


    QPixmap resultImg(DstWidth, DstHeight);
    resultImg.fill(Qt::transparent);
    QPainter* painter = new QPainter(&resultImg);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    if (!resultImg.isNull()) {
        painter->drawPixmap(0, 0, pix_1);
        painter->drawPixmap(iLeft, 0, pix_2);
        painter->drawPixmap(DstWidth - iRight, 0, pix_3);

        painter->drawPixmap(0, iTop, pix_4);
        painter->drawPixmap(iLeft, iTop, pix_5);
        painter->drawPixmap(DstWidth - iRight, iTop, pix_6);

        painter->drawPixmap(0, DstHeight - iBottom, pix_7);
        painter->drawPixmap(iLeft, DstHeight - iBottom, pix_8);
        painter->drawPixmap(DstWidth - iRight, DstHeight - iBottom, pix_9);
        painter->end();
    }
    return resultImg;
}

QScreen* totalcommon::getScreen(QPoint pos, bool* isPosInScreen)
{
	QScreen* screen = nullptr;

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
	screen = qApp->screenAt(pos);
#else
	int screenIndex = qApp->desktop()->screenNumber(pos);
	auto screenList = qApp->screens();
	if (0 <= screenIndex && screenIndex < screenList.size())
		screen = screenList.at(screenIndex);
#endif

	if (!screen) {
		screen = qApp->primaryScreen();

		if (isPosInScreen != nullptr) *isPosInScreen = false;
	}
	else if (isPosInScreen != nullptr) {
		*isPosInScreen = true;
	}

	return screen;
}

QString totalcommon::generateMd5(QString strInfo)
{
	QString sourceMd5;
    if (!strInfo.isEmpty())
    {
		QCryptographicHash hash(QCryptographicHash::Md5);
		hash.addData(strInfo.toStdString().c_str());
		sourceMd5 = hash.result().toHex();
    }
    else
    {
        qInfo() << "str info is empty";
    }
    return sourceMd5;
}
QString totalcommon::HalfToFull(QString strInput)
{
    QString tmp;
    for (int i = 0; i < strInput.length(); i++) {
        QChar c = strInput.at(i);
        if (c.unicode() == 32) {
            tmp.append(QChar(12288));
        }
        else if (c.unicode() == 46) {
            tmp.append(QChar(12290));
        }
        else if (c.unicode() < 127) {
            tmp.append(QChar(c.unicode() + 65248));
        }
        else
            tmp.append(c);
    }
    return tmp;
}

QString totalcommon::FullToHalf(QString strInput)
{
    QString tmp;
    for (int i = 0; i < strInput.length(); i++) {
        QChar c = strInput.at(i);
        if (c.unicode() > 65280 && c.unicode() < 65375) {
            tmp.append(QChar(c.unicode() - 65248));
        }
        else if (c.unicode() == 12288) {
            tmp.append(QChar(32));
        }
        else if (c.unicode() == 12290) {
            tmp.append(QChar(46));
        }
        else {
            tmp.append(c);
        }
    }
    return tmp;
}

QString totalcommon::FileMD5(const QString sPath,const QString appendStr)
{
    if(!sPath.isEmpty())
    {
//        QFileInfo sFile(sPath);
//        qint64  iSize = sFile.size();
//        qInfo()<<"AAAAA--iSize:"<<iSize;

        QFile sourceFile(sPath);
        if(sourceFile.exists())
        {
            qint64 fileSize = sourceFile.size();
            const qint64 bufferSize = 10240;

            if (sourceFile.open(QIODevice::ReadOnly)) {
                char buffer[bufferSize];
                int bytesRead;
                int readSize = qMin(fileSize, bufferSize);

                QCryptographicHash hash(QCryptographicHash::Md5);

                while (readSize > 0 && (bytesRead = sourceFile.read(buffer, readSize)) > 0) {
                    fileSize -= bytesRead;
                    hash.addData(buffer, bytesRead);
                    readSize = qMin(fileSize, bufferSize);
                    break;
                }

                if(!appendStr.isEmpty())
                {
                    hash.addData(appendStr.toUtf8());
                }
                sourceFile.close();
                return QString(hash.result().toHex());
            }
        }
        else
        {
            qInfo()<<"AAAAA--file not exist";
        }
    }
    return QString();
}

void totalcommon::desktopShowInFolder(const QString &file_path)
{
    bool success = false;

#if defined(Q_OS_WIN)
    QString command = "explorer.exe";
    QStringList arguments;
    QString path = QDir::toNativeSeparators(file_path);
    arguments << "/select," << path + "";
    success = QProcess::startDetached(command, arguments);
#elif defined(Q_OS_MAC)
    QStringList script_args;
    QString escaped_path = file_path;

    escaped_path.replace('"', "\\\"");
    script_args << "-e"
              << QString("tell application \"Finder\" to reveal POSIX file \"%1\"")
                                    .arg(escaped_path);
    if (QProcess::execute("/usr/bin/osascript", script_args) == 0)
    {
       success = true;
       script_args.clear();
       script_args << "-e"
                  << "tell application \"Finder\" to activate";
       QProcess::execute("/usr/bin/osascript", script_args);
    }
#else
   // Is there a way to highlight the file using xdg-open?
#endif
   if (!success)
   {
       QFileInfo file_info(file_path);
       QDesktopServices::openUrl(QUrl::fromLocalFile(file_info.dir().absolutePath()));
   }
}
