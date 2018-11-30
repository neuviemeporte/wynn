#ifndef LOG_HPP
#define LOG_HPP

#include <iostream>
#include <QString>

#define LOG(x) std::cout << x << std::endl;

inline std::ostream& operator<<(std::ostream &os, const QString &str) { return os << str.toStdString(); }

#endif // LOG_HPP
