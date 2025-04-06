#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsView>
#include <QScrollBar>
#include <QDebug>
#include <QtMath>
#include <QTime>
#include <QGraphicsPixmapItem>
#include <QGraphicsBlurEffect>
#include <QGraphicsItemGroup>
#include <QColorDialog>
#include <QColor>
#include <QRadialGradient>
#include <QRandomGenerator>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	//defaultPath = "C:/Users/Bil/Documents/QT/img/bmp";
	defaultPath = "C:/Users/Hakan/QT_Compression/GUI_Encode1/bmp";

	connect(ui->graphicsView, SIGNAL(mouseMove()), this, SLOT(mouseMove()));
	connect(ui->graphicsView, SIGNAL(mousePress()), this, SLOT(mousePress()));
	connect(ui->graphicsView, SIGNAL(mouseRelease()), this, SLOT(mouseRelease()));
	connect(ui->graphicsView, SIGNAL(mouseLeave()), this, SLOT(mouseLeave()));
	connect(ui->graphicsView, SIGNAL(mouseWheel()), this, SLOT(mouseWheel()));

//	connect(ui->spinGamma, SIGNAL(valueChanged(double)), ui->sliderGamma, SLOT(setValue(double)) );
//	connect(ui->sliderGamma, SIGNAL(valueChanged(double)), ui->spinGamma, SLOT(setValue(double)) );

	//connect(scene, &QGraphicsScene::changed, this, &MainWindow::trans);
	//connect(scene, SIGNAL(changed(const QList<QRectF> &)), this, SLOT(trans()));

//	timer = new QTimer();
//	connect (timer, SIGNAL(timeout()), this, SLOT(trans()));

	ui->graphicsView->setCursor(Qt::OpenHandCursor);

	ui->frameRGB->setEnabled(true); ui->frameYUV->setEnabled(false);

	on_pushNew_clicked();

	ui->graphicsView->setScene(scene);

	fogRainColor = QColor(255,255,255,0);
	fogSnowColor = QColor(255,255,255,0);
	fogBokehColor = QColor(255,255,255,0);

	rainColor = snowColor = Qt::white;
	bokehColor1 = QColor(33,255,170,200);
	bokehColor2 = QColor(255,37,40,200);
	//bokehColorEdge = Qt::white;

	QString qss1 = QString("background-color: %1").arg(bokehColor1.name());
	ui->labelBokehColor1->setStyleSheet(qss1);

	QString qss2 = QString("background-color: %1").arg(bokehColor2.name());
	ui->labelBokehColor2->setStyleSheet(qss2);

	//ui->tableWidgetRain->setRowCount(totalRainLayer);
	ui->tableWidgetRain->setColumnWidth(0,35); ui->tableWidgetRain->setColumnWidth(1,170);
	ui->tableWidgetRain->verticalHeader()->setDefaultSectionSize(20);

	//ui->tableWidgetRain->setRowCount(totalRainLayer);
	ui->tableWidgetSnow->setColumnWidth(0,35); ui->tableWidgetSnow->setColumnWidth(1,170);
	ui->tableWidgetSnow->verticalHeader()->setDefaultSectionSize(20);

	//ui->tableWidgetRain->setRowCount(totalRainLayer);
	ui->tableWidgetBokeh->setColumnWidth(0,35); ui->tableWidgetBokeh->setColumnWidth(1,170);
	ui->tableWidgetBokeh->verticalHeader()->setDefaultSectionSize(20);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::createHeader()
{
    //data.clear();
	scene->clear();
	ui->graphicsView->items().clear();

	QByteArray widthStr, heightStr;
	widthStr = QByteArray::number(width);
	heightStr = QByteArray::number(height);

	// Creating PPM format header
	data.append("P6 ");
	data.append(widthStr).append(" ");
	data.append(heightStr).append(" ");
	data.append("255 ");

	headerSize = 9 + widthStr.size() + heightStr.size();
}

void MainWindow::drawSceneRegion()
{
	rect = scene->addRect(-1, -1, width + 2, height + 2, QColor(128,128,128), Qt::NoBrush);
}

void MainWindow::on_pushOpen_clicked()
{
	QFileDialog dialog(this);
	filename = dialog.getOpenFileName(this, tr("Open File"), defaultPath, "All Image Files (*.bmp; *.png; *.ppm; *.tif; *.jpg; *.jpeg; *.webp);;BMP Files (*.bmp);;PNG Files (*.png);;PPM Files (*.ppm);;TIFF Files (*.tif);;JPEG Files (*.jpg; *.jpeg);;WEBP Files (*.webp);;All Files (*.*)");
	if (filename != "") defaultPath = filename.left(filename.lastIndexOf("/"));
	else return;

	on_pushReset_clicked();
}

void MainWindow::on_checkFit_clicked()
{
	if (ui->checkFit->isChecked()) ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
	else ui->graphicsView->resetTransform();

	scale = ui->graphicsView->transform().m11();
	ui->doubleSpinScale->setValue(scale);
}

void MainWindow::on_pushGray_clicked()
{
	if (filename == "") return;

	createHeader();

	float R, G, B, total;
	for (int i = 0; i < size; i += 3) {
		R = (quint8)bytes.at(i) * .299;
		G = (quint8)bytes.at(i+1) * .587;
		B = (quint8)bytes.at(i+2) * .114;
		total = R + G + B;
		//qDebug() << R << G << B << total;
		data.append(total).append(total).append(total);
	}

	showImage(false);

}

void MainWindow::on_pushNegative_clicked()
{
	if (filename == "") return;

	createHeader();

	for (int i = 0; i < size; i++) {
		data.append(255 - bytes.at(i));
	}

	showImage(false);

}

void MainWindow::on_doubleSpinScaleX_valueChanged(double arg1)
{
	QTransform t;
	t.rotate(ui->dial->value());
	t.scale(ui->doubleSpinScaleX->value(),ui->doubleSpinScaleY->value());

	ui->graphicsView->setTransform(t);
}

void MainWindow::on_doubleSpinScaleY_valueChanged(double arg1)
{
	QTransform t;
	t.rotate(ui->dial->value());
	t.scale(ui->doubleSpinScaleX->value(), ui->doubleSpinScaleY->value());

	ui->graphicsView->setTransform(t);
}

void MainWindow::on_dial_valueChanged(int value)
{	
	QTransform t;
	t.rotate(ui->dial->value());
	t.scale(ui->doubleSpinScaleX->value(), ui->doubleSpinScaleY->value());

	ui->graphicsView->setTransform(t);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
	int right = 260;
	int left = ui->centralWidget->width() - right;
	ui->splitter->setSizes(QList<int>() << left << right);

	ui->scrollAreaImage->setWidget(ui->frameImage);
	ui->scrollAreaImage->setGeometry(0,0,ui->pageImage->width(), ui->pageImage->height());

	ui->scrollAreaRain->setWidget(ui->frameRain);
	ui->scrollAreaRain->setGeometry(0,0,ui->pageRain->width(), ui->pageRain->height());

	ui->scrollAreaSnow->setWidget(ui->frameSnow);
	ui->scrollAreaSnow->setGeometry(0,0,ui->pageSnow->width(), ui->pageSnow->height());

	ui->scrollAreaBokeh->setWidget(ui->frameBokeh);
	ui->scrollAreaBokeh->setGeometry(0,0,ui->pageBokeh->width(), ui->pageBokeh->height());

}

void MainWindow::showEvent(QShowEvent *event)
{

}

