#include "backend.h"

#include "opencv2/opencv.hpp"
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

#include <string>
#include <fstream>
#include <QPixmap>
#include <QMessageBox>
using cv::Mat;
using namespace bc;

bool Backend::useOneBar = false;
bool Backend::useRgbBar = false;
bool Backend::saveTrainBars = true;
using std::vector;


bool Backend::getReady() const
{
    return imgLoaded;
}


void Backend::addImgToTable(int id,const QString path)
{
    //imageObj->setProperty("source", "data:image/png;base64,iVBORw0KGgo
    QMetaObject::invokeMethod(rootWindow, "addSubImg",
                              Q_ARG(QVariant,QVariant::fromValue(id)),
                              Q_ARG(QVariant,QVariant::fromValue(path)));
}

int Backend::getTypeLen(int off)
{
    return MAX(MAX(source.rows, source.cols) / 500 + off,1);
}

//===============================JSON================================

std::vector<barstruct> Backend::getBarStr()
{
    std::vector<barstruct> vec;
    vec.push_back(bc::barstruct(bc::ProcType::f0t255,useRgbBar?bc::ColorType::rgb: bc::ColorType::gray,bc::ComponentType::Hole));
    vec.push_back(bc::barstruct(bc::ProcType::f255t0,useRgbBar?bc::ColorType::rgb: bc::ColorType::gray,bc::ComponentType::Hole));
    return vec;
}

bool Backend::parceJson(QString filePath, QString prjPath)
{
    QFile file;
    file.setFileName(filePath);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject docObj = doc.object();
    file.close();

    QJsonObject json;
    QJsonArray jsSelected = docObj["selected"].toArray();
    len = docObj["len"].toInt(-1);
    prjProcessed = docObj["ready"].toBool(false);
    this->compired = docObj["compired"].toBool(false);

    displayAllFinded = docObj["displayAll"].toBool(true) ;
    displayResults = docObj["displayResults"].toBool(true);
    minPorogOfBarcodeShoject = uchar(docObj["porogSmejn"].toInt(100));
    minPorogOfBarcodeLineLen= uchar(docObj["porogSmejnLinelen"].toInt(10));
    minS = docObj["minS"].toInt(800);
    maxS = docObj["maxS"].toInt(10000);
    useOneBar = docObj["useOneBar"].toBool(false);
    useRgbBar = docObj["useRgbBar"].toBool(false);

    foreach (const QJsonValue &value, jsSelected) {
        QJsonObject obj = value.toObject();
        int objId = obj["id"].toInt();
        uchar fs = obj["foneStart"].toInt(5);
        uchar fe = obj["foneEnd"].toInt(5);
        uchar mid = obj["mid"].toInt(128);
        bool isauto = obj["isAuto"].toBool(true);
        uchar fid = obj["funcID"].toInt(0);


        if (objId >= 0)
            continue;
        SelMat *sm = new SelMat(prjPath + "/loaded/" + QString::number(objId) + ".png", objId, fs, fe);
        sm->initUI(mid, isauto, (ProcessType) fid);
        QString name = SelMat::getBarcodeObjName(useOneBar, useRgbBar);
        QJsonObject bar = obj[name].toObject();
//        sm->mBarcontainer = SelMat::getBarFromJson(bar, useOneBar, useRgbBar);


        selectedImgs.insert(objId,sm);
        //addImgToTable(objId,sm.toBase64(false));
    }
    jsSelected = docObj["avgs"].toArray();
    if(jsSelected.count()>0)
    {
        SelMat::MIsumNlenAVG =jsSelected[0].toInt();
        SelMat::MIhistAVG = jsSelected[1].toInt();
    }
    return true;
}

bool Backend::createJson(QString path)
{
    QJsonDocument doc;
    QJsonObject json;
    QJsonArray selected;

    json["version"] = this->ver;

    json["useOneBar"] = useOneBar;
    json["useRgbBar"] = useRgbBar;

    json["selected"] = selected;
    json["len"] = len;//-1
    json["ready"] = false;
    json["compired"] = false;
    json["avgs"] = "[]";
    json["displayAll"] = false;
    json["displayResults"] = false;
    json["procSmejn"] = 100;
    json["avgs"] = "[]";
    json["minS"] = 1000;
    json["maxS"] = 6000;
    doc.setObject(json);
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    file.write(doc.toJson());
    file.close();
    return true;
}

