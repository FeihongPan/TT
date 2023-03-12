/*************************************************************************
*
* TURBOTIDES
* An Integrated CAE Platform for Turbomachinery Design and Development
* ____________________________________________________________________
*
*  [2016] - [2021]  TaiZe Inc
*  All Rights Reserved.
*
* NOTICE:  All information contained herein is, and remains
* the property of TaiZe Inc and its suppliers, if any.
* The intellectual and technical concepts contained herein
* are proprietary to TaiZe Inc and its suppliers and may
* be covered by U.S. and Foreign Patents, patents in proces
* and are protected by trade secret or copyright law.
* Dissemination of this information or reproduction of this
* material is strictly forbidden unless prior written
* permission is obtained from TaiZe Inc.
*/

#include "exp_BladeImport.h"
#include "core_Application.h"
#include "w_TModule.h"
#include "w_PropertyHolderDialog.h"
#include "w_PropertyHolderWidget.h"
#include "plt_SurfaceGraphWidget.h"


#include <w_TTWidget.h>
#include "w_TableInput.h"
#include "w_PropertyDoubleTable.h"
REGISTER_OBJECT_CLASS(exp_BladeImport, "BladeImport", TObject);
REGISTER_OBJECT_CLASS_DISPLAY_NAME(exp_BladeImport, "BladeImport");

exp_BladeImport::exp_BladeImport(QString object_n, TObject *iparent) :TObject(object_n, iparent)
{
	INIT_OBJECT;

	setTag("MethodOrder", "BladePlot,"
		"BladePlotX," "BladeSplit,"
		"$");
	//DEFINE_QSTRINGLIST_INIT(fileNameList, 0, 0, NULL);
	DEFINE_QINT_VECTOR_INIT(splitNumList, 0, 0, NULL, NULL);
	DEFINE_QINT_VECTOR_INIT(TotNumList, 0, 0, NULL, NULL);
}


QStringList exp_BladeImport::getCSV(QString fileName)
{
QStringList nameList;

QFile file(fileName);
if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
{
	eprintf("Open failed %s", fileName);
}
QString mStr;

QTextStream in(&file);
int i = 0;
while (!in.atEnd())
{
	in >> mStr;
	if (mStr.size()<4) { continue; };
	//eprintf("read file  is %s and row=%d and size=%d", mStr.ascii(),i, mStr.size());
	i++;
	nameList.push_back(mStr);
}

return nameList;

}

QVector<QVector<QString>> exp_BladeImport::getCSV(QString fileName, QString seprator)
{
QVector<QVector<QString>> txt;
QString mStr;
QStringList lStr;
double num=0;
QVector<QString> single_txt;


QFile file(fileName);

if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
{
	eprintf("Open failed %s", fileName.ascii());
}
QTextStream in(&file);
int i = 0;
while (!in.atEnd())
{
	in >> mStr;
	lStr = mStr.split(seprator);
	if (lStr.size() < 3)
	{
		break;//因为会读取到最后一行，最后一行是没有数据的。如果按照逗号拆分，数据小于3的话说明数据有问题
	}
	for each (QString var in lStr)
	{
		single_txt.push_back(var);
	}
	//eprintf("i=%d, mStr=%s", i,mStr.ascii());
	i++;
	txt.push_back(single_txt);
	single_txt.clear();
}

return txt;

}

void exp_BladeImport::putCSV(QString fileName, QVector<QVector<QString>> txt,int row,int col)
{
QFile aFile(fileName);
QString sTxt;
QByteArray bTxt;
if (!aFile.open(QIODevice::WriteOnly | QIODevice::Text))
{
	eprintf("write failed");

}
for (int i = 0; i < row; i++)
{
	for (int j = 0; j < col; j++)
	{
		sTxt.push_back(txt[i][j]);
		if (j < col - 1){sTxt.push_back(","); }
		else
			sTxt.push_back("\n");
	}
		
	bTxt = sTxt.toUtf8();
		
	sTxt.clear();
	aFile.write(bTxt);
		
}
aFile.close();
}