void MainWindow::mouseRelease()
{
	ui->graphicsView->setCursor(Qt::OpenHandCursor);

	if (filename == "" || ui->toolBox->currentIndex() != 3 || ui->radioColorManual->isChecked()) return;

	posX = ui->graphicsView->x;
	posY = ui->graphicsView->y;
	drawBokeh = true;
	QBrush brush;
	brush.setStyle(Qt::SolidPattern);
	QPen pen;
	pen.setStyle(Qt::SolidLine);

	int X, Y;
	int bokehCount = ui->spinBokehCount->value();
	int N = ui->spinBokehSide->value();

	int area = ui->sliderBokehArea->value();
	int radius = ui->sliderBokehRadius->value();

	int bokehSizeMin = ui->spinBokehSizeMin->value();
	int BokehSizeMax = ui->spinBokehSizeMax->value();

	int bokehRotationMin = ui->spinBokehRotationMin->value();
	int BokehRotationMax = ui->spinBokehRotationMax->value();

	int bokehBlurMin = ui->spinBokehBlurMin->value();
	int BokehBlurMax = ui->spinBokehBlurMax->value();

	float bokehThicknessMin = ui->spinBokehThicknessMin->value();
	float BokehThicknessMax = ui->spinBokehThicknessMax->value();

	float bokehOpacityMin = ui->spinBokehOpacityMin->value();
	float BokehOpacityMax = ui->spinBokehOpacityMax->value();

	float bokehEdgeOpacityMin = ui->spinBokehEdgeOpacityMin->value();
	float BokehEdgeOpacityMax = ui->spinBokehEdgeOpacityMax->value();


	quint8** colorR = new quint8*[height];
	for(int i = 0; i < height; ++i) colorR[i] = new quint8[width];
	quint8** colorG = new quint8*[height];
	for(int i = 0; i < height; ++i) colorG[i] = new quint8[width];
	quint8** colorB = new quint8*[height];
	for(int i = 0; i < height; ++i) colorB[i] = new quint8[width];

	int counter = 0;
	for (int i=0; i < height; i++) {
		for (int j=0; j < width; j++) {

			colorR[i][j] = bytes.at(counter);
			colorG[i][j] = bytes.at(counter + 1);
			colorB[i][j] = bytes.at(counter + 2);

			counter += 3;
//			qDebug() << colorR[i][j] << colorG[i][j] << colorB[i][j];
		}
	}



	group = new QGraphicsItemGroup;

	for (int j=0; j<bokehCount; j++) {

		int bokehSize = random() % (BokehSizeMax - bokehSizeMin + 1) + bokehSizeMin;

		if (ui->radioColorClick->isChecked()) {
			X = posX + random() % (2 * area) - area;
			Y = posY + random() % (2 * area) - area;
		}
		else {
			X = random() % (width + bokehSize) - bokehSize;
			Y = random() % (height + bokehSize) - bokehSize;
		}


		int bokehRotation = random() % (BokehRotationMax - bokehRotationMin + 1) + bokehRotationMin;
		int bokehBlur = random() % (BokehBlurMax - bokehBlurMin + 1) + bokehBlurMin;
		float bokehThickness = random() % (int)((BokehThicknessMax - bokehThicknessMin) * 100 + 1) / 100.0 + bokehThicknessMin;
		pen.setWidthF(bokehThickness);
		float bokehOpacity = random() % (int)((BokehOpacityMax - bokehOpacityMin) * 100 + 1) / 100.0 + bokehOpacityMin;
		float bokehEdgeOpacity = random() % (int)((BokehEdgeOpacityMax - bokehEdgeOpacityMin) * 100 + 1) / 100.0 + bokehEdgeOpacityMin;

		QGraphicsBlurEffect *blurEffect = new QGraphicsBlurEffect();
		if (highQuality) blurEffect->setBlurHints(QGraphicsBlurEffect::QualityHint);
		else blurEffect->setBlurHints(QGraphicsBlurEffect::PerformanceHint);
		blurEffect->setBlurRadius(bokehBlur);


//		R = bytes.at(posX * 3  + posY * 3 * width) + random() % (2*20) - 20;
//		G = bytes.at(posX * 3  + posY * 3 * width + 1) + random() % (2*20) - 20;
//		B = bytes.at(posX * 3  + posY * 3 * width + 2) + random() % (2*20) - 20;

		int iStart, iEnd, kStart, kEnd;

		if (ui->radioColorClick->isChecked()) {
			iStart = posY - radius; if (iStart < 0) iStart = 0; if (iStart > height-1) iStart = height-1;
			iEnd = posY + radius; if (iEnd < 0) iEnd = 0; if (iEnd > height-1) iEnd = height-1;
			kStart = posX - radius; if (kStart < 0) kStart = 0; if (kStart > width-1) kStart = width-1;
			kEnd = posX + radius; if (kEnd < 0) kEnd = 0; if (kEnd > width-1) kEnd = width-1;
		}
		else {
			iStart = Y - radius; if (iStart < 0) iStart = 0; if (iStart > height-1) iStart = height-1;
			iEnd = Y + radius; if (iEnd < 0) iEnd = 0; if (iEnd > height-1) iEnd = height-1;
			kStart = X - radius; if (kStart < 0) kStart = 0; if (kStart > width-1) kStart = width-1;
			kEnd = X + radius; if (kEnd < 0) kEnd = 0; if (kEnd > width-1) kEnd = width-1;
		}

//qDebug () << iStart << iEnd << kStart << kEnd;
		int totalR = 0, totalG = 0, totalB = 0;

		for (int i = iStart; i < iEnd; i++) {
			for (int k = kStart; k < kEnd; k++) {
				totalR += colorR[i][k];
				totalG += colorG[i][k];
				totalB += colorB[i][k];
//				qDebug () << colorR[i][k] << totalR << totalR / (40*40) << counter;
			}
		}

		int R = totalR / (4*radius*radius) + 2*radius; R += random() % (2*radius); if (R>255) R=255;
		int G = totalG / (4*radius*radius) + 2*radius; G += random() % (2*radius); if (G>255) G=255;
		int B = totalB / (4*radius*radius) + 2*radius; B += random() % (2*radius); if (B>255) B=255;

//		qDebug () << R << G << B;

		brush.setColor(QColor(R,G,B,bokehOpacity*255));
		pen.setColor(QColor(R,G,B,bokehEdgeOpacity*255));

		if (N >= 3) {
			QPolygonF poly;
			for(int i=0; i<N; i++) {
				poly << QPointF( X + bokehSize * qCos(i * 360.0 / N * 3.14 / 180.0) , Y + bokehSize * qSin(i * 360.0 / N * 3.14 / 180.0) );
			}

			QGraphicsPolygonItem *bokeh;
			bokeh = scene->addPolygon(poly, pen, brush);
			bokeh->setTransformOriginPoint(bokeh->boundingRect().center());
			bokeh->setRotation(bokehRotation);
			bokeh->setGraphicsEffect(blurEffect);
			bokeh->setOpacity(bokehOpacity);
			group->addToGroup(bokeh);
		}
		else {
			ellipse = scene->addEllipse(X, Y, bokehSize, bokehSize, pen, brush);
			ellipse->setTransformOriginPoint(ellipse->rect().center());
			ellipse->setRotation(bokehRotation);
			ellipse->setGraphicsEffect(blurEffect);
			ellipse->setOpacity(bokehOpacity);
			group->addToGroup(ellipse);

		}
	}

	scene->addItem(group);

}

void MainWindow::mousePress()
{
	scale = ui->graphicsView->transform().m11();
	ui->graphicsView->setCursor(Qt::ClosedHandCursor);
	posX = scale * ui->graphicsView->x;
	posY = scale * ui->graphicsView->y;

}

void MainWindow::mouseMove()
{
	//qDebug() << ui->graphicsView->x << posX;
	ui->graphicsView->horizontalScrollBar()->setValue( ui->graphicsView->horizontalScrollBar()->value() - (scale * ui->graphicsView->x - posX) );
	ui->graphicsView->verticalScrollBar()->setValue( ui->graphicsView->verticalScrollBar()->value() - (scale * ui->graphicsView->y - posY) );
}

void MainWindow::mouseLeave()
{

}

void MainWindow::mouseWheel()
{
//	scale = ui->graphicsView->transform().m11();
//	ui->doubleSpinScale->setValue(scale);
}

void MainWindow::on_pushColorize_clicked()
{
	if (filename == "") return;

	createHeader();

	QVector < QVector <quint8> > R; QVector <quint8> RVal;
	QVector < QVector <quint8> > G; QVector <quint8> GVal;
	QVector < QVector <quint8> > B; QVector <quint8> BVal;
	quint32 counter = 0;
	quint8 R_, G_, B_;

	for (int i=0; i<height; i++) {
		for (int j=0; j<width; j++) {
			R_ = bytes.at(counter);
			G_ = bytes.at(counter + 1);
			B_ = bytes.at(counter + 2);
			counter += 3;
			//qDebug()<< R << G << B;

			RVal.append(R_);
			GVal.append(G_);
			BVal.append(B_);
		}
		//qDebug()<< YVal << UVal << VVal;
		R.append(RVal); G.append(GVal); B.append(BVal);
		RVal.clear(); GVal.clear(); BVal.clear();
	}

	int valR, valG, valB;
	float valueStep, limitStep;
	const quint8 colorCount = ui->comboColorCount->currentText().toInt(&ok,10);
	QVector <quint8> colors;
	QVector <quint8> limits;

	valueStep = 255.0 / (colorCount - 1);
	limitStep = 256 / colorCount;

	for (int i = 0; i < colorCount; i++) colors.append(qRound(valueStep * i));
	for (int i = 1; i < colorCount; i++) limits.append(qRound(limitStep * i)); // qDebug() << limitStep * i;

	for (int i=0; i<height; i++) {
		for (int j=0; j<width; j++) {

			valR = 255; valG = 255; valB = 255;
			for (int k=0; k<colorCount-1; k++) if (R.at(i).at(j) < limits.at(k)) { valR = colors.at(k); break; }
			for (int k=0; k<colorCount-1; k++) if (G.at(i).at(j) < limits.at(k)) { valG = colors.at(k); break; }
			for (int k=0; k<colorCount-1; k++) if (B.at(i).at(j) < limits.at(k)) { valB = colors.at(k); break; }

			data.append(valR).append(valG).append(valB);
		}
	}

	R.clear(); G.clear(); B.clear();

	showImage(false);
}

