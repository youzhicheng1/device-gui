#include "Analyzer.h"


statistics Analyzer::analyze(const cv::Mat &img, double ratio,const AnalyzeParams& params) const
{
    statistics ss;
    //灰度图
    cv::Mat gray;
    cv::cvtColor(img,gray,cv::COLOR_BGR2GRAY);
    //二值图
    cv::Mat binary;
    cv::threshold(gray,binary,params.threshold,255,cv::THRESH_BINARY);
    if(params.morphType!=0){
        cv::Mat kernel=cv::getStructuringElement(cv::MORPH_RECT,cv::Size(params.kernelSize,params.kernelSize));
        cv::Mat clean;
        int op=(params.morphType==1)?cv::MORPH_OPEN:cv::MORPH_CLOSE;
        cv::morphologyEx(binary,clean,op,kernel);
        binary=clean;
    }
    //寻找轮廓
    cv::findContours(binary,ss.contours,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_SIMPLE);

    //筛选颗粒
    std::vector<double> areas;
    std::vector<double> diameters;
    std::vector<double> circularities;
    for(const auto &c:ss.contours){
        double area=cv::contourArea(c);
        if(area<params.minArea) continue;
        areas.push_back(area);
        //直径
        double d=2*std::sqrt(area/CV_PI);
        //圆度
        double p=cv::arcLength(c,true);
        double circ=4*CV_PI*area/(p*p);

        diameters.push_back(d);
        circularities.push_back(circ);
    }

    //统计
    ////颗粒数
    ss.count=areas.empty()?0:areas.size();
    ////面积
    double sum=0;
    ss.maxA=0,ss.minA=1e9;
    for(double a:areas){
        sum+=a;
        ss.maxA=std::max(ss.maxA,a);
        ss.minA=std::min(ss.minA,a);
    }
    ss.avgA=areas.empty()?0:(sum/areas.size());

    ////直径
    double sumD=0;
    ss.maxD=0,ss.minD=1e9;
    for(double d:diameters){
        sumD+=d;
        ss.maxD=std::max(ss.maxD,d);
        ss.minD=std::min(ss.minD,d);
    }
    ss.avgD=diameters.empty()?0:(sumD/diameters.size());
    ss.realD=ss.avgD*ratio;

    ////圆度
    double sumC=0;
    for(double C:circularities){
        sumC+=C;
    }
    ss.avgR=circularities.empty()?0:(sumC/circularities.size());

    //粒径分布
    std::sort(diameters.begin(),diameters.end());
    std::vector<double> distribution;//粒径分布
    std::vector<double> micron;//微米数组
    int N=diameters.size();
    for(auto p:diameters) micron.push_back(p*ratio);

    auto percentile=[&](double p)->double{
        if(micron.empty()) return 0.0;
        if(micron.size()==1) return micron[0];

        double position=p/100.0*(N-1);
        int idx=(int)position;
        double frac=position-idx;
        if(idx>=N-1) return micron[N-1];
        return micron[idx]+frac*(micron[idx+1]-micron[idx]);
    };

    for(int i=0;i<=100;i+=5){
        ss.curve.push_back(percentile(i));
    }
    ss.d10=percentile(10);
    ss.d50=percentile(50);
    ss.d90=percentile(90);
    ss.span = (ss.d50 > 0) ? (ss.d90 - ss.d10) / ss.d50 : 0.0;

    return ss;
}
