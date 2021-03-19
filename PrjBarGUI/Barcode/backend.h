#ifndef UI_H
#define UI_H


#include <QJsonArray>
#include <QObject>
#include <QQuickItem>

#include <vector>
#include "selmat.h"

class Backend : public QObject
{
    Q_OBJECT
//    enum class Imagetype { finded,loaded };
    //Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    //Q_PROPERTY(bool ready READ getReady WRITE setReady)


    bc::CompireFunction mainCompireFunction;
//    cv::Mat marcedImg;
    cv::Mat resultImg;
    QString ver = "2.7.0";
    //without '/'
    QString prjPath;
    QString prjName;
    cv::Mat source;
    bool compired = false;
    bool imgLoaded=false;
    bool prjProcessed = false;
    uchar minPorogOfBarcodeLineLen=10;
    uchar minPorogOfBarcodeShoject=100;
    int minS=800,maxS=7000;
    ProcessType avgType;
    uchar avgRangeMore;
    uchar avgRangeLess;
    std::vector<cv::Rect> findedImgRects;
    std::vector<bc::Barbase*> findedImgBars;
    std::vector<uchar> findedImgProc;
    QHash<int,SelMat*> selectedImgs;
    //QVector<int> loadedImg;

     bool drawProcents = false;
    int len = -1;
    bool parceJson(QString path, QString prjPath);
public:
    static bool useOneBar;
    static bool useRgbBar;
    static bool saveTrainBars;
    static std::vector<bc::barstruct> getBarStr();
    QPixmap getSource();
    explicit Backend(QObject *parent = nullptr);
    QObject *rootWindow;
    Q_INVOKABLE int createProject(QString prjPath, QString prjName, QString imgPath = "");
    Q_INVOKABLE bool openProject(QString prjPath);
    Q_INVOKABLE bool loadMainImg(QString path);
    Q_INVOKABLE void process();
    Q_INVOKABLE int loadSImgFormDisk(QString path);
    Q_INVOKABLE void removeSelectedImg(int index);
    Q_INVOKABLE QString getFindedImgs(int id);
    Q_INVOKABLE QImage getSelectedImg(int id);
    Q_INVOKABLE QString getImg(int id, bool needAlpha);
    Q_INVOKABLE QStringList getParts();
    Q_INVOKABLE QString getProcessedSubImg(int id);
    Q_INVOKABLE int getImgAvg(int id, int type);
    Q_INVOKABLE void save();
    Q_INVOKABLE uchar getImgMid(int id);
    Q_INVOKABLE QJsonObject getImgInfo(int id);
    Q_INVOKABLE QJsonObject getImgBar(int id, int porog);

    Q_INVOKABLE QString processImg(int id, int type, int less, int mid, int more, bool isAuto);
    Q_INVOKABLE QStringList getAllSelected();
    Q_INVOKABLE bool checkExist(int id);
    Q_INVOKABLE int addPartToSelects(int id);
    Q_INVOKABLE void processMainImg(bool skipWhite, bool skipBlack, int avgType, int funType, int fone, int dropFone);
    Q_INVOKABLE void findSimmularObjects(int barcodePorog, int proc, int type, bool normalize);
    Q_INVOKABLE void retryCompire(int nedBarcodePorog);
    Q_INVOKABLE void exportResult(QString path, bool expAll);
    Q_INVOKABLE void presetProcess(int minS, int maxS, bool useOne, bool useRgbBar);
    bool createJson(QString path);

    bool getReady() const;

    void setReady(bool imgLoaded);
    void addImgToTable(int id, QString path);
    int getTypeLen(int off = 0);

    cv::Mat detect(bool skipWhite, bool skipBlack, ProcessType avgtype, ProcessType funType, int fone, int dropFone, int minS, int maxS);
    uchar compireObjects(int id, uchar porog, bool normalize);
    float compireBarcode(const bc::Barbase *X, const bc::Barbase *Y);
    Q_INVOKABLE QImage tryProcessMainImage(bool skipWhite, bool skipBlack, int avgType,int funType, int fone, int dropFone);


    Q_PROPERTY(bool imgLoaded MEMBER imgLoaded)// NOTIFY imgLoadedChanged)
    Q_PROPERTY(bool prjProcessed MEMBER prjProcessed)//  NOTIFY prjProcessedChanged)
    Q_PROPERTY(bool compired MEMBER compired)// NOTIFY compiredChanged)
    Q_PROPERTY(int barcodePorog MEMBER minPorogOfBarcodeShoject)// NOTIFY barcodePorogChanged)
    Q_PROPERTY(QString prjPath MEMBER prjPath)

    /***UI***/
    bool displayAllFinded = true;
    bool displayResults = true;
    //    int minBarcodeShoj=100;
    Q_PROPERTY(bool displayAllFinded MEMBER displayAllFinded)// NOTIFY displayAllFindedChanged)
    Q_PROPERTY(bool displayResults MEMBER displayResults)// NOTIFY displayResultsChanged)
    Q_PROPERTY(int minBarcodeShoj MEMBER minPorogOfBarcodeShoject)// NOTIFY minPorogOfBarcodeShojectChanged)
    Q_PROPERTY(int minBarcodeLen MEMBER minPorogOfBarcodeLineLen)// NOTIFY minPorogOfBarcodeLineLenChanged)
    Q_PROPERTY(int minS MEMBER minS)
    Q_PROPERTY(int maxS MEMBER maxS)
    Q_PROPERTY(int useOneBar MEMBER useOneBar)
    /********/
    std::vector<std::vector<cv::Point> > myDetectInv(ProcessType avgType, ProcessType funType, uchar porog, bool skipWhite, bool skipBlack, int *avg);

signals:
//    void imgLoadedChanged();
//    void prjProcessedChanged();
//    void compiredChanged();
//    void barcodePorogChanged();
//    void displayAllFindedChanged();
//    void displayResultsChanged();
//    void minPorogOfBarcodeShojectChanged();
//    void minPorogOfBarcodeLineLenChanged();

};

#endif // UI_H