void MainWindow::on_pushReset_clicked()
{
    //data.clear();
	if (filename == "") return;
	scene->clear();
	//ui->graphicsView->items().clear();

    QImage imageObject;
	imageObject.load(filename);
	imageObject = imageObject.convertToFormat(QImage::Format_RGB888);
	width = imageObject.width();
	height = imageObject.height();
	size = width * height * 3;

	const uchar *bits;
	bytes.clear();
	for (int i = 0; i < height; i++) {
		bits = imageObject.constScanLine(i);
		bytes.append( QByteArray((char*)(bits), width * 3) );
	}

    QPixmap imagePixmap = QPixmap::fromImage(imageObject);
//    scene->addPixmap(imagePixmap);
//    scene->setSceneRect(imagePixmap.rect());
    pixItem = scene->addPixmap(imagePixmap);
    scene->setSceneRect(imagePixmap.rect());


	if (ui->checkFit->isChecked()) ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
	else ui->graphicsView->resetTransform();

	RGBValue = 0;
	YUVValue = 0;
	ui->radioRGB->setChecked(true);
	ui->frameRGB->setEnabled(true); ui->frameYUV->setEnabled(false);
	ui->pushR->setChecked(false); ui->pushG->setChecked(false); ui->pushB->setChecked(false);
	ui->pushY->setChecked(false); ui->pushU->setChecked(false); ui->pushV->setChecked(false);

	ui->sliderColorR->setValue(0); ui->sliderColorG->setValue(0); ui->sliderColorB->setValue(0);
	ui->sliderBrightness->setValue(0); ui->sliderContrast->setValue(0); ui->sliderGamma->setValue(400);

}

void MainWindow::on_pushR_clicked()
{
	if (ui->pushR->isChecked()) RGBValue ++;
	else RGBValue--;
	colorChannelSelectorRGB(RGBValue);
}

void MainWindow::on_pushG_clicked()
{
	if (ui->pushG->isChecked()) RGBValue += 2;
	else RGBValue -= 2;
	colorChannelSelectorRGB(RGBValue);
}

void MainWindow::on_pushB_clicked()
{
	if (ui->pushB->isChecked()) RGBValue += 4;
	else RGBValue -= 4;
	colorChannelSelectorRGB(RGBValue);
}

void MainWindow::colorChannelSelectorRGB(quint8 color)
{
    if (filename == "") return;
	//qDebug() << RGBValue;
	createHeader();

	quint8 R,G,B;
	for (int i = 0; i < size; i += 3) {
		R = bytes.at(i);
		G = bytes.at(i+1);
		B = bytes.at(i+2);
//		qDebug() << R << G << B;
		switch (color) {
			case 0:
				data.append(R).append(G).append(B);
				break;
			case 1:
				data.append(R).append(R).append(R);
				break;
			case 2:
				data.append(G).append(G).append(G);
				break;
			case 4:
				data.append(B).append(B).append(B);
				break;
			case 3:
				data.append(R).append(G).append((char)0);
				break;
			case 5:
				data.append(R).append((char)0).append(B);
				break;
			case 6:
				data.append((char)0).append(G).append(B);
				break;
			case 7:
				data.append(R).append(G).append(B);
				break;
		}
	}

	showImage(false);
}

void MainWindow::colorChannelSelectorYUV(quint8 color)
{
    if (filename == "") return;
//	qDebug() << YUVValue;
	createHeader();

	quint8 R, G, B, Y,U,V;
	for (int i = 0; i < size; i += 3) {
		R = bytes.at(i);
		G = bytes.at(i+1);
		B = bytes.at(i+2);

		Y = R * 0.299 + G * 0.587 + B * 0.114;
		U = R * -0.14713 + G * -0.28886 + B * 0.436 + 128;
		V = R * 0.615 + G * -0.51499 + B * -0.10001 + 128;
//		qDebug() << Y << U << V;
		switch (color) {
			case 0:
				data.append(Y).append(U).append(V);
				break;
			case 1:
				data.append(Y).append(Y).append(Y);
				break;
			case 2:
				data.append(U).append(U).append(U);
				break;
			case 4:
				data.append(V).append(V).append(V);
				break;
			case 3:
				data.append(Y).append(U).append((char)0);
				break;
			case 5:
				data.append(Y).append((char)0).append(V);
				break;
			case 6:
				data.append((char)0).append(U).append(V);
				break;
			case 7:
				data.append(Y).append(U).append(V);
				break;
		}
	}

	showImage(false);
}

void MainWindow::showImage(bool state)
{
	QPixmap pixmap;
	pixmap.loadFromData(data, "PPM");

	if (state) bytes = data.mid(headerSize);
	data.clear();

	scene->addPixmap(pixmap);
	scene->setSceneRect(pixmap.rect());
}

void MainWindow::on_radioRGB_clicked()
{
	RGBValue = 0;
	ui->frameRGB->setEnabled(true); ui->frameYUV->setEnabled(false);
	ui->pushR->setChecked(false); ui->pushG->setChecked(false); ui->pushB->setChecked(false);
	ui->pushY->setChecked(false); ui->pushU->setChecked(false); ui->pushV->setChecked(false);
	colorChannelSelectorRGB(RGBValue);
}

void MainWindow::on_radioYUV_clicked()
{
	YUVValue = 0;
	ui->frameRGB->setEnabled(false); ui->frameYUV->setEnabled(true);
	ui->pushY->setChecked(false); ui->pushU->setChecked(false); ui->pushV->setChecked(false);
	ui->pushR->setChecked(false); ui->pushG->setChecked(false); ui->pushB->setChecked(false);
	colorChannelSelectorYUV(YUVValue);
}

void MainWindow::on_pushY_clicked()
{
	if (ui->pushY->isChecked()) YUVValue ++;
	else YUVValue --;
	colorChannelSelectorYUV(YUVValue);
}

void MainWindow::on_pushU_clicked()
{
	if (ui->pushU->isChecked()) YUVValue += 2;
	else YUVValue -= 2;
	colorChannelSelectorYUV(YUVValue);
}

void MainWindow::on_pushV_clicked()
{
	if (ui->pushV->isChecked()) YUVValue += 4;
	else YUVValue -= 4;
	colorChannelSelectorYUV(YUVValue);
}

void MainWindow::on_pushSnow_clicked()
{
	fogSnowColor = QColor(fogSnowColor.red(),fogSnowColor.green(),fogSnowColor.blue(),ui->spinSnowFog->value());
	fogBrush.setStyle(Qt::SolidPattern);
	fogBrush.setColor(fogSnowColor);

	snow();

	if (drawSnow) scene->removeItem(fogRect);
	fogRect = scene->addRect(0, 0, width, height, Qt::NoPen, fogBrush);

	drawSnow = true;

}

void MainWindow::snow() {

	LAYER_SNOW.append(new QGraphicsItemGroup());

	int particleSizeMin, particleSizeMax, particleSize;
	int particleRotationMin, particleRotationMax, particleRotation;
	int particleBlurMin, particleBlurMax, particleBlur;
	int particleKernelMin, particleKernelMax, particleKernel;
	int particleMotionMin, particleMotionMax, particleMotion;
	double particleOpacityMin, particleOpacityMax, particleOpacity;
	int startX, startY;

	for (int i=0; i<ui->spinSnowCount->value(); i++) {

		particleSizeMin = ui->spinSnowSizeMin->value();
		particleSizeMax = ui->spinSnowSizeMax->value();
		particleSize = random() % (particleSizeMax - particleSizeMin + 1) + particleSizeMin;

		particleRotationMin = ui->spinSnowRotationMin->value();
		particleRotationMax = ui->spinSnowRotationMax->value();
		particleRotation = random() % (particleRotationMax - particleRotationMin + 1) + particleRotationMin;

		particleBlurMin = ui->spinSnowBlurRadiusMin->value();
		particleBlurMax = ui->spinSnowBlurRadiusMax->value();
		particleBlur = random() % (particleBlurMax - particleBlurMin + 1) + particleBlurMin;

		particleKernelMin = ui->spinSnowKernelMin->value();
		particleKernelMax = ui->spinSnowKernelMax->value();
		particleKernel = random() % (particleKernelMax - particleKernelMin + 1) + particleKernelMin;

		particleMotionMin = ui->spinSnowMotionMin->value();
		particleMotionMax = ui->spinSnowMotionMax->value();
		particleMotion = random() % (particleMotionMax - particleMotionMin + 1) + particleMotionMin;

		particleOpacityMin = ui->spinSnowOpacityMin->value();
		particleOpacityMax = ui->spinSnowOpacityMax->value();
		particleOpacity = random() % (int)((particleOpacityMax - particleOpacityMin) * 100 + 1) / 100.0 + particleOpacityMin;

//		qDebug() << ui->spinSnowCount->value() << i;

		startX = random() % width;
		startY = random() % (height + particleSize) - particleSize;


		QGraphicsItemGroup *group = new QGraphicsItemGroup();
		QGraphicsBlurEffect *blurEffect = new QGraphicsBlurEffect();
		if (highQuality) blurEffect->setBlurHints(QGraphicsBlurEffect::QualityHint);
		else blurEffect->setBlurHints(QGraphicsBlurEffect::PerformanceHint);
		//blurEffect->setBlurHints(QGraphicsBlurEffect::AnimationHint);

		int tx, ty, rt;
		float sx, sy;
//		sx = sy = particleSize;
		int newSizeX = particleSize - particleMotion / 2 + 1;
		if (newSizeX < 1) newSizeX = 1;
		int newSizeY = particleSize + particleMotion + 1;


			for (int j=0; j<particleKernel; j++) {
				tx = random() % (newSizeX) - newSizeX / 2;
				ty = random() % (newSizeY) - newSizeY / 2;

				sx = random() % (particleSize / 2 + 1) + (particleSize / 2);
				sy = random() % (particleSize / 2 + 1) + (particleSize / 2);

				rt = random() % 360;

				ellipse = scene->addEllipse(startX + tx, startY + ty, sx, sy, Qt::NoPen, QColor(snowColor.red(), snowColor.green(), snowColor.blue(), snowColor.alpha()));
				ellipse->setTransformOriginPoint(ellipse->rect().center());
				ellipse->setRotation(rt);

				if (ui->checkBoxCloudy->isChecked()) {
					float opacity = random() % (int)((particleOpacityMax - particleOpacityMin) * 100 + 1) / 100.0 + particleOpacityMin;
					ellipse->setOpacity(opacity);
				}

				group->addToGroup(ellipse);
			}

		group->setTransformOriginPoint(group->boundingRect().center());
		group->setRotation(particleRotation);
		group->setOpacity(particleOpacity);
		blurEffect->setBlurRadius(particleBlur);
		group->setGraphicsEffect(blurEffect);

		//scene->addItem(group);
		LAYER_SNOW.at(totalSnowLayer)->addToGroup(group);
	}
	scene->addItem(LAYER_SNOW.at(totalSnowLayer));
	LAYER_SNOW.at(totalSnowLayer)->setZValue(totalSnowLayer+1);

	ui->tableWidgetSnow->setRowCount(totalSnowLayer + 1);
	QTableWidgetItem *itemShow = new QTableWidgetItem();
	QTableWidgetItem *itemLayer = new QTableWidgetItem();
	itemShow->setTextAlignment(Qt::AlignCenter);
	itemShow->setText("Hide");
	itemShow->setFlags(Qt::ItemIsEditable);

	itemLayer->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	itemLayer->setText("New Snow Layer " + QString::number(totalSnowLayer+1));

//	qDebug () << totalSnowLayer;
	ui->tableWidgetSnow->setItem(totalSnowLayer,0,itemShow);
	ui->tableWidgetSnow->setItem(totalSnowLayer,1,itemLayer);
	totalSnowLayer++;
//	ui->tableWidgetSnow->viewport()->update();

}

