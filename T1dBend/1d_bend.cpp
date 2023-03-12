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

#include "1d_bend.h"
#include "1d_machine.h"

REGISTER_OBJECT_CLASS(T1dBend, "Bend", T1dVaneless);

T1dBend::T1dBend(QString object_n, TObject* iparent) :T1dVaneless(object_n, iparent)
{
	INIT_OBJECT;
	nType = type_bend;
	pinchOption = 0;
	tipContourType = 0;
	hubContourType = 0;
}

T1dBend::~T1dBend()
{

}

// 1.1
void T1dBend::InitGeom(T1dStation* sta_in)
{
	// 1. phi
	TObject::copyObject(in, sta_in, true, true);
	out->phi = 0. * PI / 180;
	double dphi_check = 90. * PI / 180;
	if (in->phi == 0) 
		out->phi = in->phi + dphi_check;
	double dphi = out->phi - in->phi;

	// 2. b
	out->b = in->b;
	double _sign = 1.;
	if (in->phi < 0)
		_sign = -1.;
	Double2 _b1_check = { 0., _sign * 1.5 * in->b / 2. };
	_b1_check = _b1_check.rotate(in->phi);
	Double2 _b2_check = _b1_check.rotate(dphi);
	double delta_R_check = _b2_check[1] - _b1_check[1];

	// 3. Ra
	// 3.1 Ra_ratio
	double Ra_ratio = 0;
	double Ra_ratio_check = delta_R_check / in->Ra + 1.;
	if (in->phi > -tolr1)
	{
		Ra_ratio = 1.2;
		if (Ra_ratio < Ra_ratio_check)
			Ra_ratio = Ra_ratio_check;
	}
	if (in->phi < -tolr1)
	{
		out->phi = in->phi + dphi;
		Ra_ratio = 0.8;
		if (Ra_ratio < Ra_ratio_check)
			Ra_ratio = Ra_ratio_check;
	}

	// 3.2 Ra
	out->Ra = Ra_ratio * in->Ra;
	double dR = out->Ra - in->Ra;

	// 4. L_axial
	if (fabs(in->phi) < tolr1) // axial
		L_axial = 1. * in->b;
	if (fabs(dphi) < tolr1) // radial
		L_axial = 0.;
	else
		L_axial = dR / tan(in->phi);
	//double dR = out->Ra - in->Ra;
	double dPhi = 0.5 * (in->phi + out->phi);
	if (!getUseGeomCurve())
		L_axial = dR * tan(PI / 2 - dPhi);

	// 5. Za
	out->Za = in->Za + L_axial;

	// 6.1 phi1_hub
	phi1h_set = true;
	phi1_hub = in->phi;
	// 6.2 phi1_shroud
	phi1s_set = true;
	phi1_shroud = in->phi;
	// 6.4 phi2_hub
	phi2h_set = true;
	phi2_hub = out->phi;
	// 6.3 phi2_shroud
	phi2s_set = true;
	phi2_shroud = out->phi;

	inletGeomOption = 0;
	outletGeomOption = 0;
	geomValid = true;
}

// 1.2
void T1dBend::checkingConstraint()
{
	if (inputRatio)
		fromRatio();
	// 1. check out->phi
	double d_phi = out->phi - in->phi;
	if (in->phi >= 0)
		if (fabs(d_phi) > 180 * PI / 180.) out->phi = in->phi - 90 * PI / 180.;
	else
		if (fabs(d_phi) > 180 * PI / 180.) out->phi = in->phi + 90 * PI / 180.;

	// 2. check out->Ra
	// 2.1 b
	double dphi = out->phi - in->phi;
	double _sign = 1.;
	if (in->phi < 0) 
		_sign = -1.;
	Double2 _b1_check = { 0., _sign * 1.5 * in->b / 2. };
	_b1_check = _b1_check.rotate(in->phi);
	Double2 _b2_check = _b1_check.rotate(dphi);
	double delta_R_check = _b2_check[1] - _b1_check[1];

	// 2.2 Ra
	double Ra_ratio = 0;
	if (isUserInput(this, "Rout_Rin"))
		Ra_ratio = Rout_Rin;
	else
		Ra_ratio = out->Ra / in->Ra;
	double Ra_ratio_check = delta_R_check / in->Ra + 1.;
	if (in->phi >= -tolr1) 
		if (Ra_ratio < Ra_ratio_check) Ra_ratio = Ra_ratio_check;
	else if (in->phi < -tolr1) 
		if (Ra_ratio > Ra_ratio_check) Ra_ratio = Ra_ratio_check;

	out->Ra = Ra_ratio * in->Ra;
	Rout_Rin = Ra_ratio;
}

