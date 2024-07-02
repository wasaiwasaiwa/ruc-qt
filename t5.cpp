#include <Eigen/Dense>
#include <QVector>
#include <QFile>
#include <QTextStream>
#include <QtDebug>
#include <cmath>
#include "t5.h"
#include "t3.h"
// 读取指定月份的收盘价数据
QVector<double> readPrices(const QString &stockCode, const QString &month) {
    QVector<double> closePrices;
    QFile file("output.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open output.txt";
        return closePrices;
    }

    QTextStream in(&file);

    if(file.seek(T3::table[stockCode][month])){
        QString line;
        while (!in.atEnd()) {
            line = in.readLine();
            QStringList fields = line.split(",");
            if (fields[0] == stockCode && fields[1].left(6) == month) {
                closePrices.append(fields[5].toDouble());
            }else{
                break;

            }
        }
    }

    file.close();
    return closePrices;
}

// 简单线性回归预测
double predictNextMonthPrice(const QVector<double> &prices) {
    if (prices.isEmpty()) return -1.0;

    int n = prices.size();
    Eigen::VectorXd x(n), y(n);
    for (int i = 0; i < n; ++i) {
        x(i) = i + 1;
        y(i) = prices[i];
    }

    double mean_x = x.mean();
    double mean_y = y.mean();
    double numerator = (x.array() - mean_x).matrix().dot((y.array() - mean_y).matrix());
    double denominator = (x.array() - mean_x).square().sum();
    double slope = numerator / denominator;
    double intercept = mean_y - slope * mean_x;

    // 预测下一个月的价格
    double nextX = n + 1;
    return slope * nextX + intercept;
}

// 计算均方误差
double calculateRMSE(const QVector<double> &actualPrices, double predictedPrice) {
    if (actualPrices.isEmpty()) return -1.0;

    double squaredErrorSum = 0.0;
    for (double price : actualPrices) {
        squaredErrorSum += std::pow(predictedPrice - price, 2);
    }
    return std::sqrt(squaredErrorSum / actualPrices.size());
}