quint32 MainWindow::random()
{
	return QRandomGenerator::global()->generate();

}

void MainWindow::on_pushSnowClear_clicked()
{
	//qDebug() << ui->graphicsView->items();
    on_pushReset_clicked();

	QList<QGraphicsItem*> allGraphicsItems = ui->graphicsView->items();
	for(int i = 0; i < allGraphicsItems.size()-1; i++)
	{
		QGraphicsItem *graphicItem = allGraphicsItems[i];
		scene->removeItem(graphicItem);
		delete graphicItem;

	}
	//scene->update();
	drawSnow = false;
	ui->tableWidgetSnow->setRowCount(0);
	totalSnowLayer = 0;
	LAYER_SNOW.clear();
	drawSceneRegion();
}

void MainWindow::on_pushNew_clicked()
{
	scene->clear();
	bytes.clear();
	filename = "";

	QPixmap imagePixmap(ui->spinWidth->value(), ui->spinHeight->value());
	width = ui->spinWidth->value(); height = ui->spinHeight->value();
	scene->addPixmap(imagePixmap);
	scene->setSceneRect(imagePixmap.rect());

	drawSceneRegion();
}

void MainWindow::on_pushSnowColor_clicked()
{
	QColor color = QColorDialog::getColor(snowColor, this,"", QColorDialog::ShowAlphaChannel);
	if( color.isValid() ) {
		snowColor = color;
		//qDebug() << snowColor.red() << snowColor.green() << snowColor.blue() << snowColor.alpha();
		QString qss = QString("background-color: %1").arg(color.name());
		ui->labelSnowColor->setStyleSheet(qss);
	}
}

void MainWindow::on_checkQuality_stateChanged(int arg1)
{
    if (arg1 == 2) highQuality = true;
    else if (arg1 == 0) highQuality = false;
}

void MainWindow::on_checkHide_stateChanged(int arg1)
{
    if (filename == "") return;

    if (arg1 == 2) {
//        imagePixmap.fill(Qt::black);
//        scene->addPixmap(imagePixmap);
        pixItem->hide();
    }
    else {
//        imagePixmap = QPixmap::fromImage(imageObject);
//        scene->addPixmap(imagePixmap);
//        scene->setSceneRect(imagePixmap.rect());
        pixItem->show();
    }
}

void MainWindow::on_pushSave_clicked()
{
	QString imagePath = QFileDialog::getSaveFileName(this, tr("Save File"),	"",	tr("PNG (*.png);;TIFF (*.tif *.tiff);;WebP (*.webp)"));
	if (imagePath.isEmpty()) return;

	scene->setSceneRect(0,0,width, height); // Re-shrink the scene to it's bounding contents
	QImage image(width, height, QImage::Format_ARGB32); // Create the image with the exact size of the shrunk scene
	image.fill(Qt::transparent); // Start all pixels transparent

	QPainter painter(&image);
	painter.setRenderHint(QPainter::Antialiasing, true);
	scene->render(&painter);
	image.save(imagePath);

}

void MainWindow::on_pushRain_clicked()
{    
//	rainPen.setColor(QColor(rainColor.red(), rainColor.green(), rainColor.blue(), rainColor.alpha()));
	rainPen.setStyle(Qt::SolidLine); rainPen.setCapStyle(Qt::RoundCap); rainPen.setJoinStyle(Qt::RoundJoin);

	fogRainColor = QColor(fogRainColor.red(),fogRainColor.green(),fogRainColor.blue(),ui->spinRainFog->value());
	fogBrush.setStyle(Qt::SolidPattern);
	fogBrush.setColor(fogRainColor);

	rain();

	if (drawRain) scene->removeItem(fogRect);
    fogRect = scene->addRect(0, 0, width, height, Qt::NoPen, fogBrush);
	//scene->addItem(fogRect);

	drawRain = true;

}

void MainWindow::rain()
{
	LAYER_RAIN.append(new QGraphicsItemGroup());

	int particleSizeMin, particleSizeMax, particleSize;
	int particleRotationMin, particleRotationMax, particleRotation;
	int particleBlurMin, particleBlurMax, particleBlur;
	double particleThicknessMin, particleThicknessMax, particleThickness;
	double particleOpacityMin, particleOpacityMax, particleOpacity;
	int startX, startY;

	for (int i=0; i<ui->spinRainCount->value(); i++) {

		particleSizeMin = ui->spinRainSizeMin->value();
		particleSizeMax = ui->spinRainSizeMax->value();
		particleSize = random() % (particleSizeMax - particleSizeMin + 1) + particleSizeMin;

		particleRotationMin = ui->spinRainRotationMin->value();
		particleRotationMax = ui->spinRainRotationMax->value();
		particleRotation = random() % (particleRotationMax - particleRotationMin + 1) + particleRotationMin;

		particleBlurMin = ui->spinRainBlurRadiusMin->value();
		particleBlurMax = ui->spinRainBlurRadiusMax->value();
		particleBlur = random() % (particleBlurMax - particleBlurMin + 1) + particleBlurMin;

		particleThicknessMin = ui->spinRainThicknessMin->value();
		particleThicknessMax = ui->spinRainThicknessMax->value();
		particleThickness = random() % (int)((particleThicknessMax - particleThicknessMin) * 100 + 1) / 100.0 + particleThicknessMin;

		particleOpacityMin = ui->spinRainOpacityMin->value();
		particleOpacityMax = ui->spinRainOpacityMax->value();
		particleOpacity = random() % (int)((particleOpacityMax - particleOpacityMin) * 100 + 1) / 100.0 + particleOpacityMin;

		// qDebug() << particleSize << particleRotation << particleThickness << particleOpacity << particleBlur;

		startX = random() % width;
		startY = random() % (height + particleSize) - particleSize;

		QRadialGradient gradient(startX, startY, particleSize);

		gradient.setColorAt((random() % 10) / 10.0, QColor(0,0,0,0));
		gradient.setColorAt((random() % 10) / 10.0, QColor(rainColor.red(), rainColor.green(), rainColor.blue(), rainColor.alpha()));
		gradient.setColorAt((random() % 10) / 10.0, QColor(0,0,0,0));
		gradient.setColorAt((random() % 10) / 10.0, QColor(rainColor.red(), rainColor.green(), rainColor.blue(), rainColor.alpha()));
		gradient.setColorAt((random() % 10) / 10.0, QColor(0,0,0,0));
		gradient.setColorAt(1, QColor(rainColor.red(), rainColor.green(), rainColor.blue(), rainColor.alpha()));

		QBrush rainBrush(gradient);
		rainPen.setBrush(rainBrush);
		rainPen.setWidthF(particleThickness);

		line = scene->addLine(startX, startY, startX, startY + particleSize, rainPen);

		line->setTransformOriginPoint(line->boundingRect().center());
		line->setRotation(particleRotation);

		QGraphicsBlurEffect *blurEffect = new QGraphicsBlurEffect();
		if (highQuality) blurEffect->setBlurHints(QGraphicsBlurEffect::QualityHint);
		else blurEffect->setBlurHints(QGraphicsBlurEffect::PerformanceHint);
		blurEffect->setBlurRadius(particleBlur);
		line->setGraphicsEffect(blurEffect);

		line->setOpacity(particleOpacity);

		LAYER_RAIN.at(totalRainLayer)->addToGroup(line);
	}

	scene->addItem(LAYER_RAIN.at(totalRainLayer));
	LAYER_RAIN.at(totalRainLayer)->setZValue(totalRainLayer+1);

	ui->tableWidgetRain->setRowCount(totalRainLayer + 1);
	QTableWidgetItem *itemShow = new QTableWidgetItem();
	QTableWidgetItem *itemLayer = new QTableWidgetItem();
	itemShow->setTextAlignment(Qt::AlignCenter);
	itemShow->setText("Hide");
	itemShow->setFlags(Qt::ItemIsEditable);

	itemLayer->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	itemLayer->setText("New Rain Layer " + QString::number(totalRainLayer+1));


	ui->tableWidgetRain->setItem(totalRainLayer,0,itemShow);
	ui->tableWidgetRain->setItem(totalRainLayer,1,itemLayer);
	totalRainLayer++;
//	ui->tableWidgetRain->viewport()->update();
}