void Backend::save()
{
    if (!this->imgLoaded)
        return;
    QJsonDocument doc;
    QJsonObject json;

    json["version"] = this->ver;
    json["useOneBar"] = useOneBar;
    json["useRgbBar"] = useRgbBar;

    json["len"] = len;
    json["ready"] = prjProcessed;
    json["compired"] = compired;
    json["minS"] = minS;
    json["maxS"] = maxS;

    json["porogSmejn"] = minPorogOfBarcodeShoject;
    json["porogSmejnLinelen"] = minPorogOfBarcodeLineLen;

    QJsonArray selected;
    foreach (SelMat* value, selectedImgs) {
        selected.push_front(value->toJsonObject(useOneBar,useRgbBar));
    }
    json["selected"] = selected;
    json["avgs"] = QJsonArray({SelMat::MIsumNlenAVG, SelMat::MIhistAVG});
    doc.setObject(json);


    QFile file(prjPath + "/" + prjName);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

//===============================================================

QPixmap Backend::getSource()
{
    return QPixmap::fromImage(SelMat::matToQImage(source, false));
}

void createFile(QString path)
{
    QFile file(path);
    file.open(QIODevice::WriteOnly);
    file.close();
}

int Backend::createProject(QString prjPath, QString prjName, QString imgPath)
{
    QDir dir;
    QFile file;
    if(!dir.exists(prjPath))
        return 2;//Путь некорретный


    QString fullPath = prjPath + '/' + prjName;
    if (dir.exists(fullPath))
        return 1; //Уже существует такой проект

    if (imgPath != "")
        if (SelMat::imread(imgPath).rows<=0)
            return 3;//Нет изображения или оно некорректно


    if (!dir.mkpath(fullPath))
        return -1;

    fullPath += "/";

//    //parts
//    if(!dir.mkpath(fullPath + "parts"))
//        return -1;

    //loaded
    if(!dir.mkpath(fullPath + "loaded"))
        return -1;


    //project file
    if (!createJson(fullPath + prjName + ".bc"))
        return -1;

    imgLoaded = false;
    prjProcessed = false;
    imgLoaded = false;
    minPorogOfBarcodeLineLen = 10;
    minPorogOfBarcodeShoject = 100;
    minS = 800;
    maxS = 7000;
    this->prjName = prjName;
    this->prjPath = fullPath;
    displayAllFinded = true;
    displayResults = true;

    if (imgPath.length() != 0)
        if (loadMainImg(imgPath))
            imgLoaded = true;

    return 0;
    //
}

bool Backend::openProject(QString prjFilePath)
{
    qDebug() << prjFilePath;
    QDir dir(prjFilePath);
    QFile file(prjFilePath);
    if (!dir.exists(prjFilePath))
        return false; //Путь некорретный

    QString path = dir.cleanPath(prjFilePath);
    int pos = path.lastIndexOf('/');

    this->selectedImgs.clear();
    this->findedImgRects.clear();

    //to works
    path.remove(pos, path.length() - pos);
    if (!parceJson(prjFilePath+"", path))
        return false;

    imgLoaded = (file.exists(path + "/source.png"));
    if (imgLoaded) {
        source = SelMat::imread(path + "/source.png");
        qDebug() << source.channels();
    }

    this->prjName = prjFilePath.remove(0, path.length()+1);
    this->prjPath = path;



    //loaded
    if (!dir.exists(path + "/loaded"))
        dir.mkpath(path + "/loaded");
//    if (file.exists(path + "/marcedImg.png"))
//        this->marcedImg = SelMat::imread(path + "/marcedImg.png");
    //project file
    if(imgLoaded)
    {
        prjProcessed = false;
        if (file.exists(prjPath + "/rects.json") && file.exists(prjPath + "/BarcontainerSL.json")) {
            prjProcessed = true;
            QFile file;
            file.setFileName(prjPath + "/rects.json");
            file.open(QIODevice::ReadOnly | QIODevice::Text);
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            QJsonArray finded = doc.array();
            for (int i = 0, total = finded.size(); i < total; ++i) {
                auto arr = finded[i].toArray();
                findedImgRects.push_back(cv::Rect(arr[0].toInt(0), arr[1].toInt(0), arr[2].toInt(0), arr[3].toInt(0)));
            }


            file.setFileName(prjPath + "/BarcontainerSL.json");
            file.open(QIODevice::ReadOnly | QIODevice::Text);
            doc = QJsonDocument::fromJson(file.readAll());
            QJsonArray bars = doc.array();
            if(useOneBar)
            {
                for (int i = 0, total = bars.size(); i < total; ++i) {
                    Barcontainer *bc = new Barcontainer();
                    Baritem* b=new Baritem();
                    bc->addItem(b);

                    auto arr = bars[i].toArray();
                    for (int j = 0, total = arr.size(); j<total; j+=2)
                        b->add(arr[j].toInt(0),arr[j+1].toInt(0));

                    findedImgBars.push_back(b);
                }
            } else
            {
                for (int i = 0, total = bars.size(); i < total; i+=2) {
                    Barcontainer *bc = new Barcontainer();
                    Baritem* b=new Baritem();
                    bc->addItem(b);
                    auto arr = bars[i].toArray();
                    for (int j = 0, total = arr.size(); j<total; j+=2)
                        b->add(arr[j].toInt(0),arr[j+1].toInt(0));

                    b=new Baritem();
                    bc->addItem(b);

                    arr = bars[i+1].toArray();
                    for (int j = 0, total = arr.size(); j<total; j+=2)
                        b->add(arr[j].toInt(0),arr[j+1].toInt(0));
                    findedImgBars.push_back(bc);
                }
            }
        }
        if (file.exists(prjPath + "/procs.json")) {
            QFile file;
            file.setFileName(prjPath + "/procs.json");
            file.open(QIODevice::ReadOnly | QIODevice::Text);
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            QJsonArray bars = doc.array();
            for (int i = 0, total = bars.size(); i < total; ++i) {
                findedImgProc.push_back(bars[i].toInt(0));
            }
        }
    } else {
        if (file.exists(prjPath + "/procs.json"))
            file.remove(prjPath + "/procs.json");

        if (file.exists(prjPath + "/BarcontainerSL.json"))
            file.remove(prjPath + "/BarcontainerSL.json");

    }
    //
    return true;
}

bool Backend::loadMainImg(QString imgPath)
{
    Mat img = SelMat::imread(imgPath);

//    cv::Mat img = cv::imread(imgPath.toStdString(), cv::IMREAD_COLOR);
    source = img.clone();
    SelMat::imwrite(prjPath + "/source.png", img,false);
    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
    SelMat::MIsumNlenAVG = SelMat::findSumdivAVG(img);
    SelMat::MIhistAVG = SelMat::findHistAVG(img);
    this->imgLoaded = true;
    this->prjProcessed = false;
    this->compired = false;
    this->findedImgBars.clear();
    this->findedImgRects.clear();
    this->findedImgProc.clear();

    QJsonDocument doc;
    doc.setArray(QJsonArray());
    QFile file(prjPath + "/rects.json");
    file.open(QIODevice::WriteOnly);
    file.write(doc.toJson());
    file.close();
    file.setFileName(prjPath + "/BarcontainerSL.json");
    file.open(QIODevice::WriteOnly);
    file.write(doc.toJson());
    file.close();

    file.setFileName("marcedImg.png");
    file.remove();
    file.close();

    file.setFileName("result.png");
    file.remove();
    file.close();
    //marcedImg.png
    //result.png
//    this->selectedImgs.clear();
    //QFile::copy(imgPath, prjPath + "source.png");
    return true;
}

void Backend::process() {}

int Backend::loadSImgFormDisk(QString path)
{
    cv::Mat img = SelMat::imread(path);
    QString simg_path = prjPath + "/loaded/" + QString::number(len) + ".png";
    SelMat::imwrite(simg_path, img,false);
//    m.createBarcontainer();
    selectedImgs.insert(len,new SelMat(img, len));
    int id = len;
    --len;
    return id;
}

void Backend::removeSelectedImg(int index)
{
    selectedImgs.remove(index);
    QFile file(prjPath + "/loaded/" + QString::number(index) + ".png");
    file.remove();
    save();
}

QString Backend::getFindedImgs(int id)
{
    return SelMat::matToBase64(this->source(findedImgRects[id]), false);
}

QImage Backend::getSelectedImg(int id)
{
    return this->selectedImgs[id]->toQImage();
}

QString Backend::getImg(int id,bool needAlpha)
{
    //"data:image/png;base64," + model.imageData
    if (id < 0)
        return selectedImgs[id]->toBase64(needAlpha);
    else
        return SelMat::matToBase64(this->source(findedImgRects[id]), needAlpha);
}

QStringList Backend::getParts()
{
    QStringList list;

    for (size_t i = 0, total = MIN(findedImgRects.size(),500); i < total; ++i) {
        cv::Mat img = source(findedImgRects[i]);
        //float ad = 500.0/wd;
        //cv::resizeWindow(name, 500, hi * ad);
        if (img.cols > 150) {
            float ad = 150.0f / img.cols;
            cv::resize(img,img,cv::Size(150, img.rows * ad));
        }
        if (img.rows > 150) {
            float ad = 150.0f / img.rows;
            cv::resize(img,img,cv::Size(img.cols*ad, img.cols));
        }
        list.append(QString::number(i) + ":" + SelMat::matToBase64(img, false));
    }
    return list;
}

QStringList Backend::getAllSelected() {
    QStringList list;

    auto keys = selectedImgs.keys();
    std::sort(keys.begin(), keys.end());
    for (auto key : keys) {
        list.append(QString::number(key) + ":" + selectedImgs[key]->toBase64(false));
    }
    return list;
}

bool Backend::checkExist(int id)
{
    return selectedImgs.contains(id);
}

int Backend::addPartToSelects(int id)
{
    if ((int) findedImgRects.size() > id) {
        int nid = len;
        Mat img = source(findedImgRects[id]);

        auto sm =new SelMat(img, nid);
        selectedImgs.insert(nid, sm);
        SelMat::imwrite(prjPath + "/loaded/" +QString::number(len) + ".png",img,false);
        --len;
        return nid;
    } else
        return 0;
}


//========================================ProcessMainImg==============================================================

std::vector<std::vector<cv::Point>> Backend::myDetectInv(ProcessType avgType,ProcessType funType, uchar porog,bool skipWhite,bool skipBlack, int *avg)
{
    Mat gray;
    cv::cvtColor(source, gray, cv::COLOR_BGR2GRAY);
    int hei = source.rows;
    int wid = source.cols;
    SelMat::MIsumNlenAVG = SelMat::findSumdivAVG(gray,skipWhite,skipBlack);
    SelMat::MIhistAVG = SelMat::findHistAVG(gray,skipWhite,skipBlack);
    uchar imax = 128;
    switch (avgType) {
    case ProcessType::Hist:
        imax = SelMat::MIhistAVG;
        break;
    case ProcessType::AvgNsum:
        imax = SelMat::MIsumNlenAVG;
        break;
    case ProcessType::Manuly:
        imax = *avg;
        break;
    default:
        imax = *avg;
    }
    *avg = imax;
    Mat new_img;
    switch (funType)
    {
    case ProcessType::Manuly:
    {
        new_img = gray.clone();
        for (int j = 0; j < hei; ++j) //hei
        {
            for (int i = 0; i < wid; ++i) //wid
            {
                auto p = gray.at<uchar>((int) j, (int) i);
                //p = abs(p-imax);
                if (p > imax)
                    p = p - imax;
                else
                    p = imax - p;

                new_img.at<uchar>((int) j, (int) i) = (p < porog) ? 0 : 255;
            }
        }
        int erosion_size = 2;
        Mat el = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1), cv::Point(erosion_size, erosion_size));
        cv::dilate(new_img, new_img, el);

        //    cv::namedWindow("test", cv::WINDOW_NORMAL);
        //    cv::imshow("test", new_img);
        //    cv::waitKey(0);
        //************************************************
        for (int i = 0; i < wid; ++i) {
            new_img.at<uchar>(0, i) = 255;
            new_img.at<uchar>(hei - 1, i) = 255;
        }
        for (int i = 0; i < hei; ++i) {
            new_img.at<uchar>(i, 0) = 255;
            new_img.at<uchar>(i, wid - 1) = 255;
        }
        break;
    }
    case ProcessType::CvFunction: {
        new_img = gray.clone();

        for (int j = 1; j < hei; ++j) //hei
        {
            for (int i = 1; i < wid; ++i) //wid
            {
                short p = gray.at<uchar>((int)j, (int)i);
                short pm1 = gray.at<uchar>((int)j-1, (int)i);
                short pm2 = gray.at<uchar>((int)j, (int)i-1);
                //p = abs(p-imax);
                if (abs(p-pm1)>porog || abs(p-pm2)>porog)
                    new_img.at<uchar>((int) j, (int) i) = 255;
                else
                    new_img.at<uchar>((int) j, (int) i) = 0;
            }
        }
        break;

    }
    case ProcessType::Waterfall: {
        new_img = source.clone();

        for (int i = 0; i < source.rows; i++) {
            for (int j = 0; j < source.cols; j++) {
                if (new_img.at<cv::Vec3b>(i, j) == cv::Vec3b(255, 255, 255)) {
                    new_img.at<cv::Vec3b>(i, j)[0] = 0;
                    new_img.at<cv::Vec3b>(i, j)[1] = 0;
                    new_img.at<cv::Vec3b>(i, j)[2] = 0;
                }
            }
        }
        Mat kernel = (cv::Mat_<float>(3, 3) << 1, 1, 1, 1, -8, 1, 1, 1, 1); // an approximation of second derivative, a quite strong kernel
        Mat imgLaplacian;
        filter2D(new_img, imgLaplacian, CV_32F, kernel);
        Mat sharp;
        new_img.convertTo(sharp, CV_32F);
        Mat imgResult = sharp - imgLaplacian;
        sharp.release();
        imgLaplacian.release();
        imgResult.convertTo(imgResult, CV_8UC3);
        Mat bw;
        cvtColor(imgResult, bw, cv::COLOR_BGR2GRAY);
        imgResult.release();
        threshold(bw, bw, imax, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);//40
        Mat dist;
        distanceTransform(bw, dist, cv::DIST_L2, 3);
        bw.release();
        normalize(dist, dist, 0, 1.0, cv::NORM_MINMAX);
        threshold(dist, dist, 0.4, 1.0, cv::THRESH_BINARY);
        Mat kernel1 = Mat::ones(3, 3, CV_8U);
        dilate(dist, dist, kernel1);
        dist.convertTo(new_img, CV_8U);
        break;

    }
    case ProcessType::Canny: {
        Mat new_img;
        cv::blur(gray, new_img, cv::Size(3, 3));
        std::vector<std::vector<cv::Point>> contours;

        /// Detect edges using canny
        Canny(new_img, new_img, imax, imax * 2, 3);
        /// Find contours
        break;
    }
    case ProcessType::AvgSdvig: {
        Mat imageSegment;
        int spatialRadius = 35;
        int colorRadius = 60;
        int pyramidLevels = 3;
        cv::pyrMeanShiftFiltering(source, imageSegment, spatialRadius, colorRadius, pyramidLevels);
        cv::cvtColor(source, gray, cv::COLOR_BGR2GRAY);
        cv::threshold(gray, new_img, 0, 255, cv::ThresholdTypes::THRESH_BINARY_INV | cv::ThresholdTypes::THRESH_OTSU);
        break;
    }

//    case ProcessType::Barcode: {
//        barcodeCreator bc;

//        return bc.segmetbar(gray, false,*avg);
////        return bc.createBar(gray, false, (uchar)(*avg));
//    }
    }
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(new_img, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);  //simple - only end points
    return contours;
}