void exp_BladeImport::convertXYZFromZ_RTheta_R(QVector<QVector<QString>> &txt, int row)
{

double theta;
double xValue;
double yValue;
double zValue;
const int mFactor = 1;
for (int i = 0; i < row; i++)
{
	theta = txt[i][1].toDouble() / txt[i][2].toDouble();
	xValue = txt[i][2].toDouble()*sin(theta)*mFactor;
	yValue= txt[i][2].toDouble()*cos(theta)*mFactor;
	zValue= txt[i][0].toDouble()*mFactor;
	txt[i][0] = QString::number(xValue, 'f', 10);
	txt[i][1] = QString::number(yValue, 'f', 10);
	txt[i][2] = QString::number(zValue, 'f', 10);
}
	
}

void exp_BladeImport::convertXYZFromZ_R_Theta(QVector<QVector<QString>> &txt, int row)
{

double theta;
double xValue;
double yValue;
double zValue;
const int mFactor = 1;
for (int i = 0; i < row; i++)
{
	theta = txt[i][2].toDouble();
	xValue = txt[i][2].toDouble()*sin(theta)*mFactor;
	yValue= txt[i][2].toDouble()*cos(theta)*mFactor;
	zValue= txt[i][0].toDouble()*mFactor;
	txt[i][0] = QString::number(xValue, 'f', 10);
	txt[i][1] = QString::number(yValue, 'f', 10);
	txt[i][2] = QString::number(zValue, 'f', 10);
}
	
}

void exp_BladeImport::convertXYZFromXYZ(QVector<QVector<QString>> &txt, int row)
{

//double theta;
double xValue;
double yValue;
double zValue;
const int mFactor = 1;
for (int i = 0; i < row; i++)
{
	//theta = txt[i][2].toDouble();
	xValue = txt[i][0].toDouble()*mFactor;
	yValue= txt[i][1].toDouble()*mFactor;
	zValue= txt[i][2].toDouble()*mFactor;
	txt[i][0] = QString::number(xValue, 'f', 10);
	txt[i][1] = QString::number(yValue, 'f', 10);
	txt[i][2] = QString::number(zValue, 'f', 10);
}
	
}

QString exp_BladeImport::getFileStem(QString fileName) 
{
	QString steName = fileName.replace("\\", "/").split("/").last();
	return steName;
}

QVector<QVector<double>> exp_BladeImport::str2doubleVector(QVector<QVector<QString>>&txt, int row,int col)
{
	QVector<QVector<double> > mrow;
	double x;
	double y;
	double z;
	for (int i = 0; i < row; i++)
	{
		
		QVector<double> point;
		x = txt[i][0].toDouble();
		y = txt[i][1].toDouble();
		z = txt[i][2].toDouble();
		
		point.push_back(x);
		point.push_back(y);
		point.push_back(z);
		mrow.push_back(point);
	}
	return mrow;
}

QVector<QVector<Double3>> exp_BladeImport::splitCurve(QVector<QVector<Double3>> profiles,int profileSize, int spliteFactor)
{
	QVector<QVector<Double3>> points;
	//Double3 point;
	QVector<Double3> point;
	for (int i = 0; i < profileSize; i++)
	{
		for (int j = 0; j < spliteFactor; j++)
		{
			point.push_back(profiles[i][j]);
		}
		points.push_back(point);
		point.clear();
	}

	return points;
}