void MainWindow::on_pushRainClear_clicked()
{
	//qDebug() << ui->graphicsView->items();
    on_pushReset_clicked();

	QList<QGraphicsItem*> allGraphicsItems = ui->graphicsView->items();
	for(int i = 0; i < allGraphicsItems.size()-1; i++)
	{
		QGraphicsItem *graphicItem = allGraphicsItems[i];
		scene->removeItem(graphicItem);
		delete graphicItem;

	}
	//scene->update();	

	drawRain = false;
	ui->tableWidgetRain->setRowCount(0);
	totalRainLayer = 0;
	LAYER_RAIN.clear();
    drawSceneRegion();
}

void MainWindow::on_pushRainColor_clicked()
{
    QColor color = QColorDialog::getColor(rainColor, this,"", QColorDialog::ShowAlphaChannel);
    if( color.isValid() ) {
        rainColor = color;
        QString qss = QString("background-color: %1").arg(color.name());
        ui->labelRainColor->setStyleSheet(qss);
    }
}

void MainWindow::on_pushRainFogColor_clicked()
{
	QColor color = QColorDialog::getColor(fogRainColor, this, "");
	if( color.isValid() ) {
		fogRainColor = color;
        QString qss = QString("background-color: %1").arg(color.name());
        ui->labelRainFogColor->setStyleSheet(qss);
	}
}

void MainWindow::on_pushBokeh_clicked()
{
    fogBokehColor = QColor(fogBokehColor.red(),fogBokehColor.green(),fogBokehColor.blue(),ui->spinBokehFog->value());
    fogBrush.setStyle(Qt::SolidPattern);
    fogBrush.setColor(fogBokehColor);

    if (drawBokeh) scene->removeItem(fogRect);
    fogRect = scene->addRect(0, 0, width, height, Qt::NoPen, fogBrush);
    //scene->addItem(fogRect);
    drawBokeh = true;

	QBrush brush;
    brush.setStyle(Qt::SolidPattern);

	QPen pen;
	pen.setStyle(Qt::SolidLine);

    int deltaR = qAbs(bokehColor1.red() - bokehColor2.red());
    int deltaG = qAbs(bokehColor1.green() - bokehColor2.green());
    int deltaB = qAbs(bokehColor1.blue() - bokehColor2.blue());
    int deltaA = qAbs(bokehColor1.alpha() - bokehColor2.alpha());

    int X, Y;
    int bokehCount = ui->spinBokehCount->value();
    int N = ui->spinBokehSide->value();


	LAYER_BOKEH.append(new QGraphicsItemGroup());

    for (int j=0; j<bokehCount; j++) {

        int bokehSizeMin = ui->spinBokehSizeMin->value();
        int BokehSizeMax = ui->spinBokehSizeMax->value();
        int bokehSize = random() % (BokehSizeMax - bokehSizeMin + 1) + bokehSizeMin;

        X = random() % (width + bokehSize) - bokehSize;
        Y = random() % (height + bokehSize) - bokehSize;

        int bokehRotationMin = ui->spinBokehRotationMin->value();
        int BokehRotationMax = ui->spinBokehRotationMax->value();
        int bokehRotation = random() % (BokehRotationMax - bokehRotationMin + 1) + bokehRotationMin;

        int bokehBlurMin = ui->spinBokehBlurMin->value();
        int BokehBlurMax = ui->spinBokehBlurMax->value();
        int bokehBlur = random() % (BokehBlurMax - bokehBlurMin + 1) + bokehBlurMin;

        float bokehThicknessMin = ui->spinBokehThicknessMin->value();
        float BokehThicknessMax = ui->spinBokehThicknessMax->value();
        float bokehThickness = random() % (int)((BokehThicknessMax - bokehThicknessMin) * 100 + 1) / 100.0 + bokehThicknessMin;
        pen.setWidthF(bokehThickness);

        float bokehOpacityMin = ui->spinBokehOpacityMin->value();
        float BokehOpacityMax = ui->spinBokehOpacityMax->value();
        float bokehOpacity = random() % (int)((BokehOpacityMax - bokehOpacityMin) * 100 + 1) / 100.0 + bokehOpacityMin;

        float bokehEdgeOpacityMin = ui->spinBokehEdgeOpacityMin->value();
        float BokehEdgeOpacityMax = ui->spinBokehEdgeOpacityMax->value();
        float bokehEdgeOpacity = random() % (int)((BokehEdgeOpacityMax - bokehEdgeOpacityMin) * 100 + 1) / 100.0 + bokehEdgeOpacityMin;

		QGraphicsBlurEffect *blurEffect = new QGraphicsBlurEffect();
		if (highQuality) blurEffect->setBlurHints(QGraphicsBlurEffect::QualityHint);
		else blurEffect->setBlurHints(QGraphicsBlurEffect::PerformanceHint);
		blurEffect->setBlurRadius(bokehBlur);


        int R = random() % (deltaR + 1);
        R += qMin(bokehColor1.red(), bokehColor2.red());
        int G = random() % (deltaG + 1);
        G += qMin(bokehColor1.green(), bokehColor2.green());
        int B = random() % (deltaB + 1);
        B += qMin(bokehColor1.blue(), bokehColor2.blue());
        int A = random() % (deltaA + 1);
        A += qMin(bokehColor1.alpha(), bokehColor2.alpha());

//        qDebug () <<bokehEdgeOpacity*255;

		brush.setColor(QColor(R,G,B,A));
        pen.setColor(QColor(R,G,B,bokehEdgeOpacity*255));

        if (N >= 3) {
			QPolygonF poly;
            for(int i=0; i<N; i++) {
                poly << QPointF( X + bokehSize * qCos(i * 360.0 / N * 3.14 / 180.0) , Y + bokehSize * qSin(i * 360.0 / N * 3.14 / 180.0) );
			}

			QGraphicsPolygonItem *bokeh;
			bokeh = scene->addPolygon(poly, pen, brush);
            bokeh->setTransformOriginPoint(bokeh->boundingRect().center());
            bokeh->setRotation(bokehRotation);
			bokeh->setGraphicsEffect(blurEffect);
			bokeh->setOpacity(bokehOpacity);
			//group->addToGroup(bokeh);
			LAYER_BOKEH.at(totalBokehLayer)->addToGroup(bokeh);
		}
		else {
			ellipse = scene->addEllipse(X, Y, bokehSize, bokehSize, pen, brush);
			ellipse->setTransformOriginPoint(ellipse->rect().center());
			ellipse->setRotation(bokehRotation);
			ellipse->setGraphicsEffect(blurEffect);
			ellipse->setOpacity(bokehOpacity);
			//group->addToGroup(ellipse);
			LAYER_BOKEH.at(totalBokehLayer)->addToGroup(ellipse);
		}
	}
	scene->addItem(LAYER_BOKEH.at(totalBokehLayer));

	LAYER_BOKEH.at(totalBokehLayer)->setZValue(totalBokehLayer+1);

	ui->tableWidgetBokeh->setRowCount(totalBokehLayer + 1);
	QTableWidgetItem *itemShow = new QTableWidgetItem();
	QTableWidgetItem *itemLayer = new QTableWidgetItem();
	itemShow->setTextAlignment(Qt::AlignCenter);
	itemShow->setText("Hide");
	itemShow->setFlags(Qt::ItemIsEditable);

	itemLayer->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	itemLayer->setText("New Bokeh Layer " + QString::number(totalBokehLayer+1));


	ui->tableWidgetBokeh->setItem(totalBokehLayer,0,itemShow);
	ui->tableWidgetBokeh->setItem(totalBokehLayer,1,itemLayer);
	totalBokehLayer++;
//	ui->tableWidgetBokeh->viewport()->update();

}

void MainWindow::on_pushBokehColor1_clicked()
{
    QColor color = QColorDialog::getColor(bokehColor1, this,"", QColorDialog::ShowAlphaChannel);
	if( color.isValid() ) {
		bokehColor1 = color;
		QString qss = QString("background-color: %1").arg(color.name());
        ui->labelBokehColor1->setStyleSheet(qss);
	}
}