//============================================processMainImg=====================================================
void Backend::presetProcess(int minS,int maxS,bool useOne,bool useRgbBar)
{
    this->useOneBar = useOne;
    this->useRgbBar = useRgbBar;
    this->minS = minS;
    this->maxS = maxS;
}

void drawPoints(cv::Mat &image, std::vector<cv::Point> &points, cv::Vec3b color)
{
    for (const cv::Point& p : points)
    {
        image.at<cv::Vec3b>(p.y, p.x) = color;
        if(p.x<image.cols-1) image.at<cv::Vec3b>(p.y, p.x+1) = color;
        if(p.y<image.rows-1) image.at<cv::Vec3b>(p.y+1, p.x) = color;
        if(p.x>0) image.at<cv::Vec3b>(p.y, p.x-1) = color;
        if(p.y>0) image.at<cv::Vec3b>(p.y-1, p.x) = color;
    }
}

void Backend::processMainImg(bool skipWhite,bool skipBlack,int avgType,int funType,int fone,int dropFone)
{
    //(bool skipWhite,bool skipBlack,int procType,int fone,int dropFone
    //skipWhite skipBlack,avg,dropFone
    for (size_t i = 0, total = findedImgBars.size(); i < total; ++i) {
        delete findedImgBars[i];
    }
    findedImgBars.clear();
    findedImgRects.clear();
    Mat gray;
    Mat marcedImg = source.clone();
    int wid = source.cols;
    int hei = source.rows;

    //************************************************
    int avg = fone;
    std::vector<std::vector<cv::Point>> contours =
                                         myDetectInv(static_cast<ProcessType>(avgType),static_cast<ProcessType>(funType), dropFone, skipWhite, skipBlack, &avg);

    //*******************************************************
    int coun = -1;
    std::cout << contours.size() << std::endl;

    std::vector<Barcontainer> bs;

    QJsonArray jsRects;
    QJsonArray jsBars;


    gray.release();
    for (auto contur : contours) {
        cv::Rect rect = cv::boundingRect(contur);
        const int mpas = 5;
        rect.x = MAX(rect.x - mpas, 0);
        rect.y = MAX(rect.y - mpas, 0);
        rect.width = rect.width + mpas*2;
        rect.height = rect.height + mpas*2;

        int e = rect.x + rect.width - wid;
        if (e >= 0)
            rect.width -= e;

        e = rect.y + rect.height - hei;
        if (e >= 0)
            rect.height -= e;

        //*******************************************************
        if (rect.width * rect.height > maxS || rect.width * rect.height < minS) //<min(min(img.rows,img.cols)/10)
            continue;
        //*******************************************************
        ++coun;
//        if (coun != 63) continue;

        barcodeCreator coofs;
		coofs.setVisualize(false);
		coofs.setCreateBinaryMasks(false);
		coofs.setCreateBinaryMasks(false);
		coofs.setReturnType(0);
		//std::cout << coun << std::endl;
        Barcontainer *bt;
        cv::cvtColor(source(rect), gray, cv::COLOR_BGR2GRAY);
//        cv::imwrite("D:/1.png", gray);
        if (useOneBar) {
            bt = coofs.createSLbarcode(source(rect), MAX(avg - dropFone, 0), MIN(avg + dropFone, 255),useRgbBar);
        } else {
            auto st = getBarStr();
            bt = coofs.createBarcode(source(rect),st);
        }

        for (size_t i=0, total = bt->count();i< total ;++i )
            jsBars.append(SelMat::getArrayFromBar(bt->get(i)));

//        drawPoints(marcedImg, contur, cv::Vec3b(0, 255, 255));

//        cv::polylines(marcedImg, contur, true, cv::Scalar(0, 255, 255), 3,4);
        cv::rectangle(marcedImg, rect, cv::Scalar(0, 255, 255), 3);

        //coofs.detectBs(normlImg(rect), !savePath.empty());
        //*******************************************************
        findedImgBars.push_back(bt);
        findedImgRects.push_back(rect);
        jsRects.append(QJsonArray({rect.x, rect.y, rect.width, rect.height}));

        QJsonArray arr;


    }
    QJsonDocument doc;
    QJsonObject json;
    QJsonArray selected;

    doc.setArray(jsRects);
    QFile file(prjPath + "/rects.json");
    file.open(QIODevice::WriteOnly);
    file.write(doc.toJson());
    file.close();

    doc.setArray(jsBars);
    file.setFileName(prjPath + "/BarcontainerSL.json");
    file.open(QIODevice::WriteOnly);
    file.write(doc.toJson());
    file.close();

    SelMat::imwrite(prjPath + "/marcedImg.png", marcedImg, false);
    this->prjProcessed = true;
    this->compired = false;
    save();
}