int getminLength(Double2 xy, QVector<Double2> points);
void getminLength(Double2 xy, QVector<Double2> points, int& fIndex, double& fLength);
double getLength(Double2 xy, Double2 xynew);
double getR(Double2 xy);
double getR(double x, double y);
double findInsection(void);
void initm2(QVector<Double2>& points);
void initm3(QVector<Double3>& points);
#include "vis_Widget.h"
#include "w_Property.h"
#include "pe_TPropertyInputWidget.h"
//#include "w_VectorPropertyWidgetH.h"
#include "w_VectorPropertyWidgetV.h"
O_METHOD_BEGIN(exp_BladeImport, BladePlot, "01-Blade plot", 0, 0, "")
{
	
	if (VARLID_ARG)
	{

		exp_BladeImport* mSlot;
		QVector<property_t*> properties;
		TObject* o = dynamic_cast<TObject*>(object);

		QString fileName;
		w_PropertyHolderDialog dlg;
		dlg.setWindowTitle("Blade Plot");

		w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 2, "Input data");
		w_PropertyHolderWidget* holder1 = holder->getHolder(0, 0, 1, 2, "config");
		
		w_Property* wGetFilName = holder1->addProperty(&fileName, "File path", 0, false,
			false, false, "TCallbackOpenFileBrowser");
		wGetFilName->setToolTip("Input the full file path");
		QString fileName_read = "fileName";
		QStringList fileList;
		//w_Property* mfileName = holder1->addProperty(&fileName_read, "read fileName");

		QStringList dataType = QStringList() << "Z_R*Theta_R"
			<< "Z_R_Theta" << "X_Y_Z";
		QString selection = dataType[0];
		
		/*w_Property* w_sel = holder->addProperty(&selection, "Data type",
			&dataType,false,false,w_Property::VLayout);*/

		w_Property* w_sel = holder1->addProperty(&selection, "Data type",
			&dataType, false, false, true, "",
			false, w_Property::VLayout);

		w_QPushButton* apply = holder1->addButton("Click to plot", 3, 0, 1, 2);

		
		QVector<QVector<QString>> txt;
		QString sName = "csvFile";
		QVector<QVector<QVector<double> > > mdata;

		vis_Widget* ww = nullptr;
		w_Property* w_sels = nullptr;
		w_VectorPropertyWidget* mTable = nullptr;
		QObject::connect(apply, &w_QPushButton::clicked, &dlg, [ &sName, &mdata, &holder,
			wGetFilName,&apply, &fileList, mSlot, &txt,&dlg,&w_sel,dataType,&holder1,&ww,&w_sels,
			o, &mTable,&properties]()
			{	
				o->property("splitNumList")->asIntVector().clear();
				QString selected =  w_sel->getValue().toString();
				if (selected == dataType[0])
				{
					eprintf("selection is Z_R*Theta_R");
				}
				else if (selected == dataType[1])
				{ eprintf("selection is Z_R_Theta");}
				else if (selected == dataType[2])
				{ eprintf("selection is X_Y_Z");}
				fileList = mSlot->getCSV(wGetFilName->getValue().toString());
				QStringList fileSteList;
				QVector<QVector<double> >  sdata;
				QVector<QVector<QVector<double> > > data;

				QVector<QVector<Double3>> profiles;
				Double3 x;
				QVector<QVector<double>> values;
				QString sFileList;
				//QMap<QString, QVariant> args =
				//{
				//  {"colorName", "lightBlue"},
				//  {"lineWidth", 1},
				//  //{"showMode", "Edge+Face"}
				//};
				for each (QString var in fileList)
				{
					txt = mSlot->getCSV(var, ",");
					sFileList.push_back(var);
					sFileList.push_back("\n");
					if (selected == "Z_R*Theta_R")
					{mSlot->convertXYZFromZ_RTheta_R(txt, txt.size()); }
					else if (selected == "Z_R_Theta")
					{mSlot->convertXYZFromZ_R_Theta(txt, txt.size());}
					else if (selected == "X_Y_Z")
					{mSlot->convertXYZFromXYZ(txt, txt.size());}
					sName = var;
					sName.replace(".csv", "_convert.csv");
					QVector<Double3> L;
					QVector<double> V;

					mSlot->putCSV(sName, txt, txt.size(), txt[0].size());
					sdata = mSlot->str2doubleVector(txt, txt.size(), txt[0].size());
					for (int i = 0; i < sdata.size(); i++)
					{
						x[0] = sdata[i][0];
						x[1] = sdata[i][1];
						x[2] = sdata[i][2];
						L.push_back(x);
						V.push_back(x[2]);
					}
					profiles.push_back(L);
					values.push_back(V);
					mdata.push_back(sdata);
					mdata.clear();
					//QString NamesX = mSlot->getFileStem(var);
					//mSlot->fileNameList.push_back(NamesX);
					fileSteList.push_back(mSlot->getFileStem(var));
					o->property("splitNumList")->asIntVector().push_back(sdata.size() / 2);
					
				}
				w_PropertyHolderWidget* holder2 = holder->getHolder(0, 2, 1, 8, "Blade plot");
				if (ww)
				{
					ww->clear();
				}
				else
				{ 
					ww = vis_Widget::newWidget("vis_WidgetVtk", holder2);
				}
				ww->displaySurfaceFromProfiles("Imp profiles", profiles, &values);
				holder2->placeWidget(ww);
				/*
				if (vis_Widget* ww = vis_Widget::newWidget("vis_WidgetVtk", holder2))
				{
					//ww->setSizeHint(QSize(800, 600));
					ww->displaySurfaceFromProfiles("Imp profiles", profiles, &values);
					holder2->placeWidget(ww);
					w_sel->setDisabled(true);
				}
				*/
				//apply->setHidden(true);
				/*
				if (w_sels)
				{

				}
				else
				{
					w_sels = holder1->addProperty(&fileList[0], "selected",
						&fileList, false, false, false, "",
						true, w_Property::VLayout);
				}
				/*w_Property* w_sels = holder1->addProperty(&fileList[0], "selected",
				&fileList, false, false, false, "",
				true, w_Property::VLayout);
				w_sels->setDisabled(true);*/
				if (mTable)
				{
					mTable->setProperties(properties, &fileSteList);
					holder1->placeWidget(mTable, 4, 0, 1, 2);
					holder1->updateShow();
		
				}
				else
				{
					dlg.maximumSize();
					mTable = new w_VectorPropertyWidgetV(holder1);
					properties.push_back(o->property("splitNumList"));
					mTable->setProperties(properties, &fileSteList);
					holder1->placeWidget(mTable, 4, 0, 1, 2);
					holder1->updateShow();
					o->property("splitNumList");

				}

			}
		);

		if (dlg.exec() == QDialog::Accepted)
		{
		}

	}
}
O_METHOD_END;