void MainWindow::on_pushBokehColor2_clicked()
{
    QColor color = QColorDialog::getColor(bokehColor2, this,"", QColorDialog::ShowAlphaChannel);
	if( color.isValid() ) {
		bokehColor2 = color;
		QString qss = QString("background-color: %1").arg(color.name());
        ui->labelBokehColor2->setStyleSheet(qss);
	}
}

void MainWindow::on_pushBokehFogColor_clicked()
{
    QColor color = QColorDialog::getColor(fogBokehColor, this, "");
    if( color.isValid() ) {
        fogBokehColor = color;
        QString qss = QString("background-color: %1").arg(color.name());
        ui->labelBokehFogColor->setStyleSheet(qss);
    }
}

void MainWindow::on_pushBokehClear_clicked()
{
    on_pushReset_clicked();

	QList<QGraphicsItem*> allGraphicsItems = ui->graphicsView->items();
	for(int i = 0; i < allGraphicsItems.size()-1; i++)
	{
		QGraphicsItem *graphicItem = allGraphicsItems[i];
		scene->removeItem(graphicItem);
		delete graphicItem;

	}
	//scene->update();

    drawBokeh = false;
	ui->tableWidgetBokeh->setRowCount(0);
	totalBokehLayer = 0;
	LAYER_BOKEH.clear();
	drawSceneRegion();
}

void MainWindow::on_pushRainLayerUp_clicked()
{
	int row = ui->tableWidgetRain->currentRow();
	if (row == -1 || row == 0) return;

	QTableWidgetItem *itemShow = ui->tableWidgetRain->takeItem(row, 0);
	QTableWidgetItem *itemLayer = ui->tableWidgetRain->takeItem(row, 1);
	QTableWidgetItem *itemUpShow = ui->tableWidgetRain->takeItem(row - 1, 0);
	QTableWidgetItem *itemUpLayer = ui->tableWidgetRain->takeItem(row - 1, 1);

	ui->tableWidgetRain->setItem(row - 1, 0, itemShow);
	ui->tableWidgetRain->setItem(row - 1, 1, itemLayer);
	ui->tableWidgetRain->setItem(row, 0, itemUpShow);
	ui->tableWidgetRain->setItem(row, 1, itemUpLayer);

	ui->tableWidgetRain->setCurrentCell(row - 1, 1);

	LAYER_RAIN.at(row)->setZValue(row);
	LAYER_RAIN.at(row - 1)->setZValue(row + 1);
}

void MainWindow::on_pushRainLayerDown_clicked()
{
	int row = ui->tableWidgetRain->currentRow();

	if (row == -1 || row == ui->tableWidgetRain->rowCount() - 1) return;

	QTableWidgetItem *itemShow = ui->tableWidgetRain->takeItem(row, 0);
	QTableWidgetItem *itemLayer = ui->tableWidgetRain->takeItem(row, 1);
	QTableWidgetItem *itemBelowShow = ui->tableWidgetRain->takeItem(row + 1, 0);
	QTableWidgetItem *itemBelowLayer = ui->tableWidgetRain->takeItem(row + 1, 1);

	ui->tableWidgetRain->setItem(row + 1, 0, itemShow);
	ui->tableWidgetRain->setItem(row + 1, 1, itemLayer);
	ui->tableWidgetRain->setItem(row, 0, itemBelowShow);
	ui->tableWidgetRain->setItem(row, 1, itemBelowLayer);

	ui->tableWidgetRain->setCurrentCell(row + 1, 1);

	LAYER_RAIN.at(row)->setZValue(row + 2);
	LAYER_RAIN.at(row + 1)->setZValue(row +1);

}

void MainWindow::on_pushRainLayerDelete_clicked()
{
	int row = ui->tableWidgetRain->currentRow();
	if (row < 0) return;

	scene->removeItem(LAYER_RAIN.at(row));
	LAYER_RAIN.removeAt(row);
	totalRainLayer--;

	QList<QTableWidgetItem*> selected_rows = ui->tableWidgetRain->selectedItems();

	while( !selected_rows.isEmpty() )
	{
		QTableWidgetItem *itm = selected_rows.at(0);
		ui->tableWidgetRain->removeRow(itm->row());
		selected_rows = ui->tableWidgetRain->selectedItems();
	}
	ui->tableWidgetRain->setFocus();

	if (row == 0) ui->tableWidgetRain->selectRow(0);
	else ui->tableWidgetRain->selectRow(row-1);
}

void MainWindow::on_toolBox_currentChanged(int index)
{
	if (index == 0) {
		ui->scrollAreaImage->setWidget(ui->frameImage);
		ui->scrollAreaImage->setGeometry(0,0,ui->pageImage->width(), ui->pageImage->height());
	}
	else if (index == 1) {
		ui->scrollAreaRain->setWidget(ui->frameRain);
		ui->scrollAreaRain->setGeometry(0,0,ui->pageRain->width(), ui->pageRain->height());
	}
	else if (index == 2) {
		ui->scrollAreaSnow->setWidget(ui->frameSnow);
		ui->scrollAreaSnow->setGeometry(0,0,ui->pageSnow->width(), ui->pageSnow->height());
	}
	else if (index == 3) {
		ui->scrollAreaBokeh->setWidget(ui->frameBokeh);
		ui->scrollAreaBokeh->setGeometry(0,0,ui->pageBokeh->width(), ui->pageBokeh->height());
	}

}

void MainWindow::on_sliderColorR_valueChanged(int value)
{
	colorCorrection();
}

void MainWindow::on_sliderColorG_valueChanged(int value)
{
	colorCorrection();
}

void MainWindow::on_sliderColorB_valueChanged(int value)
{
	colorCorrection();
}

void MainWindow::colorCorrection()
{
    if (filename == "") return;

	createHeader();

	colorCorrectionR = ui->sliderColorR->value();
	colorCorrectionG = ui->sliderColorG->value();
	colorCorrectionB = ui->sliderColorB->value();

	colorBrightness = ui->sliderBrightness->value();
	colorContrast = ui->sliderContrast->value();

	gamma = (500 - ui->sliderGamma->value() ) / 100.0;
	ui->spinGamma->setValue(gamma);

	int R, G, B;

	for (int i = 0; i < size; i += 3) {
		R = (quint8)bytes.at(i);
		G = (quint8)bytes.at(i+1);
		B = (quint8)bytes.at(i+2);

		R += colorCorrectionR;
		G += colorCorrectionG;
		B += colorCorrectionB;

		R += colorBrightness;
		G += colorBrightness;
		B += colorBrightness;

		float contrast = (259.0 * (colorContrast + 255)) / (255.0 * (259 - colorContrast));
		R = contrast * (R - 128) + 128;
		G = contrast * (G - 128) + 128;
		B = contrast * (B - 128) + 128;

//		double contrast = qPow((100.0 + colorContrast) / 100.0, 2);
//		R = (((((float)R / 255.0) - 0.5) * (float)contrast) + 0.5) * 255.0;
//		G = (((((float)G / 255.0) - 0.5) * (float)contrast) + 0.5) * 255.0;
//		B = (((((float)B / 255.0) - 0.5) * (float)contrast) + 0.5) * 255.0;

		R = 255 * qPow(R / 255.0, gamma);
		G = 255 * qPow(G / 255.0, gamma);
		B = 255 * qPow(B / 255.0, gamma);

		if (R < 0) R = 0;
		if (G < 0) G = 0;
		if (B < 0) B = 0;

		if (R > 255) R = 255;
		if (G > 255) G = 255;
		if (B > 255) B = 255;

		//qDebug() << R << G << B;
		data.append(R).append(G).append(B);
	}

	showImage(false);
}

void MainWindow::on_sliderGamma_valueChanged(int value)
{
	colorCorrection();
}

void MainWindow::on_sliderBrightness_valueChanged(int value)
{
	colorCorrection();
}

void MainWindow::on_sliderContrast_valueChanged(int value)
{
	colorCorrection();
}

void MainWindow::on_radioColorSmart_clicked()
{
	ui->stackedBokeh->setCurrentIndex(1);
	ui->sliderBokehArea->setEnabled(false);
	ui->spinBokehArea->setEnabled(false);
}

void MainWindow::on_radioColorClick_clicked()
{
	ui->stackedBokeh->setCurrentIndex(1);
	ui->sliderBokehArea->setEnabled(true);
	ui->spinBokehArea->setEnabled(true);
}

void MainWindow::on_radioColorManual_clicked()
{
	ui->stackedBokeh->setCurrentIndex(0);
}

void MainWindow::on_pushSnowFogColor_clicked()
{
	QColor color = QColorDialog::getColor(fogSnowColor, this, "");
	if( color.isValid() ) {
		fogSnowColor = color;
		QString qss = QString("background-color: %1").arg(color.name());
		ui->labelSnowFogColor->setStyleSheet(qss);
	}
}