//====================================Barcontainer=================================

float toFixed(float v, int len)
{
    int val = pow(10, len);
    return int(v * val + 0.5) / float(val);
}


uchar Backend::compireObjects(int id,uchar porog,bool normalize)
{
    bool selectBest = true;
    //меньше 0 не будет, т.к. mq по умолчанию - 0
    float mq = 0;
    Barbase* objBarcode = findedImgBars[id]->clone();
    objBarcode->preprocessBar(porog, normalize);
    //hils.forEach(el => mq = Math.min(Math.abs(1 - BarcontainerCompire(el, bar, 10)), mq));
    if (selectBest) {
        for (SelMat *sm : selectedImgs) {
            auto compret =bc::Barbase::compireBarcodes(sm->mBarcode, objBarcode, mainCompireFunction);
            mq = MAX(compret, mq);
        }
    } else {
        for (SelMat *sm : selectedImgs)
            mq += bc::Barbase::compireBarcodes(sm->mBarcode, objBarcode, mainCompireFunction);
        mq /= selectedImgs.size();
    }


    assert(mq <= 1.1);
    compired = true;
    return (int(roundf(mq * 100)));
}

//=================================================StartFinding========================================
void Backend::findSimmularObjects(int BarcontainerPorog,int proc,int type,bool normalize)
{
    mainCompireFunction = static_cast<CompireFunction>(type);
    minPorogOfBarcodeShoject = static_cast<uchar>(BarcontainerPorog);
    QHash<int, Barbase*> baseBar;
    for (QHash<int, SelMat*>::iterator i = selectedImgs.begin(), total = selectedImgs.end(); i != total; ++i)
    {
        Barbase* temp = i.value()->createBarcode(useOneBar, useRgbBar,true);
        baseBar.insert(i.key(),temp->clone()); //i.value() =I
        i.value()->mBarcode->preprocessBar(proc,normalize);
    }

    Mat result(source.rows, source.cols, CV_8UC4, cv::Scalar(0,0,0,0));
    QJsonArray arr;
    findedImgProc.clear();
//    QFile file2(this->prjPath + "/marcedImg.png");

    Mat marcedImg = source.clone();

    int co = 0;
    for (int i = 0, total = static_cast<int>(findedImgRects.size()); i < total; ++i)
    {
        uchar ret = compireObjects(i, proc,normalize);
        findedImgProc.push_back(ret);
        arr.append(ret);

        cv::Point p = findedImgRects[i].tl();
        p.y = MAX(0, p.y - 8);

        if (ret >= minPorogOfBarcodeShoject)
        {
            cv::rectangle(result, findedImgRects[i], cv::Scalar(0, 255, 0, 255), getTypeLen());
            if(drawProcents)
                cv::putText(
                    result, QString::number((int)ret).toStdString(),p,
                    cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 255, 0,255),getTypeLen(-1)); //надо хранить и rect и желательно процент
            ++co;
        } else
        {
            cv::rectangle(marcedImg, findedImgRects[i], cv::Scalar(0, 255, 255), getTypeLen());
            if(drawProcents)
                cv::putText(
                    marcedImg, QString::number((int)ret).toStdString(),p,
                    cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 255, 255),getTypeLen(-1)); //надо хранить и rect и желательно процент
        }
    }

    QJsonDocument doc;
    doc.setArray(arr);
    QFile file(prjPath + "/procs.json");
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(doc.toJson());
        file.close();
    }

    for (QHash<int, Barbase *>::iterator i = baseBar.begin(), total = baseBar.end(); i != total; ++i)
    {
        delete selectedImgs[i.key()]->mBarcode;
        selectedImgs[i.key()]->mBarcode =  dynamic_cast<bc::Barcontainer*>(i.value());
    }

    qDebug() << "Найдено " << co;
    SelMat::imwrite(prjPath+"/result.png", result,true);
    SelMat::imwrite(prjPath+"/marcedImg.png", marcedImg,false);
    marcedImg.release();
    result.release();
    compired = true;
    save();
}