// 1.3
void T1dBend::LineupComp(double Za)
{
	checkingConstraint();
	T1dComponent::LineupComp(Za);
}

// 2.1
void T1dBend::UpdateContour()
{
	if (getUseGeomCurve())//use geometry curve
	{
		updateContour4Geom();
		CreateInletOutlet();
		return;
	}
	DrawBend();
}

// 2.2
void T1dBend::DrawBend()
{
	UpdateContourBase();
}

// 2.3
void T1dBend::createItemPlot(bool modify)
{
	UpdateGeometry();
	collectPlotsIntoItems();
}

// 3.
int T1dBend::ExitBlockage(double& blockage)
{
	int errorCode = 0;
	if (blockModel == 1)
	{
		blockage = block_user;
		out->blockage = blockage;
		return errorCode;
	}
	double Rm = out->Ra;
	double Bm = 0.5 * (in->Ra + out->Ra);
	double rb_m = 0.25 * (in->Ra * in->b + 2 * Rm * Bm + out->Ra * out->b);
	double b6a = 1 - rb_m / (out->Ra * out->b);

	double Km = 1 / out->b;
	double b6b = SQR(Km * out->b) / (12 + SQR(Km * out->b));
	blockage = MAX(b6a, b6b);
	blockage = MAX(blockage, 0);
	out->blockage = blockage;
	return errorCode;
}

int T1dBend::Analysis()
{
	int errorCode = 0;
	double b6_old = 0;
	SC = 1;
	for (int i = 0; i < 50; i++)
	{
		MACHINE_RESPONSE_INTERRUPT(this);
		b6_old = out->blockage;
		ExitBlockage(out->blockage);
		errorCode = Solve_m(in->massflow, in, out, 0, 0, 0);
		double db = b6_old - out->blockage;
		if (fabs(db) < 1.0E-5)
			break;
	}
	return errorCode;
}

int T1dBend::PrelimSizing(double phi)
{
	int error = 1;
	property_t* p = property("Rout_Rin");
	bool hasRout_Rin = false;
	if (p)
	{
		hasRout_Rin = IF_PROPERTY_USER_OVERWRITE(p);
	}
	p = property("Bout_Bin");
	bool hasBout_Bin = false;
	if (p)
	{
		hasBout_Bin = IF_PROPERTY_USER_OVERWRITE(p);
	}
	if (hasBout_Bin) {
	}
	else {
		Bout_Bin = 1.;
	}
	out->b = Bout_Bin * in->b;

	double alpha4 = 30 + SQR(phi / 0.06);
	if (!hasRout_Rin)
	{
		//out->Ra = (1.1 + phi)*in->Ra;
		Rout_Rin = 1.07;
	}

	out->Ra = Rout_Rin * in->Ra;
	// Check:
	// max Bout_Bin
	double Bout_max = 0., Bout_Bin_max = 0.;
	double Rout_min = 0., Rout_Rin_min = 0.;

	if (hasRout_Rin)
	{
		Rout_min = in->Ra + Bout_Bin * in->b / 2.;
		Rout_Rin_min = Rout_min / in->Ra;
		if (Rout_Rin < Rout_Rin_min) {
			Rout_Rin = 1.05 * Rout_Rin_min;
			eprintf("Please check Rout/Rin of Bend diffuser.");
		}
	}
	else
	{
		Rout_min = in->Ra + Bout_Bin * in->b / 2.;
		Rout_Rin_min = Rout_min / in->Ra;
		if (Rout_Rin < Rout_Rin_min) {
			Rout_Rin = 1.05 * Rout_Rin_min;
			out->Ra = Rout_Rin * in->Ra;
		}
	}

	//-------------------------
	alpha4 = (90 - alpha4) * PI / 180;

	double bold = 0;
	SC = 1.0;

	//calculate friction coefficient
	double mu = dVisc_pt(in->p, in->t);
	if (mu < 0)mu = 1.0E-8;
	double Re = in->rho * in->C * 2 * in->b / mu;
	double Dh = in->b;
	int error1 = 0;
	double cf = skinFricCoef(Re, Dh, e_pass, error1);
	double b_avg = in->b;
	double alpha_avg = 0.5 * (alpha4 + in->Alpha);
	double term = -cf * (out->Ra - in->Ra) / (b_avg * cos(alpha_avg));
	double SC1 = exp(term);
	if (SC1 > 1)SC1 = 1;
	else if (SC1 < 0.8)SC1 = 0.8;

	//double SC1 = 0.92;
	out->blockage = 0;
	double rho4 = in->rho;
	double RR = out->Ra / in->Ra;

	for (int i = 0; i < 50; i++)
	{
		double Ct4 = SC1 * (in->Ra * in->Ct) / out->Ra;

		if (hasBout_Bin)
		{
			out->b = in->b * Bout_Bin;
		}
		else
		{
			out->b = Bout_Bin * in->b;
		}

		double Cm4 = in->massflow / (2 * PI * out->Ra * out->b * rho4 * (1 - out->blockage));

		out->phi = 0.;
		UpdateGeometry();
		geomValid = true;
		double rho4_old = rho4;
		error = Analysis();
		ExitBlockage(out->blockage);
		if (error > 0)
		{
			eprintf("Sizing of Bend diffuser failed.\n");
			return error;
		}
		rho4 = out->rho;
		double db = out->b - bold;
		double drho4 = rho4 - rho4_old;
		if (i > 0)
		{
			out->b = bold + 0.5 * db;
		}
		if (fabs(db) < 1.0E-3 && fabs(drho4) < 1.0E-5)
		{
			error = 0;
			break;
		}
		bold = out->b;
	}

	if (!isUserInput(this, "Rout_Rin"))
	{
		Rout_Rin = out->Ra / in->Ra;
	}

	if (!isUserInput(this, "Bout_Bin"))
	{
		Bout_Bin = out->b / in->b;
	}

	inputRatio = true;
	in->geomOption = 0;
	out->geomOption = 0;
	in->RecalcGeom();
	out->RecalcGeom();
	return error;
}

