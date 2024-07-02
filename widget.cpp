 #include "widget.h"
#include "ui_widget.h"
#include <unordered_set>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle("股票分析");
    QVector<QComboBox*> coms = {ui->cbb1, ui->cbb2, ui->cbb3, ui->cbb4, ui->cbb5,
                                 ui->cbb6, ui->cbb7, ui->cbb8, ui->cbb9, ui->cbb10};
    for(auto x:coms)
        x->setEditable(true);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::setsingleitem()
{
    auto cbarry = {ui->cbb1, ui->cbb2, ui->cbb3, ui->cbb4, ui->cbb5, ui->cbb6, ui->cbb7, ui->cbb8, ui->cbb9, ui->cbb10
                    ,ui->cb1_2};

    for (const auto &pair : T3::table) {
        ui->cb1->addItem(pair.first);
        for(auto&x:cbarry)
            x->addItem(pair.first);
    }

    int index = 0;
    for (auto& x : cbarry) {
        x->setCurrentIndex(index);  // 设置当前显示项为第index个item
        index++;
    }
}


void Widget::on_pushButton_2_clicked()
{
    // Check if output.txt already exists
    QString outputFileName = "output.txt";
    if (QFile::exists(outputFileName)) {
        QMessageBox::information(this, "File Exists", "The file 'output.txt' already exists. Please perform further actions.");
        return; // Exit the function if output.txt exists
    }

    // File doesn't exist, proceed with the rest of the function
    QString f = "input1000.csv";
    splitFile(f, 400000);
    sortFile();
    mergeFile();
    deletetempFile();
}


void Widget::on_pushButton_clicked()
{
    createindexFile();
    index2memory();
    setsingleitem();
}

void Widget::on_cb1_currentTextChanged(const QString &arg1)
{
    // 清空cb2
    ui->cb2->clear();

    // 获取cb1当前选择的股票键
    QString selectedStock = arg1;

    // 检查该股票键是否存在于T3::table中
    if (T3::table.contains(selectedStock)) {
        // 获取该股票的年月信息
        const auto &yearMonthInfo = T3::table[selectedStock];

        // 将年月信息添加到cb2中
        for (const auto &pair : yearMonthInfo) {
            ui->cb2->addItem(pair.first);
        }
    } else {
        qDebug() << "Selected stock not found in table";
    }
}

//K线图
void Widget::on_pushButton_5_clicked()//pushButton_5的槽函数
{
    QString trade_code = ui->cb1->currentText();
    QString year_month = ui->cb2->currentText();
    long long offset = T3::table[trade_code][year_month];

    // 打开文件
    QFile file("output.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file";
        return;
    }

    // 将文件流定位到指定的偏移量
    if (!file.seek(offset)) {
        qDebug() << "Failed to seek to offset";
        file.close();
        return;
    }

    // 从文件中读取数据
    QTextStream in(&file);
    QVector<std::tuple<double, double, double, double,int>> mydata;

    // 读取文件直到找到特定的年月
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(',');
        if (parts[0] == trade_code && parts[1].left(6) == year_month) {
            double val1 = parts[2].toDouble();
            double val2 = parts[3].toDouble();
            double val3 = parts[4].toDouble();
            double val4 = parts[5].toDouble();
            double val5 = parts[1].right(2).toInt();
            mydata.push_back(std::make_tuple(val1, val2, val3, val4, val5));
        }
    }

    file.close();

    // Clear the plot

//热力图
void Widget::on_pushButton_3_clicked ()
{

    std::map<QString, int> stockCount; // 用来存储每个股票代码的出现次数

    // 遍历所有的QComboBox，统计每个股票代码的出现次数
    for(auto comboBox : {ui->cbb1, ui->cbb2, ui->cbb3, ui->cbb4, ui->cbb5,
                          ui->cbb6, ui->cbb7, ui->cbb8, ui->cbb9, ui->cbb10}) {
        QString stockCode = comboBox->currentText();
        stockCount[stockCode]++;
    }

    bool hasDuplicates = false;
    QString duplicateStocks;

    // 构造包含重复的股票代码的提示消息
    for(auto it = stockCount.begin(); it != stockCount.end(); ++it) {
        if(it->second > 1) {
            hasDuplicates = true;
            duplicateStocks += it->first + "\n";
        }
    }

    // 如果有重复的股票代码，则弹出警告消息框
    if(hasDuplicates) {
        QMessageBox::warning(this, "重复股票代码", "含有相同的股票代码:\n" + duplicateStocks);
        return;
    }


    QVector<std::unordered_set<QString>> sets;

    // 遍历所有股票代码并获取其日期集合
    for (auto comboBox : {ui->cbb1, ui->cbb2, ui->cbb3, ui->cbb4, ui->cbb5,
                          ui->cbb6, ui->cbb7, ui->cbb8, ui->cbb9, ui->cbb10}) {
        QString stockCode = comboBox->currentText();
        std::unordered_set<QString> currentDates;
        for (const auto& date : T3::table[stockCode]) {
            currentDates.insert(date.first);
        }
        sets.append(currentDates);
    }

    // 计算多个集合的交集
    std::unordered_set<QString> commonDates = intersectMultipleSets(sets);

    // 将共有的年月信息添加到 cbb01 中
    ui->cbb01->clear();
    for (const auto& date : commonDates) {
        ui->cbb01->addItem(date);
    }
}