void Backend::retryCompire(int nedBarcontainerPorog)
{
    Mat result(source.rows, source.cols, CV_8UC4, cv::Scalar(0, 0, 0, 0));
    minPorogOfBarcodeShoject = nedBarcontainerPorog;
    for (int i = 0, total = static_cast<const int>(findedImgProc.size()); i < total; ++i) {
        if (findedImgProc[i] >= minPorogOfBarcodeShoject) {
            cv::rectangle(result, findedImgRects[i], cv::Scalar(0, 255, 0, 255), 3);
            cv::Point p = findedImgRects[i].tl();
            p.y = MAX(0, p.y - 8);
            if (drawProcents)
                cv::putText(
                    result, QString::number((int)findedImgProc[i]).toStdString(),p,
                    cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 255, 0,255),getTypeLen(-1)); //надо хранить и rect и желательно процент
        }
        //надо хранить и rect и желательно процент
    }
    SelMat::imwrite(prjPath+"/result.png", result,true);
}

void Backend::exportResult(QString path,bool expAll)
{
    Mat result = source.clone();
    for (int i = 0; i < static_cast<const int>(findedImgProc.size()); ++i) {
        if (findedImgProc[i] >= minPorogOfBarcodeShoject)
        {
            cv::Point p = findedImgRects[i].tl();
            p.y = MAX(0, p.y - 8);
            cv::rectangle(result, findedImgRects[i], cv::Scalar(0, 255, 0), getTypeLen());
            if (drawProcents)
                cv::putText(
                    result, QString::number((int)findedImgProc[i]).toStdString(),p,
                    cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 255, 0), getTypeLen(-1)); //надо хранить и rect и желательно процент

        } else if(expAll)
        {
            cv::Point p = findedImgRects[i].tl();
            p.y = MAX(0, p.y - 8);
            cv::rectangle(result, findedImgRects[i], cv::Scalar(0, 255, 255), getTypeLen());
            if (drawProcents)
                cv::putText(
                    result, QString::number((int)findedImgProc[i]).toStdString(),p,
                    cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 255, 255),getTypeLen(-1)); //надо хранить и rect и желательно процент
        }
        //надо хранить и rect и желательно процент
    }
    SelMat::imwrite(path, result,false);
    QMessageBox msgBox;
    msgBox.setText("Эксперт завешен");
    msgBox.exec();
}
//==========================================================