void MainWindow::on_pushSnowLayerUp_clicked()
{
	int row = ui->tableWidgetSnow->currentRow();
	if (row == -1 || row == 0) return;

	QTableWidgetItem *itemShow = ui->tableWidgetSnow->takeItem(row, 0);
	QTableWidgetItem *itemLayer = ui->tableWidgetSnow->takeItem(row, 1);
	QTableWidgetItem *itemUpShow = ui->tableWidgetSnow->takeItem(row - 1, 0);
	QTableWidgetItem *itemUpLayer = ui->tableWidgetSnow->takeItem(row - 1, 1);

	ui->tableWidgetSnow->setItem(row - 1, 0, itemShow);
	ui->tableWidgetSnow->setItem(row - 1, 1, itemLayer);
	ui->tableWidgetSnow->setItem(row, 0, itemUpShow);
	ui->tableWidgetSnow->setItem(row, 1, itemUpLayer);

	ui->tableWidgetSnow->setCurrentCell(row - 1, 1);

	LAYER_SNOW.at(row)->setZValue(row);
	LAYER_SNOW.at(row - 1)->setZValue(row + 1);
}

void MainWindow::on_pushSnowLayerDown_clicked()
{
	int row = ui->tableWidgetSnow->currentRow();

	if (row == -1 || row == ui->tableWidgetSnow->rowCount() - 1) return;

	QTableWidgetItem *itemShow = ui->tableWidgetSnow->takeItem(row, 0);
	QTableWidgetItem *itemLayer = ui->tableWidgetSnow->takeItem(row, 1);
	QTableWidgetItem *itemBelowShow = ui->tableWidgetSnow->takeItem(row + 1, 0);
	QTableWidgetItem *itemBelowLayer = ui->tableWidgetSnow->takeItem(row + 1, 1);

	ui->tableWidgetSnow->setItem(row + 1, 0, itemShow);
	ui->tableWidgetSnow->setItem(row + 1, 1, itemLayer);
	ui->tableWidgetSnow->setItem(row, 0, itemBelowShow);
	ui->tableWidgetSnow->setItem(row, 1, itemBelowLayer);

	ui->tableWidgetSnow->setCurrentCell(row + 1, 1);


	LAYER_SNOW.at(row)->setZValue(row + 2);
	LAYER_SNOW.at(row + 1)->setZValue(row +1);
}

void MainWindow::on_pushSnowLayerDelete_clicked()
{
	int row = ui->tableWidgetSnow->currentRow();
	if (row < 0) return;

	QList<QGraphicsItem *> items;
	items = LAYER_SNOW.at(row)->childItems();

	for (int i=0; i<items.size(); i++) scene->removeItem(items.at(i));

	LAYER_SNOW.removeAt(row);
	totalSnowLayer--;


	QList<QTableWidgetItem*> selected_rows = ui->tableWidgetSnow->selectedItems();

	while( !selected_rows.isEmpty() )
	{
		QTableWidgetItem *itm = selected_rows.at(0);
		ui->tableWidgetSnow->removeRow(itm->row());
		selected_rows = ui->tableWidgetSnow->selectedItems();
	}
	if (row == 0) ui->tableWidgetSnow->selectRow(0);
	else ui->tableWidgetSnow->selectRow(row-1);
}

void MainWindow::on_pushBokehLayerUp_clicked()
{
	int row = ui->tableWidgetBokeh->currentRow();
	if (row == -1 || row == 0) return;

	QTableWidgetItem *itemShow = ui->tableWidgetBokeh->takeItem(row, 0);
	QTableWidgetItem *itemLayer = ui->tableWidgetBokeh->takeItem(row, 1);
	QTableWidgetItem *itemUpShow = ui->tableWidgetBokeh->takeItem(row - 1, 0);
	QTableWidgetItem *itemUpLayer = ui->tableWidgetBokeh->takeItem(row - 1, 1);

	ui->tableWidgetBokeh->setItem(row - 1, 0, itemShow);
	ui->tableWidgetBokeh->setItem(row - 1, 1, itemLayer);
	ui->tableWidgetBokeh->setItem(row, 0, itemUpShow);
	ui->tableWidgetBokeh->setItem(row, 1, itemUpLayer);

	ui->tableWidgetBokeh->setCurrentCell(row - 1, 1);

	LAYER_BOKEH.at(row)->setZValue(row);
	LAYER_BOKEH.at(row - 1)->setZValue(row + 1);

}

void MainWindow::on_pushBokehLayerDown_clicked()
{
	int row = ui->tableWidgetBokeh->currentRow();

	if (row == -1 || row == ui->tableWidgetBokeh->rowCount() - 1) return;

	QTableWidgetItem *itemShow = ui->tableWidgetBokeh->takeItem(row, 0);
	QTableWidgetItem *itemLayer = ui->tableWidgetBokeh->takeItem(row, 1);
	QTableWidgetItem *itemBelowShow = ui->tableWidgetBokeh->takeItem(row + 1, 0);
	QTableWidgetItem *itemBelowLayer = ui->tableWidgetBokeh->takeItem(row + 1, 1);

	ui->tableWidgetBokeh->setItem(row + 1, 0, itemShow);
	ui->tableWidgetBokeh->setItem(row + 1, 1, itemLayer);
	ui->tableWidgetBokeh->setItem(row, 0, itemBelowShow);
	ui->tableWidgetBokeh->setItem(row, 1, itemBelowLayer);

	ui->tableWidgetBokeh->setCurrentCell(row + 1, 1);


	LAYER_BOKEH.at(row)->setZValue(row + 2);
	LAYER_BOKEH.at(row + 1)->setZValue(row +1);

//qDebug() << bokehLayerIndex << LAYER_BOKEH.at(0)->zValue() << LAYER_BOKEH.at(1)->zValue();

}

void MainWindow::on_pushBokehLayerDelete_clicked()
{
	int row = ui->tableWidgetBokeh->currentRow();
	if (row < 0) return;

//	qDebug() << ui->tableWidgetBokeh->currentRow();
	scene->removeItem(LAYER_BOKEH.at(row));
	LAYER_BOKEH.removeAt(row);
	totalBokehLayer--;


	QList<QTableWidgetItem*> selected_rows = ui->tableWidgetBokeh->selectedItems();

	while( !selected_rows.isEmpty() )
	{
		QTableWidgetItem *itm = selected_rows.at(0);
		ui->tableWidgetBokeh->removeRow(itm->row());
		selected_rows = ui->tableWidgetBokeh->selectedItems();
	}
	if (row == 0) ui->tableWidgetBokeh->selectRow(0);
	else ui->tableWidgetBokeh->selectRow(row-1);

}

void MainWindow::on_tableWidgetRain_cellClicked(int row, int column)
{

	if (column == 0) {

		if (ui->tableWidgetRain->item(row, 0)->text() == "Hide") {
			ui->tableWidgetRain->item(row, 0)->setText("Show");
			LAYER_RAIN.at(row)->hide();
		}
		else {
			ui->tableWidgetRain->item(row, 0)->setText("Hide");
			LAYER_RAIN.at(row)->show();
		}

	}

}

void MainWindow::on_tableWidgetSnow_cellClicked(int row, int column)
{

	if (column == 0) {

		if (ui->tableWidgetSnow->item(row, 0)->text() == "Hide") {
			ui->tableWidgetSnow->item(row, 0)->setText("Show");
			LAYER_SNOW.at(row)->hide();
		}
		else {
			ui->tableWidgetSnow->item(row, 0)->setText("Hide");
			LAYER_SNOW.at(row)->show();
		}

	}
}

void MainWindow::on_tableWidgetBokeh_cellClicked(int row, int column)
{

	if (column == 0) {

		if (ui->tableWidgetBokeh->item(row, 0)->text() == "Hide") {
			ui->tableWidgetBokeh->item(row, 0)->setText("Show");
			LAYER_BOKEH.at(row)->hide();
		}
		else {
			ui->tableWidgetBokeh->item(row, 0)->setText("Hide");
			LAYER_BOKEH.at(row)->show();
		}

	}
}