#include "vis_Widget.h"
O_METHOD_BEGIN(exp_BladeImport, BladePlotX, "02-Blade plotX", 0, 0, "")
{

	if (VARLID_ARG)
	{
		QString fileName;
		w_PropertyHolderDialog dlg;
		dlg.setWindowTitle("Blade Plot");

		w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 2, "Input data");
		w_PropertyHolderWidget* holder1 = holder->getHolder(0, 0, 2, 2, "config");

		w_Property* wGetFilName = holder1->addProperty(&fileName, "File path", 0, false,
			false, false, "TCallbackOpenFileBrowser");
		wGetFilName->setToolTip("Input the full file path");




		QString fileName_read = "fileName";
		QStringList fileList;
		//w_Property* mfileName = holder1->addProperty(&fileName_read, "read fileName");

		QStringList dataType = QStringList() << "Z_R*Theta_R"
			<< "Z_R_Theta" << "X_Y_Z";
		QString selection = dataType[0];
		w_Property* w_sel = holder1->addProperty(&selection, "Data type",
			&dataType, false, false, false, "",
			false, w_Property::VLayout);
		w_QPushButton* apply = holder1->addButton("Click to plot", 3, 0, 1, 2);

		exp_BladeImport* mSlot;
		QVector<QVector<QString>> txt;
		QString sName = "csvFile";
		
		w_QPushButton* plt2 = holder1->addButton("Click to plot split", 4, 0, 1, 2);
		plt2->hide();
	
		
		
		
		QVector<QVector<Double3>> newPoints;
		int dataNum = 0;
		w_Property* s_spliteNum = holder1->addProperty(&dataNum, "Splite number");
		s_spliteNum->hideAll(true);
		QVector<QVector<Double3>> profiles;
		QVector<QVector<double>> values;
		QObject::connect(apply, &w_QPushButton::clicked, &dlg, [&sName, &holder,
			wGetFilName, &apply, &fileList, mSlot, &txt, &dlg, &w_sel, &profiles,&values,
			dataType, &holder1, plt2,s_spliteNum]()
			{
				QVector<Double3> L;
				QVector<double> V;
				
				QString selected = w_sel->getValue().toString();
				if (selected == dataType[0])
				{
					eprintf("selection is Z_R*Theta_R");
				}
				else if (selected == dataType[1])
				{
					eprintf("selection is Z_R_Theta");
				}
				else if (selected == dataType[2])
				{
					eprintf("selection is X_Y_Z");
				}
				txt = mSlot->getCSV(wGetFilName->getValue().toString(),",");
				QVector<QVector<double> >  sdata;
				Double3 x;
				
				if (selected == "Z_R*Theta_R")
				{
					mSlot->convertXYZFromZ_RTheta_R(txt, txt.size());
				}
				else if (selected == "Z_R_Theta")
				{
					mSlot->convertXYZFromZ_R_Theta(txt, txt.size());
				}
				else if (selected == "X_Y_Z")
				{
					mSlot->convertXYZFromXYZ(txt, txt.size());
				}
				sdata = mSlot->str2doubleVector(txt, txt.size(), txt[0].size());
				for (int i = 0; i < sdata.size(); i++)
				{
					x[0] = sdata[i][0];
					x[1] = sdata[i][1];
					x[2] = sdata[i][2];
					L.push_back(x);
					//V.push_back(x[2]);
					V.push_back(1);
				}
				profiles.push_back(L);
				values.push_back(V);
				L.clear();
				V.clear();
				for (int i = 0; i < sdata.size(); i++)
				{
					x[0] = sdata[i][0];
					x[1] = sdata[i][1];
					x[2] = 1.0+sdata[i][2];
					L.push_back(x);
					V.push_back(1-fabs(x[2]));
				}
				profiles.push_back(L);
				values.push_back(V);
				w_PropertyHolderWidget* holder2 = holder->getHolder(3, 0, 7, 2, "Blade plot");
				if (vis_Widget* ww = vis_Widget::newWidget("vis_WidgetVtk", holder2))
				{
					//ww->setSizeHint(QSize(800, 600));
					ww->displaySurfaceFromProfiles("Imp profiles", profiles, &values);
					//ww->displaySurfaceFromProfiles("Imp profiles", profiles);
					
					holder2->placeWidget(ww);
					w_sel->setDisabled(true);
				}
				apply->setHidden(true);
				//QLabel* dataNum = new QLabel("dataNumber");
				//QLabel* dataNum = holder1->addLabel("dataNumber");
				//dataNum->setNum(sdata.size());
				int dataNum = sdata.size();
				w_Property* pdataNum = holder1->addProperty(&dataNum, "dataNumber");
				pdataNum->setEnabled(false);
				plt2->show();
				
				//s_spliteNum->setDisplayName("Splite number");
				//s_spliteNum->show();
				s_spliteNum->setValue(dataNum / 2);
				s_spliteNum->hideAll(false);
				
			}

		);
		vis_Widget* ww2 = nullptr;
		QObject::connect(plt2, &w_QPushButton::clicked, 
			[mSlot, &holder, s_spliteNum, &profiles, &values,&ww2]
			{
				w_PropertyHolderWidget* holder3 = holder->getHolder(0, 2, 10, 8, "Blade plotX");
	
				QVector<QVector<Double3>>  newPoints = mSlot->splitCurve(profiles, profiles.size(), s_spliteNum->getValue().toInt());
				if (ww2)
				{
					ww2->clear();
				}
				else
				{
					ww2 = vis_Widget::newWidget("vis_WidgetVtk", holder3);
				}
				
				ww2->displaySurfaceFromProfiles("splite profiles", newPoints, &values);
				holder3->placeWidget(ww2);
		
			}
		);
		if (dlg.exec() == QDialog::Accepted)
		{
		}

	}
}
O_METHOD_END;


