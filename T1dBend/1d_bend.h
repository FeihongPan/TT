/*************************************************************************
 *
 * TURBOTIDES
 * An Integrated CAE Platform for Turbomachinery Design and Development
 * ____________________________________________________________________
 *
 *  [2016] - [2020] TaiZe Inc
 *  All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of TaiZe Inc and its suppliers, if any.
 * The intellectual and technical concepts contained herein
 * are proprietary to TaiZe Inc and its suppliers and may
 * be covered by U.S. and Foreign Patents, patents in process,
 * and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this
 * material is strictly forbidden unless prior written
 * permission is obtained from TaiZe Inc.
 */

#ifndef ONED_BEND_H
#define ONED_BEND_H
#include "1d_vaneless.h"

class T_EXPORT_1D T1dBend : public T1dVaneless
{
	T_OBJECT;
public:
	T1dBend(QString object_n = "", TObject* iparent = NULL);
	~T1dBend();

public:
	virtual void InitGeom(T1dStation* sta_in);
	void checkingConstraint();
	virtual void LineupComp(double Za);

public:
	virtual void UpdateContour();
	void DrawBend();
	virtual void createItemPlot(bool modify = false);

public:
	virtual int ExitBlockage(double& blockage);
	virtual int Analysis();
	virtual int PrelimSizing(double phi);

protected:
	virtual void GetConfigPassagePropertyList(QVector<property_t*>& properties, int source = 0);
	virtual void GetConfigGeometryPropertyList(QVector<property_t*>& properties, int source = 0);
};

#endif // !ONED_BEND_H