QString Backend::getProcessedSubImg(int id)
{
    return selectedImgs[id]->processImg();
}

int Backend::getImgAvg(int id, int type)
{
    if (selectedImgs.contains(id))
        return selectedImgs[id]->getAvg((ProcessType) type);
    else
        return 0;
}


uchar Backend::getImgMid(int id)
{
    if (selectedImgs.contains(id))
        return selectedImgs[id]->mid;
    else
        return 0;
}


QJsonObject Backend::getImgInfo(int id)
{
    if (selectedImgs.contains(id)) {
        auto obj = selectedImgs[id]->toJsonObject(useOneBar,useRgbBar);
        return obj;
    } else
        return QJsonObject();
}
QJsonArray getJsobBar(Baritem* bar,int porog)
{
    ushort charts[256];
    for (int i = 0; i < 256; ++i)
        charts[i] = 0;
    QJsonArray arr;
    for (auto const *line :bar->bar) {
        if(line->len>=porog)
            for (ushort i = line->start; i < line->start + line->len; ++i)
                ++charts[i];
    }
    for (int i = 0; i < 256; ++i) {
        QJsonObject liner1;
        liner1["x"] = i;
        liner1["y"] = (int)charts[i];
        arr.append(liner1);
    }
    return arr;
}

QJsonObject Backend::getImgBar(int id,int porog)
{
    QJsonObject ret;
    if(id==0 || !selectedImgs.contains(id))
        return ret;
    Barcontainer *bcode = selectedImgs[id]->createBarcode(useOneBar, useRgbBar, false);
    for (size_t i = 0, total = bcode->count(); i < total; ++i)
    {
        QString st = QString::number(i);
        ret[st] = getJsobBar(bcode->get(i),porog);
    }

    return ret;
}

