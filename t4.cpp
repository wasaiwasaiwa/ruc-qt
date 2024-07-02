
#include "t4.h"
double calpc(const Eigen::VectorXd& x, const Eigen::VectorXd& y) {
    if (x.size() != y.size()) {
        return -1.01;
    }

    double mean_x = x.mean();
    double mean_y = y.mean();


    double numerator = (x.array() - mean_x).matrix().dot((y.array() - mean_y).matrix());
    double denominator = std::sqrt((x.array() - mean_x).square().sum()) * std::sqrt((y.array() - mean_y).square().sum());

    double correlation = numerator / denominator;

    return correlation;
}
