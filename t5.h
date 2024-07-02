#ifndef T5_H
#define T5_H

#include <Eigen/Dense>
#include <QVector>
#include <QFile>
#include <QTextStream>
#include <QtDebug>
#include <cmath>

// 读取指定月份的收盘价数据
QVector<double> readPrices(const QString &stockCode, const QString &month);

// 简单线性回归预测
double predictNextMonthPrice(const QVector<double> &prices);

// 计算均方误差
double calculateRMSE(const QVector<double> &actualPrices, double predictedPrice);


#endif // T5_H
