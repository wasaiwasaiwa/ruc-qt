#include "t1.h"

QVector<QString> T1::files;

void splitFile(QString& filename, size_t numLines)
{
    QFile inf(filename);//定义一个对象 inf
    if (!inf.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: Unable to open file" << filename;//qDebug当cout用
        return;
    }

    QTextStream infs(&inf);
    infs.readLine();

    size_t chunkFileindex = 1;

    while (!infs.atEnd()) {
        QString chunkFile = "chunk" + QString::number(chunkFileindex++) + ".txt";
        QFile ouf(chunkFile);
        if (!ouf.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Error: Unable to create file" << chunkFile;
            return;
        }
        QTextStream oufs(&ouf);
        QString line;
        for (size_t i = 0; i < numLines && !infs.atEnd(); ++i) {
            line = infs.readLine();
            oufs << line << "\n";
        }
        ouf.close();

        T1::files.append(chunkFile);
        qDebug() << "Chunk" << chunkFile << "created.";
    }
}
//文件分块
void sortFile()
{

    for (const auto& filePath : T1::files) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "Error: Unable to open file" << filePath;
            continue;
        }

        QTextStream in(&file);
        QStringList lines;
        while (!in.atEnd()) {
            lines.append(in.readLine());
        }
        file.close();

        std::sort(lines.begin(), lines.end());

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            qDebug() << "Error: Unable to open file for writing" << filePath;
            continue;
        }

        QTextStream out(&file);
        for (const auto& line : lines) {
            out << line << '\n';
        }
        file.close();
        qDebug() << "File: " << filePath << "sorted done.";
    }

    qDebug() << "All files sorted successfully.";
}


void mergeFile() {

    std::cout << "File merge begin......." << '\n';

    std::priority_queue<std::pair<std::string, int>, std::vector<std::pair<std::string, int>>, std::greater<std::pair<std::string, int>>> pq;

    std::vector<std::ifstream> chunkFilePointers;

    for (const auto& chunkFile : T1::files) {
        std::ifstream inputFile(chunkFile.toStdString());
        if (!inputFile.is_open()) {
            std::cerr << "Error: Unable to open file " << chunkFile.toStdString() << std::endl;
            return;
        }
        chunkFilePointers.push_back(std::move(inputFile));
    }


    std::ofstream outputFile("output.txt");
    if (!outputFile.is_open()) {
        std::cerr << "Error: Unable to create output file." << std::endl;
        return;
    }


    for (size_t i = 0; i < chunkFilePointers.size(); ++i) {
        std::string line;
        if (std::getline(chunkFilePointers[i], line)) {
            pq.push({ line, i });
        }
    }


    while (!pq.empty()) {
        auto minPair = pq.top();
        pq.pop();
        outputFile << minPair.first << '\n';

        int chunkIndex = minPair.second;
        std::string line;
        if (std::getline(chunkFilePointers[chunkIndex], line)) {
            pq.push({ line, chunkIndex });
        }
    }


    outputFile.close();
    for (auto& inputFile : chunkFilePointers) {
        inputFile.close();
    }

    std::cout << "Files merged successfully." << std::endl;
}

void deletetempFile()
{
    for (const QString &fileName : T1::files) {
        if (QFile::exists(fileName)) {
            if (QFile::remove(fileName)) {
                qDebug() << "Deleted file:" << fileName;
            } else {
                qDebug() << "Failed to delete file:" << fileName;
            }
        } else {
            qDebug() << "File does not exist:" << fileName;
        }
    }
}