//绘制
void Widget::on_pushButton_4_clicked()
{
    QVector<Eigen::VectorXd> data;

    QFile file("output.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open output.txt";
        return;
    }

    // 获取所有股票的偏移量
    QList<std::streamoff> offsets;
    QVector<QComboBox*> coms = {ui->cbb1, ui->cbb2, ui->cbb3, ui->cbb4, ui->cbb5,
                                 ui->cbb6, ui->cbb7, ui->cbb8, ui->cbb9, ui->cbb10};


    for (auto comboBox : coms) {
        // 获取偏移量
        std::streamoff offset = T3::table[comboBox->currentText()][ui->cbb01->currentText()];

        // 打开文件
        QFile file("output.txt");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "Failed to open output.txt for" << comboBox->currentText();
            continue;
        }

        // 读取数据计算隔天收益率序列
        QTextStream in(&file);
        if (file.seek(offset)) {
            QString line;
            QVector<double> closePrices;

            while (!in.atEnd()) {
                line = in.readLine();
                QStringList fields = line.split(",");
                if (fields[0] == comboBox->currentText() && fields[1].left(6) == ui->cbb01->currentText()) {
                    closePrices.append(fields[5].toDouble());
                }
                else {
                    break;
                }
            }

            // 检查是否读取到任何收盘价
            if (closePrices.isEmpty()) {
                qDebug() << "No close prices found for" << comboBox->currentText() << "in" << ui->cbb01->currentText();
                file.close();
                continue;
            }


            if(closePrices.size() == 1){
                data.append(Eigen::VectorXd(20));
            }else{
                // 计算隔天收益率
                QVector<double> returnRates;
                for (int j = 1; j < closePrices.size(); ++j) {
                    double returnRate = (closePrices[j] - closePrices[j - 1]) / closePrices[j - 1];
                    returnRates.append(returnRate);
                }

                // 检查是否计算出任何收益率
                if (returnRates.isEmpty()) {
                    qDebug() << "No return rates calculated for" << comboBox->currentText() << "in" << ui->cbb01->currentText();
                    file.close();
                    continue;
                }

                // 将临时存储的收益率序列转换为 Eigen::VectorXd 并存储到 data 中
                Eigen::VectorXd returns(returnRates.size());
                for (int j = 0; j < returnRates.size(); ++j) {
                    returns[j] = returnRates[j];
                }
                data.append(returns);
            }
        } else {
            qDebug() << "Failed to seek to offset" << offset << "for" << comboBox->currentText();
        }

        // 关闭文件
        file.close();
    }

    // 检查是否成功填充数据
    if (data.size() != coms.size()) {
        qDebug() << "Data size does not match the number of QComboBox. Data size:" << data.size();
        return;
    }

    // 计算皮尔森相关系数矩阵
    Eigen::MatrixXd correlation_matrix(coms.size(), coms.size());
    for (size_t i = 0; i < coms.size(); ++i) {
        for (size_t j = 0; j < coms.size(); ++j) {
            Eigen::VectorXd x = data[i];
            Eigen::VectorXd y = data[j];
            double correlation = calpc(x, y);
            correlation_matrix(i, j) = correlation;
            correlation_matrix(j, i) = correlation; // 对称矩阵
        }
    }

    ui->custom_plot_2->clearItems();
    ui->custom_plot_2->clearGraphs();
    ui->custom_plot_2->clearPlottables();
    ui->custom_plot_2->plotLayout()->clear();
    ui->custom_plot_2->replot();

    QCPAxisRect *axis_rect = new QCPAxisRect(ui->custom_plot_2);
    ui->custom_plot_2->plotLayout()->addElement(0, 0, axis_rect);
    QCPColorMap *color_map = new QCPColorMap(axis_rect->axis(QCPAxis::atBottom), axis_rect->axis(QCPAxis::atLeft));
    QSharedPointer<QCPAxisTickerText> text_ticker(new QCPAxisTickerText);
    int k = 10;
    color_map->data()->setSize(k, k);
    color_map->data()->setRange(QCPRange(0, k-1), QCPRange(0, k-1));


    for (int i=0; i<k; i++) {
        for (int j=0; j<k; j++) {
            double correlation = correlation_matrix(i,j);
            color_map->data()->setCell(i, j, correlation);
            QCPItemText *textLabel = new QCPItemText(ui->custom_plot_2);
            if (correlation > 0) textLabel->setColor(Qt::white);
            else textLabel->setColor(Qt::black);
            textLabel->setPositionAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            textLabel->position->setAxes(axis_rect->axis(QCPAxis::atBottom), axis_rect->axis(QCPAxis::atLeft));
            textLabel->position->setAxisRect(axis_rect);
            textLabel->setClipToAxisRect(true);
            textLabel->position->setCoords(i, j);
            textLabel->setText(QString::number(correlation));
        }
        text_ticker->addTick(i, QString::fromStdString(std::to_string(i)));
    }

    axis_rect->axis(QCPAxis::atLeft)->setTicker(text_ticker);
    axis_rect->axis(QCPAxis::atBottom)->setTicker(text_ticker);
    axis_rect->axis(QCPAxis::atBottom)->setTickLabelRotation(15);
    axis_rect->axis(QCPAxis::atLeft)->setTickLength(0);
    axis_rect->axis(QCPAxis::atBottom)->setTickLength(0);
    axis_rect->axis(QCPAxis::atLeft)->grid()->setPen(Qt::NoPen);
    axis_rect->axis(QCPAxis::atBottom)->grid()->setPen(Qt::NoPen);
    axis_rect->axis(QCPAxis::atLeft)->grid()->setZeroLinePen(Qt::NoPen);
    axis_rect->axis(QCPAxis::atBottom)->grid()->setZeroLinePen(Qt::NoPen);
    axis_rect->axis(QCPAxis::atLeft)->setRange(-0.5, k-0.5);
    axis_rect->axis(QCPAxis::atBottom)->setRange(-0.5, k-0.5);

    QCPColorScale *color_scale = new QCPColorScale(ui->custom_plot_2);
    ui->custom_plot_2->plotLayout()->addElement(0, 1, color_scale);
    color_scale->setType(QCPAxis::atRight);
    color_scale->setDataRange(QCPRange(-1.0, 1.0));
    color_map->setColorScale(color_scale);
    QCPColorGradient gradient;
    gradient.setColorStopAt(0.0, QColor("#ffffd0"));
    gradient.setColorStopAt(0.5, QColor("#3eb6c5"));
    gradient.setColorStopAt(1.0, QColor("#042060"));
    color_map->setGradient(gradient);
    color_map->setInterpolate(false);
    ui->custom_plot_2->replot();
}