#include "vis_Widget.h"
#include "w_Property.h"
#include "pe_TPropertyInputWidget.h"
//#include "w_VectorPropertyWidgetH.h"
#include "w_VectorPropertyWidgetV.h"
O_METHOD_BEGIN(exp_BladeImport, BladeSplit, "03-Blade split", 0, 0, "")
{

	if (VARLID_ARG)
	{

		exp_BladeImport* mSlot;
		QVector<property_t*> properties;
		TObject* o = dynamic_cast<TObject*>(object);

		QString fileName;
		QStringList steNameList;
		w_PropertyHolderDialog dlg;
		dlg.setWindowTitle("Blade Plot");

		w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 2, "");
		w_PropertyHolderWidget* holder1 = holder->getHolder(0, 0, 1, 2, "config");

		w_Property* wGetFilName = holder1->addProperty(&fileName, "File path", 0, false,
			false, false, "TCallbackOpenFileBrowser");
		wGetFilName->setToolTip("Input the full file path");
		
		QStringList dataType = QStringList() << "Z_R*Theta_R"
			<< "Z_R_Theta" << "X_Y_Z";
		QString selection = dataType[0];
		w_Property* w_sel = holder1->addProperty(&selection, "Data type",
			&dataType, false, false, true, "",
			false, w_Property::VLayout);

		w_QPushButton* pltImp = holder1->addButton("plot impeller", 2, 0, 1, 1);
		w_QPushButton* pltSS = holder1->addButton("plot SS", 2, 1, 1, 1);

		pltSS->hide();

		QVector<QVector<QString>> txt;//single txt data
		QVector<QVector<QVector<double> > > mdata;

		vis_Widget* bldPlt = nullptr;//blade plot
		w_Property* w_sels = nullptr;//data type
		w_VectorPropertyWidget* mTable = nullptr;//table
		QVector<QVector<Double3>> profiles;//blade all data
		QVector<QVector<double>> zColors;// the colors
		QObject::connect(pltImp, &w_QPushButton::clicked, &dlg, [&holder,
			wGetFilName, &pltImp, mSlot, &txt, &dlg, &w_sel, dataType, &holder1, &bldPlt, &w_sels,
			o, &mTable, &properties,&steNameList,&pltSS,&profiles, &zColors]()
			{
				pltSS->show();
				o->property("splitNumList")->asIntVector().clear();
				o->property("TotNumList")->asIntVector().clear();
				QString selected = w_sel->getValue().toString();//selected data type
				QStringList fileListFullName;
				if (selected == dataType[0])
				{
					eprintf("selection is Z_R*Theta_R");
				}
				else if (selected == dataType[1])
				{
					eprintf("selection is Z_R_Theta");
				}
				else if (selected == dataType[2])
				{
					eprintf("selection is X_Y_Z");
				}
				fileListFullName = mSlot->getCSV(wGetFilName->getValue().toString());//fileList_fullName
				QVector<QVector<double> >  sdata;//point data in 1 single txt
				//QVector<QVector<Double3>> profiles;
				Double3 point;
				QVector<Double3> points;
				QVector<double> zColor;
				QVector<QVector<double>> zColors;
				for each (QString var in fileListFullName)
				{
					txt = mSlot->getCSV(var, ",");
					if (selected == "Z_R*Theta_R")
					{
						mSlot->convertXYZFromZ_RTheta_R(txt, txt.size());
					}
					else if (selected == "Z_R_Theta")
					{
						mSlot->convertXYZFromZ_R_Theta(txt, txt.size());
					}
					else if (selected == "X_Y_Z")
					{
						mSlot->convertXYZFromXYZ(txt, txt.size());
					}
					

					mSlot->putCSV(var.replace(".csv", "_convert.csv"), txt, txt.size(), txt[0].size());
					sdata = mSlot->str2doubleVector(txt, txt.size(), txt[0].size());
					for (int i = 0; i < sdata.size(); i++)
					{
						point[0] = sdata[i][0];
						point[1] = sdata[i][1];
						point[2] = sdata[i][2];
						points.push_back(point);
						zColor.push_back(point[2]);
					}
					profiles.push_back(points);
					zColors.push_back(zColor);
					points.clear();
					zColor.clear();
					steNameList.push_back(mSlot->getFileStem(var));
					o->property("splitNumList")->asIntVector().push_back(sdata.size() / 2);
					o->property("TotNumList")->asIntVector().push_back(sdata.size());

				}
				w_PropertyHolderWidget* holder2 = holder->getHolder(0, 2, 1, 8, "Blade plot");
				if (bldPlt)
				{
					bldPlt->clear();
				}
				else
				{
					bldPlt = vis_Widget::newWidget("vis_WidgetVtk", holder2);
				}
				bldPlt->displaySurfaceFromProfiles("Imp profiles", profiles, &zColors);
				holder2->placeWidget(bldPlt);

				if (mTable)
				{
					mTable->setProperties(properties, &steNameList);
					holder1->placeWidget(mTable, 3, 0, 1, 2);
					holder1->updateShow();

				}
				else
				{
					//dlg.maximumSize();
					mTable = new w_VectorPropertyWidgetV(holder1);
					properties.push_back(o->property("splitNumList"));
					properties.push_back(o->property("TotNumList"));
					mTable->setProperties(properties, &steNameList);
					holder1->placeWidget(mTable, 4, 0, 1, 2);
					holder1->updateShow();
					o->property("splitNumList");

				}

			}
		);

		QObject::connect(pltSS, &w_QPushButton::click, [&holder,o, &profiles,
			&zColors]
			{
				int tot_names = o->property("splitNumList")->asIntVector().size();


			});
		if (dlg.exec() == QDialog::Accepted)
		{
		}

	}
}
O_METHOD_END;


