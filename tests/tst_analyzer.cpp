#include <QtTest>
#include <opencv2/opencv.hpp>
#include "Analyzer.h"

class TestAnalyzer:public QObject
{
    Q_OBJECT
private slots:
    void testCountThreeCircles(){
        // ① 造图:黑底 + 3 个已知半径的白圆
        cv::Mat img = cv::Mat::zeros(400, 400, CV_8UC3);
        cv::circle(img, cv::Point(80, 80),   20, cv::Scalar(255,255,255), -1);
        cv::circle(img, cv::Point(200, 200), 30, cv::Scalar(255,255,255), -1);
        cv::circle(img, cv::Point(320, 320), 40, cv::Scalar(255,255,255), -1);

        // ② 分析(ratio=1 → 1像素=1微米,方便按像素面积断言)
        Analyzer ana;
        AnalyzeParams p;
        statistics ss = ana.analyze(img, 1.0, p);

        // ③ 断言
        QCOMPARE(ss.count, 3);                                   // 颗粒数 = 3
        QVERIFY(ss.minA > 1000 && ss.minA < 1500);               // 最小面积 ≈ π·20² ≈ 1256
        QVERIFY(ss.maxA > 4500 && ss.maxA < 5500);               // 最大面积 ≈ π·40² ≈ 5027
        QVERIFY(ss.d10 <= ss.d50 && ss.d50 <= ss.d90);           // 分布有序(v15 的规则)
    }
};

QTEST_MAIN(TestAnalyzer)
#include "tst_analyzer.moc"