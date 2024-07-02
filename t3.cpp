#include "t3.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

Table T3::table;

void index2memory() {

    QFile indexFile("index.txt");
    if (!indexFile.exists()) {
        qDebug() << "index.txt does not exist. Please run the previous steps first.";
        return;
    }


    if (!indexFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open index.txt";
        return;
    }

    QTextStream input(&indexFile);
    while (!input.atEnd()) {
        QString line = input.readLine();
        QStringList parts = line.split(',');

        if (parts.size() != 3) {
            qDebug() << "Invalid line format in index.txt:" << line;
            continue;
        }

        QString ts_code = parts.at(0);
        QString year_month = parts.at(1);
        std::streamoff offset = parts.at(2).toLongLong();


        T3::table[ts_code][year_month] = offset;//储存文件偏移量
    }

    indexFile.close();
    qDebug() << "Loaded index.txt into memory table done.";
}