int getminLength(Double2 xy, QVector<Double2> points);

double getLength(Double2 xy, Double2 xynew)
{
	return sqrt(pow(xy[0] - xynew[0], 2) + pow(xy[1] - xynew[1], 2));
}

double getR(Double2 xy)
{
	return sqrt(pow(xy[0], 2) + pow(xy[1], 2));
}

double getR(double x, double y)
{
	return sqrt(pow(x, 2) + pow(y, 2));
}

double findInsection(void)
{
	QVector<Double2> m2Points;
	
	initm2(m2Points);
	QVector<Double3> m3Points;
	Double2 xy;
	double m3Rmin = getR(m3Points.first()[0], m3Points.first()[1]);
	double m3Rmax = getR(m3Points.last()[0], m3Points.last()[1]);

	double m3mean = (m3Rmin + m3Rmax) / 2;
	int m3Index_mid = (m3mean - m3Rmin) / (m3Rmax - m3Rmin);


	xy[0] = getR(m3Points[m3Index_mid][0], m3Points[m3Index_mid][1]);
	xy[1] = m3Points[m3Index_mid][2];

	int findex_mid = getminLength(xy, m2Points);
	double length_mid = getLength(xy, m2Points[findex_mid]);


	bool flag = false;
	int index_tmp = 0;
	int index_min = 0;
	int index_max = 0;
	double length_min = length_mid;
	double length_max = length_mid;
	double length_tmp = 0;
	int marNum = 3;

	for (int i = m3Index_mid; i < m3Points.size(); i++)//m3 loop
	{
		xy[0] = getR(m3Points[i][0], m3Points[i][1]);
		xy[1] = m3Points[i][2];
		
		getminLength(xy, m2Points, index_tmp, length_tmp);
		//index_tmp = getminLength(xy, m2Points);
		//length_tmp = getLength(xy, m2Points[index_tmp]);
		if (length_tmp < length_max)
		{
			length_max = length_tmp;
		}
		else
		{
			int j = i;
			flag = true;
			while (j < m3Points.size())
			{
				xy[0] = getR(m3Points[j][0], m3Points[j][1]);
				xy[1] = m3Points[i][2];
				//index_tmp = getminLength(xy, m2Points);
				//length_tmp = getLength(xy, m2Points[index_tmp]);
				getminLength(xy, m2Points, index_tmp, length_tmp);
				if (length_tmp < length_max)
				{
					flag = false;
					break;
				}
				if (j > i + marNum)
				{
					flag = true;
					break;
				}
				if (flag)
				{
					index_max = index_tmp;
					break;
				}
				j++;
			}
		}

	}
	
	for (int i = m3Index_mid; i >= 0; i--)//m3 loop
	{
		xy[0] = getR(m3Points[i][0], m3Points[i][1]);
		xy[1] = m3Points[i][2];
		//index_tmp = getminLength(xy, m2Points);
		//length_tmp = getLength(xy, m2Points[index_tmp]);
		getminLength(xy, m2Points, index_tmp, length_tmp);
		if (length_tmp < length_max)
		{
			length_min = length_tmp;
		}
		else
		{
			int j = i;
			flag = true;
			while (j >= 0)
			{
				xy[0] = getR(m3Points[j][0], m3Points[j][1]);
				xy[1] = m3Points[i][2];
				//index_tmp = getminLength(xy, m2Points);
				//length_tmp = getLength(xy, m2Points[index_tmp]);
				getminLength(xy, m2Points, index_tmp, length_tmp);
				if (length_tmp < length_min)
				{
					flag = false;
					break;
				}
				if (j < i - marNum)
				{
					flag = true;
					break;
				}
				if (flag)
				{
					index_min = index_tmp;
					break;
				}
				j--;
			}
		}

	}

	findex_mid = length_max > length_min ? index_min : index_max;
	eprintf("R=%f, z=%f", m2Points[findex_mid][0], m2Points[findex_mid][1]);
	return 0.0;
}

