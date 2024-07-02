#include "t2.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <unordered_set>
#include <QString>

void createindexFile() {
    QFile indexCheck("index.txt");
    if (indexCheck.exists()) {
        qDebug() << "index.txt already exists.";
        return;
    }
    //检查文件是否存在，如果已经存在，则不做处理。

    std::unordered_set<QString> keys;

    QFile inputFile("output.txt");
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open output.txt";
        return;
    }

    QFile indexFile("index.txt");
    if (!indexFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to open index.txt";
        inputFile.close();
        return;
    }

    QTextStream input(&inputFile);
    QTextStream output(&indexFile);
    std::streamoff offset = 0;
    while (!input.atEnd()) {
        QString line = inputFile.readLine();
        QStringList parts = line.split(',');//分部分
        QString ts_code = parts.at(0);//第一部分
        QString year_month = parts.at(1).left(6);

        if(!keys.contains(ts_code + year_month)){
            keys.insert(ts_code + year_month);
            output << ts_code << ',' << year_month << ',' << offset << '\n';
        }


        offset = inputFile.pos();
    }

    inputFile.close();
    indexFile.close();
    qDebug() << "create index.txt done";
}