void Widget::on_cb1_2_currentTextChanged(const QString &arg1)
{
    // 清空cb2
    ui->cb2_2->clear();

    // 获取cb1当前选择的股票键
    QString selectedStock = arg1;

    // 检查该股票键是否存在于T3::table中
    if (T3::table.contains(selectedStock)) {
        // 获取该股票的年月信息
        const auto &yearMonthInfo = T3::table[selectedStock];

        // 将年月信息添加到cb2中
        for (const auto &pair : yearMonthInfo) {
            ui->cb2_2->addItem(pair.first);
        }
    } else {
        qDebug() << "Selected stock not found in table";
    }
}


void Widget::on_pushButton_6_clicked()
{


    ui->custom_plot_3->clearItems();
    ui->custom_plot_3->clearGraphs();
    ui->custom_plot_3->clearPlottables();
    ui->custom_plot_3->plotLayout()->clear();
    ui->custom_plot_3->replot();
    QCPAxisRect *last_axis_rect = new QCPAxisRect(ui->custom_plot_3), *next_axis_rect = new QCPAxisRect(ui->custom_plot_3);
    ui->custom_plot_3->plotLayout()->addElement(0, 0, last_axis_rect);
    ui->custom_plot_3->plotLayout()->addElement(0, 1, next_axis_rect);
    QCPGraph *last_line_plot = new QCPGraph(last_axis_rect->axis(QCPAxis::atBottom), last_axis_rect->axis(QCPAxis::atLeft));
    QCPGraph *next_target_line_plot = new QCPGraph(next_axis_rect->axis(QCPAxis::atBottom), next_axis_rect->axis(QCPAxis::atLeft));
    QCPGraph *next_predict_line_plot = new QCPGraph(next_axis_rect->axis(QCPAxis::atBottom), next_axis_rect->axis(QCPAxis::atLeft));
    last_line_plot->setPen(QPen(Qt::red));
    next_target_line_plot->setPen(QPen(Qt::red));
    next_predict_line_plot->setPen(QPen(Qt::green));

    QDateTime currentDateTime = QDateTime::currentDateTime();
    qint64 seed = currentDateTime.toMSecsSinceEpoch();
    QRandomGenerator generator(seed);
    for (int i=1; i<=31; i++) {
        last_line_plot->addData(i, generator.generateDouble()*10);
        next_target_line_plot->addData(i, generator.generateDouble()*10);
        next_predict_line_plot->addData(i, generator.generateDouble()*10);
    }

    for(QCPAxis *axis : last_axis_rect->axes()) {
        axis->setLayer("axes");
        axis->grid()->setLayer("grid");
    }
    for(QCPAxis *axis : next_axis_rect->axes()) {
        axis->setLayer("axes");
        axis->grid()->setLayer("grid");
    }
    ui->custom_plot_3->rescaleAxes();
    last_axis_rect->axis(QCPAxis::atBottom)->setRange(0, 32);
    next_axis_rect->axis(QCPAxis::atBottom)->setRange(0, 32);

    ui->custom_plot_3->replot();
}
}

