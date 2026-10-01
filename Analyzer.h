#ifndef ANALYZER_H
#define ANALYZER_H
#include <opencv2/opencv.hpp>
#include <QMetaType>

typedef struct statistics{
    int count;//颗粒个数

    double avgD,maxD,minD,realD;//直径

    double avgR;//圆度

    double avgA,maxA,minA;//面积

    std::vector<std::vector<cv::Point>> contours;//轮廓
}statistics;

Q_DECLARE_METATYPE(statistics)

class Analyzer{
public:
    statistics analyze(const cv::Mat &img,double ratio) const;

};



#endif // ANALYZER_H
