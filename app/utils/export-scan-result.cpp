#include "export-scan-result.h"

#include "../model/scanner-result-item.h"

ExportScanResult::ExportScanResult(QString path, QObject *parent)
    : QObject{parent}, mExportPath(path)
{
    mOutStream.open(path.toUtf8().toStdString(), ios::out | ios::trunc);

    mOutStream << "任务名," << "文件名称," << "处理方式," << "文件扫描时间," << "文件修改时间" << endl;
}

ExportScanResult::~ExportScanResult()
{
    mOutStream.close();
}

ofstream &ExportScanResult::write(ScannerResultItem& it)
{
    mOutStream << it.getTaskName().toUtf8().toStdString() << ","
               << it.getFileName().toUtf8().toStdString() << ","
               << it.getStatus().toUtf8().toStdString() << ","
               << it.getFileCreateTime().toUtf8().toStdString() << ","
               << it.getFileModifyTime().toUtf8().toStdString() << endl;

    return mOutStream;
}
