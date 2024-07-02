#ifndef T1_H
#define T1_H
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QDebug>
#include <algorithm>
#include <thread>
#include <queue>
#include <string>
#include <fstream>
#include <iostream>

namespace T1 {
    extern QVector<QString> files;
}

void splitFile(QString& filename,size_t numLines);
void sortFile();
void mergeFile();
void deletetempFile();

#endif // T1_H
