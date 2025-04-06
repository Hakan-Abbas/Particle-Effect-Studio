#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	QString defaultPath;
	QString filename = "";
	QByteArray bytes, data;
	quint32 width, height, size, headerSize;
	bool ok;
	quint8 RGBValue, YUVValue;
	qint16 colorCorrectionR, colorCorrectionG, colorCorrectionB, colorBrightness, colorContrast;
	float gamma;

	QColor snowColor, rainColor;
	QColor fogRainColor, fogBokehColor, fogSnowColor;
    QColor bokehColor1, bokehColor2, bokehColorEdge;

	bool highQuality = false;

	QPen rainPen;
    QBrush fogBrush;

	QGraphicsScene *scene = new QGraphicsScene();
	QGraphicsEllipseItem *ellipse;
	QGraphicsRectItem *rect;
	QGraphicsLineItem *line;
	QGraphicsRectItem *fogRect;
	QGraphicsItemGroup *group;

    QList <QGraphicsItemGroup*> LAYER_RAIN;
    QList <QGraphicsItemGroup*> LAYER_SNOW;
    QList <QGraphicsItemGroup*> LAYER_BOKEH;

	bool drawRain = false, drawSnow = false, drawBokeh = false;

    QGraphicsPixmapItem *pixItem;

	quint8 totalRainLayer = 0, totalSnowLayer = 0, totalBokehLayer = 0;
//	quint8 rainLayerIndex = 0, snowLayerIndex = 0, bokehLayerIndex = 0;

	qint32 posX, posY, sliderX, sliderY;
	float scale;

	QTimer *timer;

	const quint8 gaussian1[9] = {1,2,1,2,4,2,1,2,1};
//	const quint8 gaussian1[9] = {0,0,0,1,1,1,0,0,0};
//	const quint8 gaussian2[25] = {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0};
	const quint8 gaussian2[25] = {1,4,6,4,1,4,16,24,16,4,6,24,36,24,6,4,16,24,16,4,1,4,6,4,1};
	const quint8 gaussian3[25] = {1,2,4,2,1,2,6,9,6,2,4,9,16,9,4,2,6,9,6,2,1,2,4,2,1};

private slots:
    void createHeader();

	void drawSceneRegion();

	void snow();

	void rain();

	quint32 random();

	void on_pushOpen_clicked();

	void on_checkFit_clicked();

	void on_pushGray_clicked();

	void on_pushNegative_clicked();

	void on_doubleSpinScaleX_valueChanged(double arg1);

	void on_doubleSpinScaleY_valueChanged(double arg1);

	void on_dial_valueChanged(int value);

	void resizeEvent(QResizeEvent* event);

	void showEvent(QShowEvent *event);

	void mousePress();

	void mouseMove();

	void mouseRelease();

	void mouseLeave();

	void mouseWheel();

	void on_pushColorize_clicked();

	void on_pushReset_clicked();

	void on_pushR_clicked();

	void on_pushG_clicked();

	void on_pushB_clicked();

	void on_radioRGB_clicked();

	void on_radioYUV_clicked();

	void on_pushY_clicked();

	void on_pushU_clicked();

	void on_pushV_clicked();

	/////////////////////////
	void colorChannelSelectorRGB(quint8);

	void colorChannelSelectorYUV(quint8);

	void showImage(bool);

	void on_pushNew_clicked();

	void on_pushSnow_clicked();

	void on_pushSnowClear_clicked();

	void on_pushSnowColor_clicked();

	void on_checkQuality_stateChanged(int arg1);

	void on_pushSave_clicked();

	void on_pushRain_clicked();

	void on_pushRainClear_clicked();

	void on_pushRainFogColor_clicked();

    void on_pushBokeh_clicked();

	void on_pushBokehColor1_clicked();

	void on_pushBokehColor2_clicked();

	void on_pushBokehClear_clicked();

	void on_tableWidgetRain_cellClicked(int row, int column);

	void on_pushRainLayerUp_clicked();

	void on_pushRainLayerDown_clicked();

	void on_pushRainLayerDelete_clicked();

	void on_pushRainColor_clicked();

	void on_toolBox_currentChanged(int index);

	void on_sliderColorR_valueChanged(int value);

	void on_sliderColorG_valueChanged(int value);

	void on_sliderColorB_valueChanged(int value);

	void colorCorrection();

	void on_sliderGamma_valueChanged(int value);

	void on_sliderBrightness_valueChanged(int value);

	void on_sliderContrast_valueChanged(int value);

    void on_radioColorSmart_clicked();

    void on_radioColorClick_clicked();

    void on_radioColorManual_clicked();

    void on_pushBokehFogColor_clicked();

	void on_pushSnowFogColor_clicked();

	void on_pushSnowLayerUp_clicked();

	void on_pushSnowLayerDown_clicked();

	void on_pushSnowLayerDelete_clicked();

	void on_pushBokehLayerUp_clicked();

	void on_pushBokehLayerDown_clicked();

	void on_pushBokehLayerDelete_clicked();

	void on_tableWidgetSnow_cellClicked(int row, int column);

	void on_tableWidgetBokeh_cellClicked(int row, int column);

	void on_pushRealBokeh1_clicked();

	void on_pushRealBokeh2_clicked();

    void on_checkHide_stateChanged(int arg1);

private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