QString Backend::processImg(int id, int type, int less,int mid, int more,bool isAuto)
{
    if (selectedImgs.contains(id)) {
        if(!useOneBar)
            return selectedImgs[id]->toBase64(false);
        selectedImgs[id]->isAuto = isAuto;
        return selectedImgs[id]->processImg((ProcessType) type, less, mid, more);
    }
    else
        return "";
}

QImage Backend::tryProcessMainImage(bool skipWhite, bool skipBlack, int avgType,int funType, int fone, int dropFone)
{
    //************************************************
    int avg = fone;
    auto nv = myDetectInv(static_cast<ProcessType>(avgType), static_cast<ProcessType>(funType), dropFone, skipWhite, skipBlack, &avg);
    Mat normlImg = source.clone();
    cv::drawContours(normlImg, nv, -1, cv::Scalar(0, 255, 255));

    //std::vector<std::vector<cv::Point>> contours;
    //cv::findContours(normlImg, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE); //simple - only end points
    //cv::drawContours(rectImg, contours, -1, cv::Scalar(0, 255, 0));
    //return SelMat::matToBase64(normlImg, false);
    return SelMat::matToQImage(normlImg, false);
}


Backend::Backend(QObject *parent) : QObject(parent)
{
    imgLoaded = false;

//    BarcontainerCreator t;
//    Mat m2627 = cv::imread("D:\\1.png", cv::IMREAD_GRAYSCALE);
//    t.createTwoSlBarcontainer(m2627,false);
}