int getminLength(Double2 xy, QVector<Double2> points)
{
	QMap<int, double> res;
	double Rmin = points.first()[0];
	double Rmax = points.last()[0];
	double midIndex = int((xy[0] - Rmin) / (Rmax - Rmin));
	int totNum = points.size();
	bool flag = false;
	double lengthMid = getLength(xy, points[midIndex]);
	double length = lengthMid;
	double lengthtmp = 0;
	int finalIndex = midIndex;
	int index2 = 0;
	int index1 = 0;
	int marNum = 3;
	for (int i = midIndex + 1; i < totNum; i++)
	{
		lengthtmp = getLength(xy, points[i]);
		if (lengthtmp < length)
		{
			length = lengthtmp;
		}
		else
		{
			int j = i;
			flag = true;
			while (j < totNum)
			{
				lengthtmp = getLength(xy, points[j]);
				j = j + 1;
				if (lengthtmp < length)
				{
					flag = false;
					break;
				}
				if (j > i + marNum)
				{
					flag = true;
					break;
				}
			}
			if (flag)
			{
				index2 = i;
				break;
			}
		}

	}
	double length2 = lengthMid;
	flag = false;
	for (int i = midIndex; i >= 0; i--)
	{
		lengthtmp = getLength(xy, points[i]);
		if (lengthtmp < length)
		{
			length2 = lengthtmp;
		}
		{
			int j = i;
			flag = true;
			while (j >= 0)
			{
				lengthtmp = getLength(xy, points[j]);
				j = j - 1;
				if (lengthtmp < length)
				{
					flag = false;
					break;
				}
				if (j < i - marNum)
				{
					break;
				}
			}
			if (flag)
			{
				index1 = i;
				break;
			}
		}
	}
	finalIndex = length2 > length ? index2 : index1;
	return finalIndex;
}