void MainWindow::on_pushRealBokeh1_clicked()
{
	if (filename == "") return;
	QTime timer; timer.start();

		QGraphicsScene *sceneBokeh = new QGraphicsScene();
		ui->graphicsViewRealBokeh->setScene(sceneBokeh);
//		sceneBokeh->setSceneRect(0,0,64,64);

		QPen pen;
		pen.setColor(Qt::white);
		pen.setStyle(Qt::SolidLine);
		pen.setWidth(2);

	//	QRadialGradient gradient(42, 42, 10);
	//	gradient.setColorAt(0, QColor(0,0,0,0));
	//	gradient.setColorAt(0.8, QColor(0,0,0,0));
	//	gradient.setColorAt(1, Qt::white);
	//	QBrush brush(gradient);

		QBrush brush;
		brush.setStyle(Qt::SolidPattern);
		if (ui->checkBoxRealBokehSharp->isChecked()) brush.setColor(Qt::gray);
		else brush.setColor(Qt::white);


		int n = ui->spinRealBokehN->value();
		int S = ui->spinRealBokehS->value();
		int startX = 0, startY = 0;

		if (n <= 1) QGraphicsEllipseItem *eclipse = sceneBokeh->addEllipse(startX, startY, S, S, pen, brush);
		else if (n == 2) {
			QPainterPath* path = new QPainterPath();
			path->moveTo(startX, startY);
			path->quadTo(startX + S, startY - S, startX + 2*S, startY);
			path->quadTo(startX + S, startY + S, startX, startY);
			sceneBokeh->addPath(*path, Qt::NoPen, brush);
		}
		else {
			QPolygonF poly;
			for(int i=0; i<n; i++) {
				poly << QPointF( startX + S * qCos(i * 360.0 / n * 3.14 / 180.0) , startY + S * qSin(i * 360.0 / n * 3.14 / 180.0) );
			}

			QGraphicsPolygonItem *bokeh;
			bokeh = sceneBokeh->addPolygon(poly, Qt::NoPen, brush);
		}

		int sceneHeight = sceneBokeh->itemsBoundingRect().height();
		int sceneWidth = sceneBokeh->itemsBoundingRect().width();

		sceneBokeh->setSceneRect(sceneBokeh->itemsBoundingRect());
		QImage image(sceneWidth, sceneHeight, QImage::Format_Grayscale8);
		image.fill(Qt::black);
		QPainter painter(&image);
	//	painter.setRenderHint(QPainter::Antialiasing, true);
		sceneBokeh->render(&painter);
	//	image.save("C:/Users/Hakan/QT_Compression/GUI_Encode1/bmp/test/hexagon_64___.png");

		//qDebug() << sceneBokeh->itemsBoundingRect().width() << sceneBokeh->itemsBoundingRect().height();


		QColor color;

		int MAX = qMax(sceneWidth, sceneHeight);
		if (MAX % 2 != 1) MAX++;

		quint8** colorMatris = new quint8*[MAX];
		for(int i = 0; i < MAX; ++i) colorMatris[i] = new quint8[MAX];

		for (int i=0; i < MAX; ++i) {
			for (int j=0; j < MAX; ++j) {
				color = image.pixel(j,i); // row-column
				colorMatris[i][j] = color.value();
			}
		}

//		qDebug () << sceneWidth << sceneHeight << MAX;
		int margin = (MAX - 1) / 2;

		QVector < QVector <quint8> > R; QVector <quint8> RVal;
		QVector < QVector <quint8> > G; QVector <quint8> GVal;
		QVector < QVector <quint8> > B; QVector <quint8> BVal;
		quint32 counter = 0;
		quint8 R_, G_, B_;

		for (int i=0; i<height + 2*margin; ++i) {
			for (int j=0; j<width + 2*margin; ++j) {

				if (i < margin || j < margin || i >= height + margin || j >= width + margin) {
					R_ = 0; G_ = 0; B_ = 0;
				}
				else {
					R_ = bytes.at(counter);
					G_ = bytes.at(counter + 1);
					B_ = bytes.at(counter + 2);
					counter += 3;
					//qDebug()<< R << G << B;
				}

	//			qDebug () << i << j << R_;

				RVal.append(R_);
				GVal.append(G_);
				BVal.append(B_);
			}
			//qDebug()<< YVal << UVal << VVal;
			R.append(RVal); G.append(GVal); B.append(BVal);
			RVal.clear(); GVal.clear(); BVal.clear();
		}

	//	qDebug () << R.at(0).size() << G.at(0).size() << B.at(0).size();

			/////////////////////////////////////
				scene->clear();
				QByteArray widthStr, heightStr;
				widthStr = QByteArray::number(width + 2*margin);
				heightStr = QByteArray::number(height + 2*margin);

				// Creating PPM format header
				data.append("P6 ");
				data.append(widthStr).append(" ");
				data.append(heightStr).append(" ");
				data.append("255 ");
				headerSize = 9 + widthStr.size() + heightStr.size();
			/////////////////////////////////////


		float valR, valG, valB;
		float spinBokeh = ui->doubleSpinRealBokeh->value();
		int limit = ui->realBokehLimit->value();
		int counterR, counterG, counterB;



		for (int i=0; i<height + 2*margin; ++i) {
			for (int j=0; j<width + 2*margin; ++j) { //qDebug () << i << j;


				if (i < margin || j < margin || i >= height + margin || j >= width + margin) {
					data.append(R.at(i).at(j)).append(G.at(i).at(j)).append(B.at(i).at(j));
				}
				else {
					valR = 0, valG = 0, valB = 0;
					counterR = 0, counterG = 0, counterB = 0;

						for (int x=0; x<sceneHeight-2; ++x) {
							for (int y=0; y<sceneWidth-2; ++y) {

								valR += R.at(i+x-margin).at(j+y-margin) * colorMatris[x][y];
								if (R.at(i+x-margin).at(j+y-margin) < limit) counterR += colorMatris[x][y];

								valG += G.at(i+x-margin).at(j+y-margin) * colorMatris[x][y];
								if (G.at(i+x-margin).at(j+y-margin) < limit) counterG += colorMatris[x][y];

								valB += B.at(i+x-margin).at(j+y-margin) * colorMatris[x][y];
								if (B.at(i+x-margin).at(j+y-margin) < limit) counterB += colorMatris[x][y];

	//							qDebug () << i+x << j+y;

							}
						}
	//					qDebug () << valR << counterR << valG << counterG << valB << counterB;
						valR /= (counterR * spinBokeh); valG /= (counterG * spinBokeh); valB /= (counterB * spinBokeh);

						if (valR > 255) valR = 255; if (valG > 255) valG = 255; if (valB > 255) valB = 255;
	//					if (valR < 0) valR = 0; if (valG < 0) valG = 0; if (valB < 0) valB = 0;

					data.append(valR).append(valG).append(valB);
				}

	//			qDebug () << i << j << G.at(i).at(j) << B.at(i).at(j);

			}
		}

		//qDebug () << data.size();
		R.clear(); G.clear(); B.clear();
		showImage(false);

		qDebug () << timer.elapsed();

}

void MainWindow::on_pushRealBokeh2_clicked()
{
	if (filename == "") return;

	float limit = ui->limit->value();
	int blur = ui->blur->value();
	int bokehSize = ui->bokehSize->value();

	scene->clear();

	QImage imageObject;
	imageObject.load(filename);
	imageObject = imageObject.convertToFormat(QImage::Format_ARGB32);

	QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(imageObject));

	QGraphicsBlurEffect *blurEffect = new QGraphicsBlurEffect();
	blurEffect->setBlurHints(QGraphicsBlurEffect::QualityHint);
	//blurEffect->setBlurHints(QGraphicsBlurEffect::PerformanceHint);
	blurEffect->setBlurRadius(blur);

	item->setGraphicsEffect(blurEffect);
	scene->addItem(item);


	QPen pen;
//	pen.setColor(Qt::white);
	pen.setStyle(Qt::SolidLine);
	pen.setWidthF(1.5);

	QBrush brush;
	brush.setStyle(Qt::SolidPattern);
//	brush.setColor(Qt::white);


	QVector < QVector <quint8> > R; QVector <quint8> RVal;
	QVector < QVector <quint8> > G; QVector <quint8> GVal;
	QVector < QVector <quint8> > B; QVector <quint8> BVal;
	quint32 counter = 0;
	quint8 R_, G_, B_;

	for (int i=0; i<height; i++) {
		for (int j=0; j<width; j++) {
			R_ = bytes.at(counter);
			G_ = bytes.at(counter + 1);
			B_ = bytes.at(counter + 2);
			counter += 3;
			//qDebug()<< R << G << B;

			RVal.append(R_);
			GVal.append(G_);
			BVal.append(B_);
		}
		//qDebug()<< YVal << UVal << VVal;
		R.append(RVal); G.append(GVal); B.append(BVal);
		RVal.clear(); GVal.clear(); BVal.clear();
	}


	float valR, valG, valB;
	int counterR, counterG, counterB;
	int RMax, GMax, BMax;
	int color;
	bool state = false;
	QGraphicsEllipseItem *ellipse;
//	QGraphicsItemGroup *group = new QGraphicsItemGroup();

	int counterX = 0, counterY = 0;

	for (int i=0; i<height; i++) {

		for (int j=0; j<width; j++) {

			color = R.at(i).at(j) * .299 + G.at(i).at(j) * .587 + B.at(i).at(j) * .114;
			if (color >= limit) {
				state = true;

				pen.setColor(QColor(R.at(i).at(j), G.at(i).at(j), B.at(i).at(j)));
				brush.setColor(QColor(R.at(i).at(j), G.at(i).at(j), B.at(i).at(j)));
				ellipse = scene->addEllipse(j - bokehSize / 2, i - bokehSize / 2, bokehSize, bokehSize, pen, brush);
				ellipse->setOpacity((color-limit) / (255-limit) - 0.3);
				ellipse->setZValue(color);

				QGraphicsBlurEffect *blurEffect1 = new QGraphicsBlurEffect();
				blurEffect1->setBlurHints(QGraphicsBlurEffect::QualityHint);
				blurEffect1->setBlurRadius( (1-(color-limit) / (255-limit)) * 2 + 1); //qDebug() << (1-(color-limit) / (255-limit)) * 2 + 2;
				ellipse->setGraphicsEffect(blurEffect1);
//				group->addToGroup(ellipse);
			j += random() % (ui->XY->value() + 1);
			}
		}
		if (state) { i += random() % (ui->XY->value() + 1); state = false; }
	}

//	scene->addItem(group);
//	qDebug () << X.size();
	R.clear(); G.clear(); B.clear();
}


