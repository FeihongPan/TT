/*************************************************************************
 *
 * TURBOTIDES
 * An Integrated CAE Platform for Turbomachinery Design and Development
 * ____________________________________________________________________
 *
 *  [2016] - [2021] TurboTides LLC
 *  All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of TurboTides LLC and its suppliers, if any.
 * The intellectual and technical concepts contained herein
 * are proprietary to TurboTides LLC and its suppliers and may
 * be covered by U.S. and Foreign Patents, patents in process,
 * and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this
 * material is strictly forbidden unless prior written
 * permission is obtained from TurboTides LLC.
 */

#pragma once

#include "util_TObject.h"

class exp_BladeImport : public QObject, public TObject
{
	
	T_OBJECT;
	Q_OBJECT;
public:
	exp_BladeImport(QString object_n = "", TObject *iparent = NULL);
	virtual ~exp_BladeImport() {};
	
	QStringList getCSV(QString fileName);
	QVector<QVector<QString>>  getCSV(QString fileName, QString seprator);
	void  putCSV(QString fileName, QVector<QVector<QString>> txt,int row,int col);
	void convertXYZFromZ_RTheta_R(QVector<QVector<QString>> &txt, int row);
	void convertXYZFromZ_R_Theta(QVector<QVector<QString>> &txt, int row);
	void convertXYZFromXYZ(QVector<QVector<QString>> &txt, int row);
	QString getFileStem(QString fileName);

	QVector<QVector<double>> str2doubleVector(QVector<QVector<QString>>&txt, int row, int col = 3);

	QVector<QVector<Double3>> splitCurve(QVector<QVector<Double3>> profiles, int profileSize, int spliteFactor);

public:
	//QStringList names;
	//QStringList namedDataVector;
	//QVector<QVector<double>> dataVectorVector;
	QStringList fileNameList;
	QVector<int> splitNumList;
	QVector<int> TotNumList;



};