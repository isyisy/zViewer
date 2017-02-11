#include "CCDataLoader.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QThread>

static int RandomIndex = 0;

CCDataLoader::CCDataLoader()
{
	qRegisterMetaType <QList<QUrl>>("QList<QUrl>");
	qRegisterMetaType <QList<QString>>("QList<QString>");
}

CCDataLoader::~CCDataLoader()
{

}

void CCDataLoader::loadDataFromUrls(QList<QUrl> urls)
{
	QList<QUrl>::iterator it = urls.begin();
	for (it; it != urls.end(); ++it)
	{
		QString name = (*it).toString();
		name = name.replace(0, 8, "");
		QFileInfo fileInfo(name);
		if (fileInfo.isFile())
			OpenDicomFile(name.toStdString().c_str());
		else if (fileInfo.isDir())
			OpenDicomDir(name.toStdString().c_str());
	}
}

void CCDataLoader::loadDataFromUrls2(QList<QString> list)
{
	QList<QString>::iterator it = list.begin();
	for (it; it != list.end(); ++it)
	{
		QString name = (*it);
		QFileInfo fileInfo(name);
		if (fileInfo.isFile())
			OpenDicomFile(name.toLocal8Bit().data());
		else if (fileInfo.isDir())
			OpenDicomDir(name.toLocal8Bit().data());
	}
}

void CCDataLoader::OpenDicomFile(const char* filePath)
{
	if (filePath == NULL)
		return;
	QString filename(filePath);
	QFileInfo fileInfo(filename);
	QString suffix = fileInfo.suffix();
	if ((!QString::compare(suffix, QString("dcm"), Qt::CaseInsensitive) == 0) &&
		(!QString::compare(suffix, QString(""), Qt::CaseInsensitive) == 0))
		return;

	CCDicomUtils* pDicomUtils = new CCDicomUtils(this);
	pDicomUtils->readDicomFile(filePath);

	emit NewDicomUtils(pDicomUtils);//发送创建UI窗口信号
	Sleep(300);//暂时不做同步，等待200ms
	emit UpdateLoadProgress(pDicomUtils->getIndex(),0,1,1);//发送更新进度信号
	emit FinishLoading(pDicomUtils->getIndex());//发送加载完毕信号
	pDicomUtils->setIndex();//重置序号
}

void CCDataLoader::OpenDicomDir(const char* dirPath)
{
	if (NULL == dirPath)
		return;
	QStringList fileNames;
	QString dir(dirPath);//直接选择文件夹
	QDir directory(dir);
	directory.setFilter(QDir::Files | QDir::NoSymLinks);
	directory.setSorting(QDir::Name);
	QFileInfoList list = directory.entryInfoList();
	int fielCount = list.count();

	//两种不同情况，同一个序列内的图像大小可能不同，DR不同于CT切片数据
	if (fielCount > 10)//DR\cR
	{
		for (int i = 0; i < fielCount; i++)
		{
			QFileInfo file_info = list.at(i);
			QString suffix = file_info.suffix();
			if (QString::compare(suffix, QString("dcm"), Qt::CaseInsensitive) == 0)
			{
				QString absolute_file_path = file_info.absoluteFilePath();
				fileNames.append(absolute_file_path);
			}
		}
		if (fileNames.empty())
			return;
		importImages(fileNames);
	}
	else//CT\MR等序列
	{
		for (int i = 0; i < fielCount; i++)
		{
			QFileInfo file_info = list.at(i);
			QString absolute_file_path = file_info.absoluteFilePath();
			if (!dicom_check(absolute_file_path.toLocal8Bit().data()))
				continue;
			OpenDicomFile(absolute_file_path.toLocal8Bit().data());
		}
	}
	
}

bool CCDataLoader::dicom_check(const char* path)
{
	FILE* f = NULL;
	char flag[4];
	if (f = fopen(path, "r"))
	{
		fseek(f, 128, 0);
		char* temp = fgets(flag, 5, f);
		QString str(temp);
		fclose(f);
		if (str == "DICM")
			return true;
		else
			return false;
	}
	return false;
}

void CCDataLoader::importImages(QStringList fileList)
{
	int fileCount = fileList.count();
	CCDicomUtils* pDicomUtils = new CCDicomUtils(this);
	pDicomUtils->readDicomFile(fileList[0].toLocal8Bit().data(), fileCount);
	emit NewDicomUtils(pDicomUtils);//发送创建UI窗口信号
	Sleep(300);//暂时不做同步，等待200ms
	for (int i = 1; i < fileCount; i++)
	{
		pDicomUtils->readSlice(fileList[i].toLocal8Bit().data());
		//emit UpdateLoadProgress(pDicomUtils->seriesInstanceId(), 0, fileCount, i+1);//发送更新进度信号
		emit UpdateLoadProgress(pDicomUtils->getIndex(), 0, fileCount, i + 1);//发送更新进度信号
	}
	emit UpdateLoadProgress(pDicomUtils->getIndex(), 0, fileCount, fileCount);
	//emit FinishLoading(pDicomUtils->seriesInstanceId());//发送加载完毕信号
	emit FinishLoading(pDicomUtils->getIndex());//发送加载完毕信号
	pDicomUtils->setIndex();//重置序号
}