void getminLength(Double2 xy, QVector<Double2> points, int &fIndex, double &fLength)
{
	QMap<int, double> res;
	double Rmin = points.first()[0];
	double Rmax = points.last()[0];
	double midIndex = int((xy[0] - Rmin) / (Rmax - Rmin));
	int totNum = points.size();
	bool flag = false;
	double lengthMid = getLength(xy, points[midIndex]);
	double length = lengthMid;
	double lengthtmp = 0;
	int finalIndex = midIndex;
	int index2 = 0;
	int index1 = 0;
	int marNum = 3;
	for (int i = midIndex + 1; i < totNum; i++)
	{
		lengthtmp = getLength(xy, points[i]);
		if (lengthtmp < length)
		{
			length = lengthtmp;
		}
		else
		{
			int j = i;
			flag = true;
			while (j < totNum)
			{
				lengthtmp = getLength(xy, points[j]);
				j = j + 1;
				if (lengthtmp < length)
				{
					flag = false;
					break;
				}
				if (j > i + marNum)
				{
					flag = true;
					break;
				}
			}
			if (flag)
			{
				index2 = i;
				break;
			}
		}

	}
	double length2 = lengthMid;
	flag = false;
	for (int i = midIndex; i >= 0; i--)
	{
		lengthtmp = getLength(xy, points[i]);
		if (lengthtmp < length)
		{
			length2 = lengthtmp;
		}
		{
			int j = i;
			flag = true;
			while (j >= 0)
			{
				lengthtmp = getLength(xy, points[j]);
				j = j - 1;
				if (lengthtmp < length)
				{
					flag = false;
					break;
				}
				if (j < i - marNum)
				{
					break;
				}
			}
			if (flag)
			{
				index1 = i;
				break;
			}
		}
	}
	finalIndex = length2 > length ? index2 : index1;
	fIndex= length2 > length ? index2 : index1;
	fLength= length2 > length ? length : length2;
	return ;
}

void initm2(QVector<Double2>& points)
{
	Double2 xy;
	xy[0] = 0;
	xy[1] = 28.41378;
	QVector<double> x = { 0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,36.5,40,42,44,45,46.5,47};
	QVector<double> y = { 28.4137373894769,28.2978260294701,28.2188977640458,28.2132639280913,
		28.3171188612529,28.5259441448266,28.7349466655075,28.8375136957043,28.8922223997855,
		29.0459670018532,29.2913250012089,29.5628209691372,29.8987906645544,30.4216272720845,
		31.247318076434,32.3155044756485,33.4130222237973,34.5555795108714,35.9966810758893,
		36.4362969351668,40.9726317144509,45.6309359959479,54.2576544552661,57.9679545062146,90.3832963909524,110.402000107804 };
	for (int i = 0; i < x.size(); i++)
	{
		xy[0] = x[i];
		xy[1] = y[i];
		points.push_back(xy);
	}

	return;
}

void initm3(QVector<Double3>& points)
{
	Double3 xy;
	xy[0] = 0;
	xy[1] = 28.41378;
	
	for (int i = 10; i < 32; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			xy[0] = i;
			xy[1] = 0;
			xy[2] = -2.75 * i + 87.5;
			points.push_back(xy);
		}

	}

	return;
}