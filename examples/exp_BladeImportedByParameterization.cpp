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

#include "exp_BladeImportedByParameterization.h"
#include "core_Application.h"
#include "w_QFile.h"
#include "w_TModule.h"
#include "w_TTaskWindow.h"

#include "mshs_Vector2D.h"
#include "mshs_Vector3D.h"

#include "w_PropertyString.h"
#include "w_PropertyDouble.h"
#include "util_Parameter.h"

#include "vis_Widget.h"
#include "curve_Curve.h"
#include "curve_Line.h"
#include "curve_Polygon.h"
#include "curve_Circle.h"
#include "curve_Spline.h"
#include "curve_Nurbs.h"
#include "curve_Topology.h"

#include "w_PropertyHolderDialog.h"
#include "w_PropertyHolderWidget.h"
#include "draw_TopologyInteractiveEditorWidget.h"
#include "draw_TopologyInteractiveEditor.h"
#include "fstream"



REGISTER_OBJECT_CLASS(exp_BladeImportedByParameterization, "exp_BladeImportedByParameterization", TObject);
//REGISTER_OBJECT_CLASS_DISPLAY_NAME(exp_VaneImportedFromDataFile, "Vane Imported From Data FileExample Class");

exp_BladeImportedByParameterization::exp_BladeImportedByParameterization(QString object_n, TObject* iparent) :QObject(),
TObject(object_n, iparent)
{
	 INIT_OBJECT;
	setTag("MethodOrder", "ImportGenerateBlade,"
		"$");

	DEFINE_QSTRING_INIT(lengthUnitExported, "Length Unit exported", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(int, _camberDataType, "Blade number", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(int, _rotationDirection, "Rotation direction", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(int, _bladeNumOfMainBladeRow, "Blade number", 0, NULL, NULL);
	DEFINE_SCALAR_INIT(double, offsetTheta, "offset theta", 0, NULL, TUnit::deltaAngle);

	_camberDataType = ZRTheta;
	lengthUnitExported = "mm";
	offsetTheta = 0.;
	_referenceAxis = negtiveZAxis;
}


double exp_BladeImportedByParameterization::getDoubleDegree(double degree)
{
	int degree_int = floor(degree);
	double degree_re = degree - degree_int;

	double degree_trans = degree_re / 0.6 + degree_int;
	return degree_trans;
}

QStringList exp_BladeImportedByParameterization::getAllReferenceAxisType()
{
	QStringList referenceAxisTypes = QStringList() << "-Zxis" << "+ZAxis";
	return referenceAxisTypes;
}

void exp_BladeImportedByParameterization::setReferenceAxis(QString referenceAxis)
{
	QStringList referenceAxisTypes = getAllReferenceAxisType();
	if (referenceAxis == referenceAxisTypes.first())
	{
		_referenceAxis = negtiveZAxis;
	}
	else
	{
		_referenceAxis = positiveZAxis;
	}
}

QString exp_BladeImportedByParameterization::getCurrentReferenceAxis()
{
	QStringList referenceAxisTypes = getAllReferenceAxisType();
	QString referenceAxisType;
	if (_referenceAxis == negtiveZAxis)
	{
		referenceAxisType = referenceAxisTypes.first();
	}
	else
	{
		referenceAxisType = referenceAxisTypes.last();
	}

	return referenceAxisType;
}

void exp_BladeImportedByParameterization::setCamberDataType(int camberDataType)
{
	_camberDataType = camberDataType;
}

int exp_BladeImportedByParameterization::getCamberDataType()
{
	return _camberDataType;
}


O_METHOD_BEGIN(exp_BladeImportedByParameterization, ImportGenerateBlade, "Import & Generate Blade", 0, 0, "")
{
	if (VARLID_ARG)
	{

		exp_BladeImportedByParameterization* curretObj = object;

		object->showDialog(curretObj);

	}
}
O_METHOD_END;


void exp_BladeImportedByParameterization::onImportPressed()
{

	bool isSuccess = getData(filename);

	//createBlade();
	
	//if (!this->getZRCurveTopology(false) && isSuccess)
	if (isSuccess)
	{
		//2. Generate
		this->getZRCurveTopology(false);
		
		eprintf("now is in onImportPressed and the file name is %s",filename.ascii());
		w_QPushButton* generateButton = holder_properties->addButton("Step2. Generate 2D view",3,0,1,2);
		QObject::connect(generateButton, &w_QPushButton::clicked,  [=]()
			{
				
				//generateButton->setEnabled(false);
				generateButton->setHidden(true);
				this->handleUpdateData();

			});
		holder->addMappedWidget("button", generateButton);
		
		
		
	}


}

void exp_BladeImportedByParameterization::onExportPressed()
{
	QString exportFilename = this->filename.replace(".csv","_exported.txt");

	exportToFile(exportFilename);
	eprintf("the export file is %s", exportFilename.ascii());
	printMessage("the export file is %s", exportFilename.ascii());
	

}
QStringList exp_BladeImportedByParameterization::getStringList(QString& s)
{
	QStringList list;
	list = s.split(",");
	return list;
};

bool exp_BladeImportedByParameterization::getData(QString filename)
{

	auto getValue = [&](QString& s)->double
	{
		QStringList list = getStringList(s);
		double value = 0.;
		if (list.size() >= 2)
			value = list[1].toDouble();
		return value;
	};

	auto getString = [&](QString& s)->QString
	{
		QStringList list = getStringList(s);
		QString value = '_';
		if (list.size() >= 2)
			value = list[1];
		return value;
	};

	w_QFile file(filename);
	QStringList strArray;

	int ii = 0;
	QString lenghtUnitString, rotationDirectionString = "ccw";
	QString CamberDataType;
	QString hubProfileFileName, shroudProfileFileName;

	if (file.open(QIODevice::ReadOnly))
	{
		QTextStream stream(&file);
		QString line;
		while (!stream.atEnd())
		{
			line = stream.readLine();
			if (line.startsWith("Unit") || line.startsWith("unit"))
			{
				lenghtUnitString = getString(line);
			}
			else if (line.startsWith("Rotation direction"))
			{
				rotationDirectionString = getString(line);
			}
			else if (line.startsWith("Camber data type"))
			{
				CamberDataType = getString(line);
			}
			else if (line.startsWith("Hub profile"))
			{
				hubProfileFileName = getString(line);
			}
			else if (line.startsWith("Shroud profile"))
			{
				shroudProfileFileName = getString(line);
			}
			else if (line.startsWith("Main blade number"))
			{		
				_bladeNumOfMainBladeRow = getValue(line);
			}
			else if (line.startsWith("Degree type"))
			{
				_degreeType = getValue(line);
			}
			else
			{
				strArray += line;
				ii++;
			}

		}

		file.close();
	}

	_rotationDirection = CCW;
	if (rotationDirectionString == "CW")
		_rotationDirection = CW;

	_camberDataType = ZRTheta;
	if(CamberDataType == "ZRBeta")
		_camberDataType = ZRBeta;
	else if (CamberDataType == "XYZ")
		_camberDataType = XYZ;

	QString filePath = getFilePath(filename);
	// part 3
	dataSpans.clear();
	// hub
	hubProfileFileName = filePath+hubProfileFileName;
	readSpanwiseData( 0, hubProfileFileName, lenghtUnitString);

	// shroud
	shroudProfileFileName = filePath + shroudProfileFileName;
	readSpanwiseData(100, shroudProfileFileName, lenghtUnitString);


	return true;
}

QString exp_BladeImportedByParameterization::getFilePath(QString filename)
{
	int index = filename.lastIndexOf('/');

	QString filePath = filename.mid(0, index+1);

	return filePath;
}

double exp_BladeImportedByParameterization::coefficentWithLengthUnit(QString lengthUnit)
{
	double coefficentWithUnitImport = 1.;
	if (lengthUnit == "inch")
		coefficentWithUnitImport = 0.0254;
	else if (lengthUnit == "mm")
		coefficentWithUnitImport = 0.001;
	else if (lengthUnit == "cm")
		coefficentWithUnitImport = 0.01;
	else
		coefficentWithUnitImport = 1.;

	return coefficentWithUnitImport;
}

int exp_BladeImportedByParameterization::readSpanwiseData(double span, QString spanwiseDataFile, QString LengthUnitImport)
{
	w_QFile file(spanwiseDataFile);
	QStringList strArray;

	int ii = 0;
	int index_merdional = 0; // Z, R
	int index_camberCurve = -1; // theta, beta, X
	int index_profile = -1; // thickness


	if (file.open(QIODevice::ReadOnly))
	{
		QTextStream stream(&file);
		QString line;
		while (!stream.atEnd())
		{
			line = stream.readLine();


			if (line.startsWith("Meridional") || line.startsWith("ZR"))
			{
				index_merdional = 0;
			}
			else if (line.startsWith("CamberCurve"))
			{
				index_camberCurve = ii;
			}
			else if (line.startsWith("Profile"))
			{
				index_profile = ii;
			}	
			else 
			{
				strArray += line;
				ii++;
			}
		}

		file.close();
	}

	QVector<int> indexs = QVector<int>() << index_merdional << index_camberCurve << index_profile << strArray.size();

  QVector<QVector<Double2>> dataSpan;
  for (int i = 0; i<indexs.size()-1; i++)
  {
		//  1.
    int index_start = indexs[i];
    int index_end = indexs[i+1]-1;

		// 2.
		QStringList strArrayCurrentSeg;
		for (int j = index_start; j<= index_end;j++)
		{
			strArrayCurrentSeg.push_back(strArray[j]);
		}

		// 3.
		QVector<Double2> data = getDouble2Data(strArrayCurrentSeg);
		if (i == 0)
		{
			double coefficent = coefficentWithLengthUnit(LengthUnitImport);
			for (int j = 0; j< data.size();j++)
				data[j] *= coefficent;
		}
		
		if (i != 0)
		{
			double coefficent = 0.01;
			for (int j = 0; j < data.size(); j++)
				data[j][0] *= coefficent;
		}

		if (i == 2)
		{
			double coefficent = coefficentWithLengthUnit(LengthUnitImport);
			for (int j = 0; j < data.size(); j++)
				data[j][1] *= coefficent;
		}

		dataSpan.push_back(data);
  }

	dataSpans.push_back(dataSpan);
	return 0;
}

QVector<Double2> exp_BladeImportedByParameterization::getDouble2Data(QStringList& strArray)
{
	QVector<Double2> data;
	for (int i = 0; i < strArray.size(); i++)
	{
		QStringList list = getStringList(strArray[i]);
		QVector<double> d;
		for (int j = 0; j < list.size(); j++)
		{
			d.push_back(list[j].toDouble());
		}
		if (d.size() >= 2)
		{
			Double2 d2 = {d[0], d[1]};
			data.push_back(d2);
		}
	}
	return data;
}

QVector<Double2> exp_BladeImportedByParameterization::getSpanData(QString whichLevelData, double span)
{
	int Span = 1;
	if (span <= 50.)
		Span = 0;

	int index = 0;
	QVector<Double2> data;
	if (whichLevelData == "Meridional")
	{
		index = 0;
	}
	else if (whichLevelData == "CamberCurve")
	{
		index = 1;
	}
	else if (whichLevelData == "Profile")
	{
		index = 2;
	}
	return dataSpans[Span][index];
}

void exp_BladeImportedByParameterization::handleUpdateData()
{
	this->createBlade();
	
	//if (holder_topoZR && wZRCurves && holder_topoMRTheta  && wMRTheta)
	//{
	//  wZRCurves->setTopology(this->getZRCurveTopology());
	//  holder_topoZR->placeWidget(wZRCurves);
	//  wZRCurves->replot();

	//  wMRTheta->setTopology(this->getMThetaCurveTopology());
	//  holder_topoMRTheta->placeWidget(wMRTheta);
	//  wMRTheta->replot();
	//}

	if (this->getZRCurveTopology(false))
	{
		//3. Export
		(holder->parentWidget())->showMaximized();

		QStringList export_unit = QStringList() << "mm" << "m";
		QString selection = export_unit[1];

		int bladeNum = 10;
		double mthetaDeg = 0;
		w_Property* mExport = this->holder_properties->addProperty(&selection,"export unit",&export_unit,false,false,true);
		w_Property* mBladeNum = this->holder_properties->addProperty(&bladeNum,"Blade number");
		w_Property* mTheta = this->holder_properties->addProperty(&mthetaDeg,"offset angele(deg)");
		mBladeNum->setRange(1, 20);
		mTheta->setRange(-180, 180);

		
		w_QPushButton* exportButton = holder_properties->addButton("Step3. Export",4,0,1,2);
		QObject::connect(exportButton, &w_QPushButton::clicked, [&,mExport, mBladeNum,mTheta]()
			{
				lengthUnitExported = mExport->getValue().toString().ascii();
				this->_bladeNumOfMainBladeRow = mBladeNum->getValue().toInt();
				this->offsetTheta = mTheta->getValue().toDouble();
				onExportPressed(); 
			});
		holder->addMappedWidget("button", exportButton);
		holder->addMappedWidget("mExport", mExport);
		holder->addMappedWidget("mBladeNumX", mBladeNum);
		holder->addMappedWidget("mTheta", mTheta);
		

		//1. Add ZR topo View
		holder_topoZR = holder_properties->getHolder(5, 0, 8, 2, "Z-R");
		if (wZRCurves = new draw_TopologyInteractiveEditorWidget(holder_topoZR))
		{
			wZRCurves->setSizeHint(QSize(800, 600));
			
			if (auto plot = wZRCurves->getPlot())
			{
				plot->setScaleDistance(0.006);
				plot->setArrowSize(0.001);
				plot->setEndSymboSize(8);
				plot->getParameter("scaleExtensionSkipDistance").setValue(0.00001);
			}
			wZRCurves->setTopology(this->getZRCurveTopology());

			holder_topoZR->placeWidget(wZRCurves);
		}

		//2. Add MTheta topo View
		holder_topoMRTheta = holder->getHolder(0, 1, 1, 2, "M - Theta");
		if (wMRTheta = new draw_TopologyInteractiveEditorWidget(holder_topoMRTheta))
		{
			wMRTheta->setSizeHint(QSize(800, 600));
			if (auto plot = wMRTheta->getPlot())
			{
				plot->setScaleDistance(0.006);
				plot->setArrowSize(0.001);
				plot->setEndSymboSize(8);
				plot->getParameter("scaleExtensionSkipDistance").setValue(0.00001);
			}
			wMRTheta->setTopology(this->getMThetaCurveTopology());

			holder_topoMRTheta->placeWidget(wMRTheta);
		}



		holder_topoMfracBeta = holder->getHolder(1, 2, 1, 1, "m-beta");
		if (wMfracBeta = new draw_TopologyInteractiveEditorWidget(holder_topoMfracBeta))
		{
			wMfracBeta->setSizeHint(QSize(800, 600));
			if (auto plot = wMfracBeta->getPlot())
			{
				plot->setScaleDistance(0.006);
				plot->setArrowSize(0.001);
				plot->setEndSymboSize(8);
				plot->getParameter("scaleExtensionSkipDistance").setValue(0.00001);
			}
			wMfracBeta->setTopology(this->getmBetaCurveTopology());

			holder_topoMfracBeta->placeWidget(wMfracBeta);
		}



		//3. Add 3D View
		holder_3D = holder->getHolder(1, 1, 1, 1, "3D");
		// if (w3D = new vis_Widget(holder_topo3D))
		if (w3D = vis_Widget::newWidget("vis_WidgetVtk", holder_3D))
		{
			w3D->setSizeHint(QSize(800, 600));

			setVTKShow(w3D);

			holder_3D->placeWidget(w3D);
		}
	}
}

void exp_BladeImportedByParameterization::revolutionSurface(curve_Polygon* zrCurve, QVector<QVector<Double3>>& surface, QVector<QVector<double>>& value)
{
	if (!zrCurve)
		return;

	int size = zrCurve->size();
	QVector<Double2> zrs;
	for (int i = 0; i < size; i++)
	{
		zrs.push_back(zrCurve->Double2At(i));
	}

	int nSliceTheta = 361;
	double dTheta = 2 * PI / (nSliceTheta - 1);

	for (int i = 0; i < nSliceTheta; i++)
	{
		QVector<Double3> data_thetaSlice;
		QVector<double> value_thetaSlice;
		double theta = i * dTheta;
		for (int j = 0; j < size; j++)
		{
			double r = zrs[j][1];
			double x = r * cos(theta);
			double y = r * sin(theta);
			double z = zrs[j][0];
			data_thetaSlice.push_back(Double3(x, y, z));
			value_thetaSlice.push_back(0.);
		}
		surface.push_back(data_thetaSlice);
		value.push_back(value_thetaSlice);
	}
}

void exp_BladeImportedByParameterization::showDialog(exp_BladeImportedByParameterization* blade)
{
	w_PropertyHolderDialog dlg(core_Application::core());
	dlg.setWindowTitle(w_PropertyHolderDialog::tr("Generate Blade"));

	QString fileName;

	holder = dlg.getHolder(0, 0, 1, 1);
	
	//Add properties
	holder_properties = holder->getHolder(0, 0, 2, 1, "Generate");
	/*w_Property* wGetFilName = holder_properties->addProperty(&fileName, "File path", 0, false,
		false, false, "TCallbackOpenFileBrowser");*/
	w_Property* wGetFilName = holder_properties->addProperty(&fileName, "File path", 0, false,
		false, false, "TCallbackOpenFileBrowser",false,w_Property::VLayout);
	wGetFilName->setToolTip("Input the full file path");

	w_QPushButton* apply = holder_properties->addButton("Step1. Import", 1, 0, 1, 2);
	
	QObject::connect(apply, &w_QPushButton::clicked, &dlg, [
		wGetFilName, &apply,&blade]()
	/*QObject::connect(apply, &w_QPushButton::clicked, [
		=]()*/
		{	
			blade->filename = wGetFilName->getValue().toString().ascii();
			blade -> onImportPressed();
			wGetFilName->setEnabled(false);
			//apply->setEnabled(false);
			apply->setHidden(true);
		}
		);
	

	/*w_QPushButton* btn_import = holder_properties->addButton(QObject::tr("Step 1. Import"), 0, 0, 1, 1);
	QObject::connect(btn_import, SIGNAL(clicked()), this, SLOT(onImportPressed()));*/


	if (dlg.exec() == QDialog::Accepted)
	{

	}

}

void exp_BladeImportedByParameterization::setVTKShow(vis_Widget* w)
{

	QVector<QVector<Double3>> profiles;
	QVector<QVector<double>> values;

	profiles.push_back(getProfile(0));

	profiles.push_back(getProfile(100));

	for (int i = 0; i < profiles.size(); i++)
	{
		QVector<double> _values;
		for (int j = 0; j < profiles[i].size(); j++)
			_values.push_back(100.);
		values.push_back(_values);
	}

	QMap<QString, QVariant> args =
	{
	  {"colorName", "lightRed"},
	  {"lineWidth", 1},
	  //{"showMode", "Edge+Face"}
	};

	w->displaySurfaceFromProfiles("Multi-layer profiles", profiles, &values, &args);


	// hub
	QVector<QVector<Double3>> Surface_hub;
	QVector<QVector<double>> values_hub;
	revolutionSurface(getZRPolgonCurve(0), Surface_hub, values_hub);
	QMap<QString, QVariant> args_hub =
	{
		{"colorName", "lightBlue"},
		{"lineWidth", 1}
	};

	w->displaySurfaceFromProfiles("hub surface", Surface_hub, &values_hub, &args_hub);

}

int exp_BladeImportedByParameterization::createProfile(double span)
{
	int error = 0;
	//step 0.
	error = createCamberCurve(span);

	// 1. update some parameters of camber curve
	error = updateCamberCurve(span);

	/*
	purpose:
	*/

	double ule = getMFracOfLECycle(span);

	// double ute = getMFracOfTEShread(span);

	double ute = 1.;
	// step 1. LE curve
	createLECurve(ule, span);

	//step 2. pressure curve
	createPressureCurve(ule, ute, span);

	//step 3. suction curve
	createSuctionCurve(ule, ute, span);

	// 4.
	// 4.1
	extendSideCurve(span);
	// 4.2
	cuttedSideCurve(span);
	// 4.3
	fitSideCurve(span);

	//step 4. TE curve
	createTECurve(span);

	return 0;
}

int exp_BladeImportedByParameterization::createCamberCurve(double span)
{
	// 1. fit zr curve
	curve_Polygon* zrPoly = getZRPolgonCurve(span);
	QVector<Double2> zrs;
	for (int i = 0; i < zrPoly->size(); i++)
	{
		zrs.push_back(zrPoly->Double2At(i));
	}

	curve_Nurbs* zrNurbs = getZRNurbsCurve(span);
	zrNurbs->fitBezier(zrs, 11, 3);


	// 2.
	// 2.1 
	curve_Polygon* MThetaPoly = getCamberMThetaPolygonCurve(span);

	QVector<Double2> MTheta;
	QVector<Double2> MFracM;
	for (int i = 0; i < MThetaPoly->size(); i++)
	{
		MTheta.push_back(MThetaPoly->Double2At(i));
		double mFrac = (MTheta.last())[0];
		double M = getMFromMFrac(span, mFrac);

		(MTheta.last())[0] = M;

		MFracM.push_back(Double2(mFrac, M));
	}
	MThetaPoly->setDouble2Vector(MTheta);

	// 2.2
	curve_Nurbs* MThetaNurbs = getCamberMThetaNurbsCurve(span);
	int nc = 11;
	if (MTheta.size() <= nc)
		nc = MTheta.size() - 1;
	MThetaNurbs->fitBezier(MTheta, nc, 3);

	//2.3
	curve_Nurbs* MFracMNurbs = getCamberMFracM(span);
	nc = 11;
	if (MFracM.size() <= nc)
		nc = MFracM.size() - 1;

	MFracMNurbs->fitBezier(MFracM, nc, 3);

	return 0;
}

int exp_BladeImportedByParameterization::updateCamberCurve(double span)
{
	if (_camberDataType == ZRBeta)
	{

	}
	else if (_camberDataType == XYZ)
	{

	}
	else
	{
		// Have known:F1. m-theta
		// to calcu:F2. m-beta
		// F1.
		curve_Polygon* poly_MTheta = getCamberMThetaPolygonCurve(span);
		curve_Nurbs* MThetaNurbs = getCamberMThetaNurbsCurve(span);
		// F2.
		QVector<Double2> mBeta;

		for (int i = 0; i < poly_MTheta->size(); i++)
		{
			Double2 pt = poly_MTheta->Double2At(i);
			double M = pt[0];
			double m = get_mfracFromM(span, M);
			QVector<double> us = MThetaNurbs->getUFromX(M);
			if (!us.size())
			{
				return -1;
			}

			Double2 T = MThetaNurbs->getTangent(us.first());
			double beta = Double2(1., 0.).angleSigned(T );

			mBeta.push_back(Double2(m, beta));
		}
		curve_Polygon* poly_mBeta = getCambermBetaPolygonCurve(span);
		poly_mBeta->setDouble2Vector(mBeta);

	}

	return 0;
}

bool exp_BladeImportedByParameterization::setProfile(double span)
{
	//1. set Meridional
	//  1.1
	QVector<Double2> zrs = getSpanData("Meridional", span);

	// 1.2
	curve_Polygon* zrPoly = getZRPolgonCurve(span);

	zrPoly->setDouble2Vector(zrs);
	// 1.3
	curve_Nurbs* zrNurbs = getZRNurbsCurve(span);
	// 1.3.1
	int nc = 11;
	if (zrPoly->size()< nc)
		nc = zrPoly->size() - 1;
	// 1.3.2
	zrNurbs->fitBezier(zrs, nc, 3);

	//2. set camber curve
	QVector<Double2> MTheta;
	if (_camberDataType == ZRBeta)
	{
		// 1.0
		QVector<Double2> mBeta = getSpanData("CamberCurve", span);
		if (_degreeType == 1)
		{
			for (int i = 0; i < mBeta.size(); i++)
				mBeta[i][1] = getDoubleDegree(mBeta[i][1]);
		}

		for (int i = 0; i < mBeta.size(); i++)
			mBeta[i][1] *= PI / 180.;

		// 2.1 set poly_mThetaCurve
		curve_Polygon* poly_mBeta = getCambermBetaPolygonCurve(span);
		poly_mBeta->setDouble2Vector(mBeta);

		// 2.2
		checkMfrac(poly_mBeta, zrPoly, zrNurbs);

		mBeta.clear();
		for (int i = 0; i < poly_mBeta->size(); i++)
			mBeta.push_back(poly_mBeta->Double2At(i));

		MTheta = transfromTomTheta(zrNurbs, mBeta);

	}
	else if (_camberDataType == XYZ)
	{

	}
	else
	{
		MTheta = getSpanData("CamberCurve", span);
		if (_degreeType == 1)
		{
			for (int i = 0; i < MTheta.size(); i++)
				MTheta[i][1] = getDoubleDegree(MTheta[i][1]);
		}
		for (int i = 0; i < MTheta.size(); i++)
			MTheta[i][1] *= PI / 180.;
	}
	// 2.1 set poly_mThetaCurve
	curve_Polygon* poly_mTheta = getCamberMThetaPolygonCurve(span);

	poly_mTheta->setDouble2Vector(MTheta);
	// 2.2
	checkMfrac(poly_mTheta, zrPoly, zrNurbs);

	//3. set poly_mThicknessCurve
	// 3.1
	QVector<Double2> mThickess = getSpanData("Profile", span);

	// 3.2
	curve_Polygon* poly_mThickness = getMFracThicknessPolgonCurve(span);
	poly_mThickness->setDouble2Vector(mThickess);

	// 3.3
	checkMfrac(poly_mThickness, zrPoly, zrNurbs);

	// 3.4
	mThickess = poly_mThickness->toDouble2Vector();
	curve_Nurbs* mThickness_Nurbs = getMFracThicknessNurbsCurve(span);
	nc = 11;
	if (poly_mThickness->size() < nc)
		nc = poly_mThickness->size() - 1;
	mThickness_Nurbs->fitBezier(mThickess, nc);

	return true;
}

double exp_BladeImportedByParameterization::getMeridonalLength(double span)
{
	if (getZRNurbsCurve(span))
	{
		return getZRNurbsCurve(span)->getLength();
	}
	return 0;
}

QVector<Double2> exp_BladeImportedByParameterization::transfromTomTheta(curve_Nurbs* zrNurbs, QVector<Double2> mBeta)
{
	double totalLength = zrNurbs->calculateLength(0., 1.);

	QVector<Double2> mTheta;
	mTheta.push_back(Double2(0., 0.));
	double uPre = 0.;
	for (int i = 1; i < mBeta.size(); i++)
	{
		// 1.
		Double2 mBeta_pre = mBeta[i - 1];
		Double2 mBeta_now = mBeta[i];
		double beta_avg = 0.5 * (mBeta_pre + mBeta_now)[1];
		double dmFrac = (mBeta_now - mBeta_pre)[0];

		// 2.
		double dm = dmFrac * totalLength;
		bool found = false;
		double uNow = zrNurbs->getUFromLength(dm, &found, uPre);
		if (!found)
		{
			eprintf("--------");
		}

		// 3. tan(beta) = rdTheta/dm
		Double2 pt_pre = zrNurbs->getPoint(uPre);
		Double2 pt_now = zrNurbs->getPoint(uNow);

		double rdTheta = dm * tan(beta_avg);
		double rAvg = 0.5*(pt_pre+ pt_now)[1];
		double dTheta = rdTheta / rAvg;

		// 3.
		Double2 _dmdTheta = { dmFrac, dTheta };
		Double2 mTheta_now = mTheta.last() + _dmdTheta;
		mTheta.push_back(mTheta_now);

		uPre = uNow;
	}

	return mTheta;
}

int exp_BladeImportedByParameterization::checkMfrac(curve_Polygon* polyChecked, curve_Polygon* zrPoly, curve_Nurbs* zrNurbs)
{
	double totalLength = zrPoly->getLength();
	double lengthActual = 0.;

	for (int i = 1; i < polyChecked->size(); i++)
	{
		Double2 pt = polyChecked->Double2At(i);
		double mFrac = pt[0];
		lengthActual += zrPoly->getLength(i-1, i);
		double mFracActual = lengthActual/ totalLength;
		if (mFrac == 0)
		{
			pt[0] = mFracActual;
			polyChecked->setControlPointPosition(i, pt);
		}

	}

	return 0;
}

int exp_BladeImportedByParameterization::createBlade()
{
	QVector<double> spans = QVector<double>() << 0. << 100.;

	for (int i = 0; i < spans.size(); i++)
	{
		double span = spans[i];
		// 1.
		setProfile(span);

		// 2. 
		createProfile(span);
	}

	return 0;
}

double exp_BladeImportedByParameterization::getMFracOfLECycle(double span)
{
	/*


	M  <--->  m  <--->  mFrac
			  |
			  |
			  v
			(z,r)

	  T = thickness/r;

	*/

	// 0.
	curve_Nurbs* pZRNurbs = getZRNurbsCurve(span);
	curve_Nurbs* pMThetaNurbs = getCamberMThetaNurbsCurve(span);
	curve_Nurbs* pMThinkNurbs = getMFracThicknessNurbsCurve(span);
	curve_Nurbs* pMFracMNurbs = getCamberMFracM(span);

	double mTotalLength = getMeridonalLength(span);

	int sign = 1;
	double x0 = 0.;
	auto getDelta = [&](double u)->double
	{
		// 0.
		Double2 pos = pMThetaNurbs->getPoint(u);
		Double2 T = pMThetaNurbs->getTangent(u);
		T /= T.length();

		double Mcurrent = pos[0];
		double mFrac = pMFracMOfNurbs(Mcurrent, span);

		// update
		// 1.
		double equivalentThinkness = getEquivalentThinkness(mFrac, span);
		// 3.
		Double2 suction = pos - 0.5 * equivalentThinkness;
		// 4.
		double delta = (suction[0] - x0);
		return delta;
	};

	//
	double equivalentThinkness = getEquivalentThinkness(0., span);

	QVector<double> uScope = QVector<double>() << 0. << 0.1 << 0.2;
	QVector<double> delta; delta.resize(3);

	double uFound = 0;

	double tol = 1.E-3;
	int steps = 50;
	for (int i = 0; i < steps; i++)
	{

		for (int j = 0; j < 3; j++)
		{
			delta[j] = getDelta(uScope[j]);
		}

		if (delta[1] < tol * equivalentThinkness && delta[1] > 0)
		{
			uFound = uScope[1];
			break;
		}

		if (delta[1] < 0.)
		{
			uScope[0] = uScope[1];
			delta[0] = delta[1];
		}
		else if (delta[1] > 0.)
		{
			uScope[2] = uScope[1];
			delta[2] = delta[1];
		}
		uScope[1] = (uScope[0] + uScope[2]) / 2.;

		if (i == steps - 1)
		{
			eprintf("  Can't find LE location");
		}
	}

	return uFound;
}

double exp_BladeImportedByParameterization::getEquivalentThinkness(double mFrac, double span)
{
	curve_Nurbs* pZRNurbs = getZRNurbsCurve(span);
	curve_Nurbs* pMThinkNurbs = getMFracThicknessNurbsCurve(span);
	double mTotalLength = getMeridonalLength(span);

	double thickness = (pMThinkNurbs->getYFromX(mFrac))[0];
	bool found = false;
	double u = pZRNurbs->getUFromLength(mFrac * mTotalLength, &found);
	if (!found)
		return 0.;

	Double2 zr = pZRNurbs->getPoint(u);
	double equivalentThinkness = thickness / zr[1];

	return equivalentThinkness;
}

double exp_BladeImportedByParameterization::pMFracMOfNurbs(double M, double span)
{
	curve_Nurbs* pMFracMNurbs = getCamberMFracM(span);
	QVector<double> u = pMFracMNurbs->getXFromY(M);
	double MLast = pMFracMNurbs->getPoint(1.)[1];

	if (u.size() > 0)
		return u.first();
	else
	{
		double re = (M - MLast) / MLast;
		if (abs(re) < 0.05)
			return 1.;
		eprintf(" There are errors.");
	}
	return 0.;
}

int exp_BladeImportedByParameterization::getSignOfCuttedSideAtTE(double span) // Very important
{
	curve_Polygon* poly_mBeta = getCambermBetaPolygonCurve(span);

	double beta2b = (poly_mBeta->Double2At(poly_mBeta->size() - 1))[1];

  int sign = 1;

  if (beta2b > 0)
    sign = -1;
  else if (beta2b < 0)
    sign = 1;

  return sign;
}

double exp_BladeImportedByParameterization::getMFracOfTEShread(double span)
{
	/*
  M  <--->  m  <--->  mFrac
			|
			|
			v
		  (z,r)

	T = thickness/r;

  */

  // 0.
	curve_Nurbs* pZRNurbs = getZRNurbsCurve(span);
	curve_Nurbs* pMThetaNurbs = getCamberMThetaNurbsCurve(span);
	curve_Nurbs* pMThinkNurbs = getMFracThicknessNurbsCurve(span);

	double mTotalLength = getMeridonalLength(span);

	int sign = getSignOfCuttedSideAtTE(span);


	double x0 = (pMThetaNurbs->getPoint(1.))[0];
	auto getDelta = [&](double u)->double
	{
		// 0.
		Double2 pos = pMThetaNurbs->getPoint(u);
		Double2 T = pMThetaNurbs->getTangent(u);
		T /= T.length();

		double Mcurrent = pos[0];
		double mFrac = pMFracMOfNurbs(Mcurrent, span);

		// update
		double equivalentThinkness = getEquivalentThinkness(mFrac, span);

		Double2 pressure = pos + 0.5 * equivalentThinkness * (T.rotate(sign * PI / 2.));

		double delta = (pressure[0] - x0);
		return delta;
	};

	//
	double equivalentThinkness = getEquivalentThinkness(1., span);

	QVector<double> uScope = QVector<double>() << 0.9 << 0.95 << 1;
	QVector<double> delta; delta.resize(3);

	double uFound = 0;

	double tol = 1.E-3;
	int steps = 50;
	for (int i = 0; i < steps; i++)
	{

		for (int j = 0; j < 3; j++)
		{
			delta[j] = getDelta(uScope[j]);
		}


		if (abs(delta[1]) < tol * equivalentThinkness && delta[1] < 0)
		{
			uFound = uScope[1];
			break;
		}

		if (delta[1] < 0.)
		{
			uScope[0] = uScope[1];
			delta[0] = delta[1];
		}
		else if (delta[1] > 0.)
		{
			uScope[2] = uScope[1];
			delta[2] = delta[1];
		}
		uScope[1] = (uScope[0] + uScope[2]) / 2.;

		if (i == steps - 1)
		{
			eprintf("  Can't find TE location");
		}

	}
	return uFound;
}

#if 0
double exp_BladeImportedByParameterization::getMFracOfTEShread(double span)
{
	/*


	M  <--->  m  <--->  mFrac
			|
			|
			v
			(z,r)

	T = thickness/r;

	*/

	// 0.
	curve_Nurbs* pZRNurbs = getZRNurbsCurve(span);
	curve_Nurbs* pMThetaNurbs = getCamberMThetaNurbsCurve(span);
	curve_Nurbs* pMThinkNurbs = getMFracThicknessNurbsCurve(span);
	double mTotalLength = getMeridonalLength(span);

	int signPressure = getPressureSignAlongCamberCurve(span);


	double x0 = (pMThetaNurbs->getPoint(1.))[0];
	auto getDelta = [&](double u)->double
	{
		// 0.
		Double2 pos = pMThetaNurbs->getPoint(u);
		Double2 T = pMThetaNurbs->getTangent(u);
		T /= T.length();

		double Mcurrent = pos[0];
		double mFrac = pMFracMOfNurbs(Mcurrent, span);

		// update
		double equivalentThinkness = getEquivalentThinkness(mFrac, span);

		Double2 pressure = pos + 0.5 * equivalentThinkness * (T.rotate(signPressure * PI / 2.));

		double delta = (pressure[0] - x0);
		return delta;
	};

	//
	double equivalentThinkness = getEquivalentThinkness(1., span);

	QVector<double> uScope = QVector<double>() << 0.9 << 0.95 << 1;
	QVector<double> delta; delta.resize(3);

	double uFound = 0;

	double tol = 1.E-3;
	int steps = 50;
	for (int i = 0; i < steps; i++)
	{

		for (int j = 0; j < 3; j++)
		{
			delta[j] = getDelta(uScope[j]);
		}


		if (abs(delta[1]) < tol * equivalentThinkness && delta[1] < 0)
		{
			uFound = uScope[1];
			break;
		}

		if (delta[1] < 0.)
		{
			uScope[0] = uScope[1];
			delta[0] = delta[1];
		}
		else if (delta[1] > 0.)
		{
			uScope[2] = uScope[1];
			delta[2] = delta[1];
		}
		uScope[1] = (uScope[0] + uScope[2]) / 2.;

		if (i == steps - 1)
		{
			eprintf("  Can't find TE location");
		}

	}
	return uFound;
}


#endif


QString exp_BladeImportedByParameterization::getString(double span)
{
	return QString("-%1").arg((int)((span + 1.e-6)));
}

curve_Topology* exp_BladeImportedByParameterization::getZRCurveTopology(bool createIfNotAvailable)
{
	QString name = "ZRCurveTopology";
	curve_Topology* topology = dynamic_cast<curve_Topology*> (object(name));
	if (topology)
		return topology;
	if (!topology && createIfNotAvailable)
	{
		topology = dynamic_cast<curve_Topology*>(TObject::new_object("curve_Topology", name, this));
		return topology;
	}
	return nullptr;
}

curve_Topology* exp_BladeImportedByParameterization::getMFracThicknessCurvesTopology(bool createIfNotAvailable)
{
	QString name = "MFracThicknessCurvesTopology";
	curve_Topology* topology = dynamic_cast<curve_Topology*> (object(name));
	if (topology)
		return topology;
	if (!topology && createIfNotAvailable)
	{
		topology = dynamic_cast<curve_Topology*>(TObject::new_object("curve_Topology", name, this));
		return topology;
	}
	return nullptr;
}

curve_Topology* exp_BladeImportedByParameterization::getMThetaCurveTopology(bool createIfNotAvailable)
{
	QString name = "MRThetaCurveTopology";
	curve_Topology* topology = dynamic_cast<curve_Topology*> (object(name));
	if (topology)
		return topology;
	if (!topology && createIfNotAvailable)
	{
		topology = dynamic_cast<curve_Topology*>(TObject::new_object("curve_Topology", name, this));
		return topology;
	}
	return nullptr;
}

curve_Curve* exp_BladeImportedByParameterization::getZRCurve(double span, bool createIfNotAvailable)
{
	QString name = "ZRCurve_" + getString(span);

	curve_Topology* T = getZRCurveTopology();
	if (!T)
		return nullptr;

	curve_Curve* c = dynamic_cast<curve_Curve*>(T->child(name));
	if (c)
		return c;

	if (!c && createIfNotAvailable)
	{
		c = (curve_Curve*)TObject::new_object("curve_Curve", name, T);
		return c;
	}

	return nullptr;
}

curve_Polygon* exp_BladeImportedByParameterization::getZRPolgonCurve(double span, bool createIfNotAvailable)
{
	QString name = "ZRPolgonCurve_" + getString(span);

	curve_Curve* c = getZRCurve(span)->getCurveByName(name);

	if (!c && createIfNotAvailable)
		c = getZRCurve(span)->addSegment(curve_Curve::Polygon, name);
	if (c)
	{
		curve_Polygon* p = dynamic_cast<curve_Polygon*>(c);
		return p;
	}
	return nullptr;
}

curve_Nurbs* exp_BladeImportedByParameterization::getZRNurbsCurve(double span, bool createIfNotAvailable)
{
	QString name = "ZRNurbsCurve_" + getString(span);

	curve_Curve* c = getZRCurve(span)->getCurveByName(name);

	if (!c && createIfNotAvailable)
		c = getZRCurve(span)->addSegment(curve_Curve::Nurbs, name);
	if (c)
	{
		curve_Nurbs* n = dynamic_cast<curve_Nurbs*>(c);
		return n;
	}
	return nullptr;
}

curve_Curve* exp_BladeImportedByParameterization::getMFracThicknessCurve(double span, bool createIfNotAvailable)
{
	QString name = "MFracThicknessCurve_" + getString(span);

	curve_Topology* T = getMFracThicknessCurvesTopology();
	if (!T)
		return nullptr;

	curve_Curve* c = dynamic_cast<curve_Curve*>(T->child(name));
	if (c)
		return c;

	if (!c && createIfNotAvailable)
	{
		c = (curve_Curve*)TObject::new_object("curve_Curve", name, T);
		return c;
	}

	return nullptr;
}

curve_Polygon* exp_BladeImportedByParameterization::getMFracThicknessPolgonCurve(double span, bool createIfNotAvailable)
{
	QString name = "MFracThicknessPolgonCurve_" + getString(span);

	curve_Curve* c = getMFracThicknessCurve(span)->getCurveByName(name);

	if (!c && createIfNotAvailable)
		c = getMFracThicknessCurve(span)->addSegment(curve_Curve::Polygon, name);
	if (c)
	{
		curve_Polygon* p = dynamic_cast<curve_Polygon*>(c);
		return p;
	}
	return nullptr;
}

curve_Nurbs* exp_BladeImportedByParameterization::getMFracThicknessNurbsCurve(double span, bool createIfNotAvailable)
{
	QString name = "MFracThicknessNurbsCurve_" + getString(span);

	curve_Curve* c = getMFracThicknessCurve(span)->getCurveByName(name);

	if (!c && createIfNotAvailable)
		c = getMFracThicknessCurve(span)->addSegment(curve_Curve::Nurbs, name);
	if (c)
	{
		curve_Nurbs* n = dynamic_cast<curve_Nurbs*>(c);
		return n;
	}
	return nullptr;
}

curve_Curve* exp_BladeImportedByParameterization::getCamberMThetaCurve(double span, bool createIfNotAvailable)
{
	QString name = "MRThetaCurve_" + getString(span);
	curve_Topology* T = getMThetaCurveTopology();
	if (!T)
		return nullptr;

	curve_Curve* c = dynamic_cast<curve_Curve*>(T->child(name));
	if (c)
		return c;

	if (!c && createIfNotAvailable)
	{
		c = (curve_Curve*)TObject::new_object("curve_Curve", name, T);
		return c;
	}

	return nullptr;
}

curve_Polygon* exp_BladeImportedByParameterization::getCamberMThetaPolygonCurve(double span, bool createIfNotAvailable)
{
	QString name = "MRThetaPolygonCurve_" + getString(span);

	curve_Curve* c = getCamberMThetaCurve(span)->getCurveByName(name);

	if (!c && createIfNotAvailable)
		c = getCamberMThetaCurve(span)->addSegment(curve_Curve::Polygon, name);
	if (c)
	{
		curve_Polygon* p = dynamic_cast<curve_Polygon*>(c);
		return p;
	}
	return nullptr;
}

curve_Nurbs* exp_BladeImportedByParameterization::getCamberMThetaNurbsCurve(double span, bool createIfNotAvailable)
{
	QString name = "MRThetaNurbsCurve_" + getString(span);

	curve_Curve* c = getCamberMThetaCurve(span)->getCurveByName(name);

	if (!c && createIfNotAvailable)
		c = getCamberMThetaCurve(span)->addSegment(curve_Curve::Nurbs, name);
	if (c)
	{
		curve_Nurbs* n = dynamic_cast<curve_Nurbs*>(c);
		return n;
	}
	return nullptr;
}

curve_Topology* exp_BladeImportedByParameterization::getmBetaCurveTopology(bool createIfNotAvailable)
{
	QString name = "mBetaCurveTopology";
	curve_Topology* topology = dynamic_cast<curve_Topology*> (object(name));
	if (topology)
		return topology;
	if (!topology && createIfNotAvailable)
	{
		topology = dynamic_cast<curve_Topology*>(TObject::new_object("curve_Topology", name, this));
		return topology;
	}
	return nullptr;
}

curve_Curve* exp_BladeImportedByParameterization::getCambermBetaCurve(double span, bool createIfNotAvailable)
{
	QString name = "mBetaCurve_" + getString(span);
	curve_Topology* T = getmBetaCurveTopology();
	if (!T)
		return nullptr;

	curve_Curve* c = dynamic_cast<curve_Curve*>(T->child(name));
	if (c)
		return c;

	if (!c && createIfNotAvailable)
	{
		c = (curve_Curve*)TObject::new_object("curve_Curve", name, T);
		return c;
	}

	return nullptr;
}

curve_Polygon* exp_BladeImportedByParameterization::getCambermBetaPolygonCurve(double span, bool createIfNotAvailable)
{
	QString name = "mBetaPolygonCurve_" + getString(span);

	curve_Curve* c = getCambermBetaCurve(span)->getCurveByName(name);

	if (!c && createIfNotAvailable)
		c = getCambermBetaCurve(span)->addSegment(curve_Curve::Polygon, name);
	if (c)
	{
		curve_Polygon* p = dynamic_cast<curve_Polygon*>(c);
		return p;
	}
	return nullptr;
}

curve_Curve* exp_BladeImportedByParameterization::getCamberMFracMCurve(double span, bool createIfNotAvailable)
{
	QString name = "camberMFracMCurve" + getString(span);

	curve_Curve* c = dynamic_cast<curve_Curve*>(this->child(name));
	if (c)
		return c;

	if (!c && createIfNotAvailable)
	{
		c = (curve_Curve*)TObject::new_object("curve_Curve", name, this);
		return c;
	}

	return nullptr;
}

curve_Nurbs* exp_BladeImportedByParameterization::getCamberMFracM(double span, bool createIfNotAvailable)
{
	QString name = "mFracMNurbsCurve_" + getString(span);

	curve_Curve* c = getCamberMFracMCurve(span)->getCurveByName(name);

	if (!c && createIfNotAvailable)
		c = getCamberMFracMCurve(span)->addSegment(curve_Curve::Nurbs, name);
	if (c)
	{
		curve_Nurbs* n = dynamic_cast<curve_Nurbs*>(c);
		return n;
	}
	return nullptr;
}

QString exp_BladeImportedByParameterization::getCurveNameOfProfile(int position)
{
	QStringList nameList = QStringList() << "LE" << "Pressure" << "TE" << "Suction";
	return nameList[position];
}

curve_Curve* exp_BladeImportedByParameterization::getProfileMThetaCurves(double span, bool createIfNotAvailable)
{
	QString name = "profileMRThetaCurves" + getString(span);
	curve_Topology* T = getMThetaCurveTopology();
	if (!T)
		return nullptr;

	curve_Curve* c = dynamic_cast<curve_Curve*>(T->child(name));
	if (c)
		return c;

	if (!c && createIfNotAvailable)
	{
		c = (curve_Curve*)TObject::new_object("curve_Curve", name, T);
		return c;
	}
	return nullptr;
}

curve_Nurbs* exp_BladeImportedByParameterization::getProfileMThetaNurbsCurve(double span, int position, bool createIfNotAvailable)
{
	QString name = getCurveNameOfProfile(position) + "OfProfileMRThetaNurbsCurve" + getString(span);

	curve_Curve* c = getProfileMThetaCurves(span)->getCurveByName(name);

	if (!c && createIfNotAvailable)
		c = getProfileMThetaCurves(span)->addSegment(curve_Curve::Nurbs, name);
	if (c)
	{
		curve_Nurbs* n = dynamic_cast<curve_Nurbs*>(c);
		return n;
	}
	return nullptr;
}

curve_Polygon* exp_BladeImportedByParameterization::getProfileMThetaPolygonCurve(double span, int position, bool createIfNotAvailable)
{
	QString name = getCurveNameOfProfile(position) + "OfProfileMRThetaPolygonCurve" + getString(span);

	curve_Curve* c = getProfileMThetaCurves(span)->getCurveByName(name);

	if (!c && createIfNotAvailable)
		c = getProfileMThetaCurves(span)->addSegment(curve_Curve::Polygon, name);
	if (c)
	{
		curve_Polygon* p = dynamic_cast<curve_Polygon*>(c);
		return p;
	}
	return nullptr;
}

int exp_BladeImportedByParameterization::getSignOfCamberCurve(double span)
{
	curve_Nurbs* camberMTheta = getCamberMThetaNurbsCurve(span);

	int methodType = 1;
	if (methodType == 0)
	{
		Double2 startPoint = camberMTheta->getPoint(0.);
		Double2 endPoint = camberMTheta->getPoint(1.);
		if (startPoint[1] > endPoint[1])
			return -1;
	}
	else
	{
		Double2 T = camberMTheta->getTangent(0.);
		double slopAngle = Double2(1., 0.).angleSigned(T);
		if(slopAngle < 0)
			return -1;
	}

	return 1;
}

int exp_BladeImportedByParameterization::getPressureSignAlongCamberCurve(double span)
{
	int sign = 0;
	if (getSignOfCamberCurve(span) > 0)
		sign = -1;
	else
		sign = 1;
	return sign;
}

int exp_BladeImportedByParameterization::getSuctionSignAlongCamberCurve(double span)
{
	int sign = 0;
	if (getSignOfCamberCurve(span) > 0)
		sign = 1;
	else
		sign = -1;
	return sign;
}

void exp_BladeImportedByParameterization::createLECurve(double ule, double span)
{
	// 0.
	curve_Nurbs* camberMTheta = getCamberMThetaNurbsCurve(span);
	curve_Nurbs* mThick = getMFracThicknessNurbsCurve(span);
	double mTotalLength = getMeridonalLength(span);

	// 1. 
	Double2 centerPos = camberMTheta->getPoint(ule);
	Double2 T = camberMTheta->getTangent(ule);
	double M = centerPos[0];

	double mFrac = pMFracMOfNurbs(M, span);

	double equivalentThinkness = getEquivalentThinkness(mFrac, span);

	//2. from pressure to suction
	int sign = getSignOfCamberCurve(span);
	Double2 startPos = centerPos + 0.5 * equivalentThinkness * (T / T.length()).rotate(getSuctionSignAlongCamberCurve(span) * PI / 2.);

	int np = 51;
	QVector<Double2> LEs;
	Double2 vec = startPos - centerPos;
	for (int i = 0; i < np; i++)
	{
		double angle = sign * PI / (np - 1.) * i;
		Double2 le = centerPos + vec.rotate(angle);
		LEs.push_back(le);
	}

	//3. 
	//3.1
	curve_Polygon* lePoly = getProfileMThetaPolygonCurve(span, 0);
	lePoly->setDouble2Vector(LEs);
	// 3.2
	curve_Nurbs* leNurbs = getProfileMThetaNurbsCurve(span, 0);
	leNurbs->fitBezier(LEs, 5);
}

void exp_BladeImportedByParameterization::createTECurve(double span)
{
	if (!getProfileMThetaNurbsCurve(span, 3, false) || !getProfileMThetaNurbsCurve(span, 1, false))
	{
		return;
	}

	Double2 startPos = getProfileMThetaNurbsCurve(span, 1)->getPoint(1.);
	Double2 endPos = getProfileMThetaNurbsCurve(span, 3)->getPoint(1.);

	int np = 51;
	QVector<Double2> TEs;
	for (int i = 0; i < np; i++)
	{
		double t = i / (np - 1.);
		Double2 te = (1 - t) * startPos + t * endPos;
		TEs.push_back(te);
	}

	//3. 
	//3.1
	int curentPostion = 2;
	curve_Polygon* pressurePoly = getProfileMThetaPolygonCurve(span, curentPostion);
	pressurePoly->setDouble2Vector(TEs);
	// 3.2
	curve_Nurbs* pressureNurbs = getProfileMThetaNurbsCurve(span, curentPostion);
	pressureNurbs->fitBezier(TEs, 5);
}

void exp_BladeImportedByParameterization::createPressureCurve(double ule, double ute, double span)
{
	// 1.
	curve_Nurbs* camber = getCamberMThetaNurbsCurve(span);
	curve_Nurbs* mThick = getMFracThicknessNurbsCurve(span);

	//2. from le to te
	int sign = getPressureSignAlongCamberCurve(span);

	int np = 101;
	QVector<Double2> pressures;
	for (int i = 0; i < np; i++)
	{
		// 1.
		double t = i / (np - 1.);
		double u = (1 - t) * ule + t * ute;

		// 2.
		Double2 pos = camber->getPoint(u);
		Double2 T = camber->getTangent(u);

		double mFrac = pMFracMOfNurbs(pos[0], span);

		double equivalentThinkness = getEquivalentThinkness(mFrac, span);

		// 3.
		Double2 pressurePos = pos + 0.5 * equivalentThinkness * (T / T.length()).rotate(sign * PI / 2.);

		pressures.push_back(pressurePos);
	}

	//3. 
	//3.1
	int pressureIndex = 1;
	curve_Polygon* pressurePoly = getProfileMThetaPolygonCurve(span, pressureIndex);
	pressurePoly->setDouble2Vector(pressures);

}

void exp_BladeImportedByParameterization::createSuctionCurve(double ule, double ute, double span)
{
	// 1.
	curve_Nurbs* camber = getCamberMThetaNurbsCurve(span);
	curve_Nurbs* mThick = getMFracThicknessNurbsCurve(span);

	//2. from le to te
	int sign = getSuctionSignAlongCamberCurve(span);

	int np = 101;
	QVector<Double2> suctions;
	for (int i = 0; i < np; i++)
	{
		// 1.
		double t = i / (np - 1.);
		double u = (1 - t) * ule + t * ute;

		// 2.
		Double2 pos = camber->getPoint(u);
		Double2 T = camber->getTangent(u);

		double mFrac = pMFracMOfNurbs(pos[0], span);

		double equivalentThinkness = getEquivalentThinkness(mFrac, span);

		// 3.
		Double2 suction = pos + 0.5 * equivalentThinkness * (T / T.length()).rotate(sign * PI / 2.);

		suctions.push_back(suction);
	}

	//3. 
	int suctionIndex = 3;
	//3.1
	curve_Polygon* suctionPoly = getProfileMThetaPolygonCurve(span, suctionIndex);
	suctionPoly->setDouble2Vector(suctions);

}

QString exp_BladeImportedByParameterization::getCuttedSideName(double span)
{
	QString cuttedSideName = "pressure";
	curve_Polygon* poly_mBeta = getCambermBetaPolygonCurve(span);
	double beta2b = (poly_mBeta->Double2At(poly_mBeta->size() - 1))[1];

	if (_rotationDirection == CCW)
	{
		if(beta2b > 0)
			cuttedSideName = "suction";
		else
			cuttedSideName = "pressure";
	}
	else if (_rotationDirection == CW)
	{
		if (beta2b > 0)
			cuttedSideName = "pressure";
		else
			cuttedSideName = "suction";
	}

	return cuttedSideName;
}

QString exp_BladeImportedByParameterization::getExtendSideName(double span)
{
	QString cuttedSideName = getCuttedSideName(span);

	QString extendSideName;
	if(cuttedSideName == "suction")
		extendSideName = "pressure";
	else if (cuttedSideName == "pressure")
		extendSideName = "suction";
	return extendSideName;
}

void exp_BladeImportedByParameterization::extendSideCurve(double span)
{
	double Mend = (getCamberMFracM(span)->getPoint(1.))[1];

	int sideindex = 0;
	if (getExtendSideName(span) == "pressure")
	{
		int pressureIndex = 1;
		sideindex = pressureIndex;
	}
	else if (getExtendSideName(span) == "suction")
	{
		int suctionIndex = 3;
		sideindex = suctionIndex;
	}

	curve_Polygon* extendSideCurve = getProfileMThetaPolygonCurve(span, sideindex);
	// 2.
	QVector<Double2> datas;
	for (int i = 0; i < extendSideCurve->size(); i++)
	{
		datas.push_back(extendSideCurve->Double2At(i));
	}
	// 3. extend
	Double2 T = extendSideCurve->getTangent((extendSideCurve->size()-1));
	double beta_end = Double2(1., 0.).angleSigned(T);

	Double2 ptRear = datas.last();

	double dM = Mend - ptRear[0];

	double dTheta = dM * tan(beta_end);

	Double2 ptEnd = ptRear + Double2(dM, dTheta);

	int np_extent = 1;
	for (int i = 1; i <= np_extent; i++)
	{
		double t = i / (1.*np_extent);
		Double2 pt = (1 - t) * ptRear + t * ptEnd;
		datas.push_back(pt);
	}
	extendSideCurve->setDouble2Vector(datas);

}

void exp_BladeImportedByParameterization::cuttedSideCurve(double span)
{
	double Mend = (getCamberMFracM(span)->getPoint(1.))[1];

	int sideindex = 0;
	if (getCuttedSideName(span) == "pressure")
	{
		int pressureIndex = 1;
		sideindex = pressureIndex;
	}
	else if (getCuttedSideName(span) == "suction")
	{
		int suctionIndex = 3;
		sideindex = suctionIndex;
	}

	curve_Polygon* cuttedSideCurve = getProfileMThetaPolygonCurve(span, sideindex);

	// 2.
	QVector<Double2> datas;
	for (int i = 0; i < cuttedSideCurve->size(); i++)
	{
		datas.push_back(cuttedSideCurve->Double2At(i));
	}
	// 3.
	bool found = false;
	int size = datas.size();
	for (int i = 0; i < datas.size()-1; i++)
	{
		int j = size - 1 - i;
		Double2 ptNow = datas[j];
		Double2 ptNext = datas[j-1];
		if (ptNow[0] > Mend && ptNext[0] < Mend)
		{
			found = true;
		}
		if (!found)
		{
			datas.removeLast();
		}
		else
		{
			double t = ( Mend- ptNext[0])/(ptNow[0] - ptNext[0]);
			Double2 ptInsert = (1 - t) * ptNext + t * ptNow;
			datas.removeLast();
			datas.push_back(ptInsert);
			break;
		}
	}

	cuttedSideCurve->setDouble2Vector(datas);
}

void exp_BladeImportedByParameterization::fitSideCurve(double span)
{
	QVector<Double2> datas;

	// 1.
	int pressureIndex = 1;
	{
		curve_Polygon* pressurePoly = getProfileMThetaPolygonCurve(span, pressureIndex);
		for (int i = 0; i < pressurePoly->size(); i++)
		{
			datas.push_back(pressurePoly->Double2At(i));
		}

		curve_Nurbs* pressureNurbs = getProfileMThetaNurbsCurve(span, pressureIndex);
		pressureNurbs->fitBezier(datas, 11);
	}


	datas.clear();
	// 2.
	int suctionIndex = 3;
	{
		curve_Polygon* suctionPoly = getProfileMThetaPolygonCurve(span, suctionIndex);
		for (int i = 0; i < suctionPoly->size(); i++)
		{
			datas.push_back(suctionPoly->Double2At(i));
		}

		curve_Nurbs* suctionNurbs = getProfileMThetaNurbsCurve(span, suctionIndex);
		suctionNurbs->fitBezier(datas, 11);
	}


}

double exp_BladeImportedByParameterization::getMFromMFrac(double span, double mFrac, double mFrac_start)
{
	if (mFrac == 0.)
		return 0.;
	// 0.
	curve_Nurbs* zrCurve = getZRNurbsCurve(span);
	double totalLength = getMeridonalLength(span);
	double uMfrac = zrCurve->getUFromLength(mFrac * totalLength);

	double uMfrac_start = 0.;
	if (mFrac_start != 0.)
		uMfrac_start = zrCurve->getUFromLength(mFrac_start * totalLength);

	// 1.
	int np = 101;
	Double2 zr_last = zrCurve->getPoint(uMfrac_start);
	Double2 zr_now;
	double M = 0.;
	double du = (uMfrac - uMfrac_start) / (np - 1.);
	for (int i = 1; i < np; i++)
	{
		double uNow = i * du + uMfrac_start;
		zr_now = zrCurve->getPoint(uNow);
		double dm = zrCurve->calculateLength(uNow - du, uNow);
		double dM = dm / (0.5 * (zr_last + zr_now)[1]);
		M += dM;
		zr_last = zr_now;
	}

	return M;
}

double exp_BladeImportedByParameterization::get_mfracFromM(double span, double M)
{

	curve_Nurbs* MFracMNurbs = getCamberMFracM(span);
	Double2 mFracM_start = MFracMNurbs->getPoint(0.);
	Double2 mFracM_end = MFracMNurbs->getPoint(1.);

	if (M <= mFracM_start[1])
		return mFracM_start[0];
	if (M >= mFracM_end[1])
		return mFracM_end[0];

	QVector<double> mfracs = MFracMNurbs->getXFromY(M);

	double mFrac = 0.;
	if (mfracs.size())
		mFrac = mfracs[0];

	return mFrac;
}

QVector<Double3> exp_BladeImportedByParameterization::get3Ds(curve_Polygon* pMTheta, double span)
{
	curve_Nurbs* zrCurve = getZRNurbsCurve(span);
	double mLength = getMeridonalLength(span);

	QVector<Double3> xyzs;
	bool found = false;
	Double2 xAxis = { 1., 0. };
	for (int i = 0; i < pMTheta->size(); i++)
	{
		Double2 MTheta = pMTheta->Double2At(i);

		double mFrac = pMFracMOfNurbs(MTheta[0], span);

		double u = zrCurve->getUFromLength(mFrac * mLength, &found);
		if (!found)
			break;

		Double2 zr = zrCurve->getPoint(u);
		double theta = MTheta[1];

	// 	Double2 xy = zr[1] * xAxis.rotate(MTheta[1]);
		Double3 xyz = get3DAtReferenceAxis(zr, theta);

		xyzs.push_back(xyz);
	}

	return xyzs;
}

Double3 exp_BladeImportedByParameterization::get3DAtReferenceAxis(Double2 zr, double theta)
{
	Double2 xy = zr[1] * Double2(1., 0.).rotate(_referenceAxis*theta);

	Double3 xyz = Double3(xy[0], xy[1], zr[0]);;

	return xyz;
}

QVector<Double3> exp_BladeImportedByParameterization::getProfile(double span)
{
	QVector<Double3> profile;

	QVector<Double3> le_profile = get3Ds(getProfileMThetaPolygonCurve(span, 0), span);;
	for (int i = 1; i < le_profile.size(); i++)
	{
		profile.push_back(le_profile[i]);
	}

	QVector<Double3> pressure_profile = get3Ds(getProfileMThetaPolygonCurve(span, 1), span);
	for (int i = 1; i < pressure_profile.size(); i++)
	{
		profile.push_back(pressure_profile[i]);
	}

	QVector<Double3> te_profile = get3Ds(getProfileMThetaPolygonCurve(span, 2), span);;
	for (int i = 1; i < te_profile.size(); i++)
	{
		profile.push_back(te_profile[i]);
	}

	QVector<Double3> suction_profile = get3Ds(getProfileMThetaPolygonCurve(span, 3), span);
	int size = suction_profile.size();
	for (int i = 1; i < suction_profile.size(); i++)
	{
		int j = (size - 1) - i;
		profile.push_back(suction_profile[j]);
	}
	return profile;
}

QVector<QVector<Double3>> exp_BladeImportedByParameterization::getProfile1(double span)
{
	QVector<QVector<Double3>> profile;

	QVector<Double3> le_profile = get3Ds(getProfileMThetaPolygonCurve(span, 0), span);;
	profile.push_back(le_profile);

	QVector<Double3> pressure_profile = get3Ds(getProfileMThetaPolygonCurve(span, 1), span);
	profile.push_back(pressure_profile);

	QVector<Double3> te_profile = get3Ds(getProfileMThetaPolygonCurve(span, 2), span);;
	profile.push_back(te_profile);

	QVector<Double3> suction_profile = get3Ds(getProfileMThetaPolygonCurve(span, 3), span);
	profile.push_back(suction_profile);

	return profile;
}

QVector<Double3> exp_BladeImportedByParameterization::getFlowPath(double span)
{
	QVector<Double3> xyz;

	int type = 1;
	if (type == 0)
	{
		curve_Polygon* zrPolygon = getZRPolgonCurve(span);

		for (int i = 0; i < zrPolygon->size(); i++)
		{
			Double2 zr = zrPolygon->Double2At(i);
			xyz.push_back(Double3(0., zr[1], zr[0]));
		}
	}
	else
	{
		curve_Nurbs* zrNurbs = getZRNurbsCurve(span);
		int npts = 51;
		for (int i= 0; i< npts; i++)
		{
			double u = i / (npts -1.);
			Double2 zr = zrNurbs->getPoint( u);
			xyz.push_back(Double3(0., zr[1], zr[0]));
		}
	}

	return xyz;
}

double exp_BladeImportedByParameterization::coeffienceWithLengthUnitExported()
{
	double k = 1.;
	if (lengthUnitExported == "mm")
		k = 1000.;
	return k;
}

void exp_BladeImportedByParameterization::exportToFile(QString fileName)
{
	ofstream xSectionFile;
	xSectionFile.open(fileName.ascii());

	xSectionFile << "Type Radial Compressor" << endl;

	xSectionFile << "Unit " << lengthUnitExported.toStdString() <<endl;

	xSectionFile << endl;

	if (false) // 
		xSectionFile << "Shrouded Yes " << endl;
	else
		xSectionFile << "Shrouded No " << endl;

	xSectionFile << "Tip clearance LE " << 0 << endl;
	xSectionFile << "Tip clearance TE " << 0 << endl;

	xSectionFile << "Rotation direction " << "Counter Clockwise" << endl;

	xSectionFile << endl;

	xSectionFile << "Blade Main" << endl;
	xSectionFile << "Main Blades " << _bladeNumOfMainBladeRow << endl;
	xSectionFile << "Sections " << 2 << endl;

	QStringList edgeName = QStringList() << "Leading_edge" << "Pressure_side" << "Trailing_edge" << "Suction_side";
	QVector<double> spanList = QVector<double>() << 0. << 100.;

	double coeffienceWithUnit = coeffienceWithLengthUnitExported();
	double mdeg = PI / 180.0;
	double rawdeg = 0;
	double newRad = 0;
	eprintf("deg is %f, radiu is %f", offsetTheta, mdeg);
	double mR = 0;
	for (int i = 0; i < spanList.size(); i++)
	{
		xSectionFile << endl;
		xSectionFile << "Section " << (i + 1) << endl;
		double span = spanList[i];
		

		QVector<QVector<Double3>> profile = getProfile1(span);
		for (int j = 0; j < edgeName.size(); j++)
		{
			QVector<Double3> edge = profile[j];
			if (!edge.empty())
			{
				xSectionFile << edgeName[j].toStdString() << endl;
				xSectionFile << "x	y	z" << endl;
				for (int i = 0; i < edge.size(); i++)
				{
					Double3 pt = edge[i]* coeffienceWithUnit;
					//mR = sqrt(pt[0] * pt[0] + pt[1] * pt[1]);
					//rawdeg = acos(pt[0] / mR);
					//newRad = (rawdeg / PI * 180 + offsetTheta) * PI/180;

					Double2 pt2D = { pt[0], pt[1]};
					pt2D = pt2D.rotate(offsetTheta);

					xSectionFile << pt2D[0] << "\t" << pt2D[1] << "\t" << pt[2] << endl;
				}
				xSectionFile << endl;
			}

		}
	}

	//Write hub & shroud flowpath curve data
	xSectionFile << endl;
	xSectionFile << "Mechanical" << endl;
	QStringList flowPathName = QStringList() << "Hub" << "Shroud";

	for (int i = 0; i < spanList.size(); i++)
	{
		double span = spanList[i];
		QVector<Double3> flowPath = getFlowPath(span);

		xSectionFile << flowPathName[i].toStdString() << endl;
		xSectionFile << "x	y	z" << endl;
		for (int j = 0; j < flowPath.size(); j++)
		{
			Double3 xyz = flowPath[j]* coeffienceWithUnit;
			mR = sqrt(xyz[0] * xyz[0] + xyz[1] * xyz[1]);
			rawdeg = acos(xyz[0] / mR);
			newRad = (rawdeg / PI * 180 + offsetTheta) * mdeg;
			//xSectionFile << xyz[0] << "\t" << xyz[1] << "\t" << xyz[2] << endl;
			xSectionFile << mR * cos(newRad) << "\t" << mR * sin(newRad) << "\t" << xyz[2] << endl;
		}
		xSectionFile << endl;
	}

	xSectionFile.close();
}
