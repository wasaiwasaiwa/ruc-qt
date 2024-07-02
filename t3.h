#ifndef T3_H
#define T3_H
#include <unordered_map>
#include <QString>

using Table = std::unordered_map<QString,std::unordered_map<QString,std::streamoff>>;//定义一个无序映射，两层；


namespace T3 {
    extern Table table;//
}

void index2memory();


#endif // T3_H