// 4.
void T1dBend::GetConfigPassagePropertyList(QVector<property_t*>& properties, int source)
{
	bool inputRatio = getTag("_bInputRatio").toBool();
	if (property_t* p = in->property("Ra"))
	{
		p->setDisplayName("Inlet radius (avg)");
		properties.push_back(p);
		SET_PROPERTY_READONLY(p);
	}
	if (property_t* p = in->property("b"))
	{
		p->setDisplayName("Inlet width");
		properties.push_back(p);
		SET_PROPERTY_READONLY(p);
	}
	if (property_t* p = in->property("phi"))
	{
		p->setDisplayName("Inlet inclination angle");
		properties.push_back(p);
		SET_PROPERTY_READONLY(p);
	}	

	if (!inputRatio)
	{
		if (property_t* p = out->property("Ra"))
		{
			p->setDisplayName("Outlet radius (avg)");
			properties.push_back(p);
		}
		if (property_t* p = out->property("b"))
		{
			p->setDisplayName("Outlet width");
			properties.push_back(p);
		}
	}
	else
	{
		QStringList L = QStringList() << "Rout_Rin" << "Bout_Bin";
		foreach(QString s, L)
		{
			if (property_t* p = property(s))
			{
				properties.push_back(p);
			}
		}
	}

	if (property_t* p = out->property("phi"))
	{
		p->setDisplayName("Outlet inclination angle");
		properties.push_back(p);
	}	
	if (property_t* p = property("L_axial"))
	{
		p->setDisplayName("Axial length");
		properties.push_back(p);
	}
	if (property_t* p = property("phi1_hub"))
	{
		p->setDisplayName("Hub contour angle at inlet");
		properties.push_back(p);
	}
	if (property_t* p = property("phi1_shroud"))
	{
		p->setDisplayName("Shroud contour angle at inlet");
		properties.push_back(p);
	}
	if (property_t* p = property("phi2_hub"))
	{
		p->setDisplayName("Hub contour angle at outlet");
		properties.push_back(p);
	}
	if (property_t* p = property("phi2_shroud"))
	{
		p->setDisplayName("Shroud contour angle at outlet");
		properties.push_back(p);
	}
}

void T1dBend::GetConfigGeometryPropertyList(QVector<property_t*>& properties, int source)
{
	QStringList L = QStringList() << "Rout_Rin" << "Bout_Bin";
	foreach(QString s, L)
	{
		if (property_t* p = property(s))
		{
			properties.push_back(p);
		}
	}
}