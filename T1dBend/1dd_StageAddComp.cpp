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

#include "1dd_stageAddComp.h"
#include "w_Widgets.h"
#include "util_Study.h"
#include "1d_machine.h"
#include "util_ObjectMethod.h"
#include "core_Application.h"
#include "w_PropertyHolderDialog.h"
#include "util_Translator.h"
 /*******************************************************************************
 ** T1dStageAddCompDlg
 */
T1dStageAddCompDlg::T1dStageAddCompDlg(TObject *o, int position, QWidget * parent, Qt::WindowFlags f) :
  w_QDialog(parent, f)
{
  oin = o;
  QVBoxLayout *grid = new QVBoxLayout;

  QGroupBox *groupBox = new QGroupBox(tr("Stage Component"), this);
  radio1 = new QRadioButton(tr("&IGV"), groupBox);
  radio2 = new QRadioButton(tr("&Diffuser"), groupBox);
  radio3 = new QRadioButton(tr("&Volute"), groupBox);
  radio4 = new QRadioButton(tr("&Return channel"), groupBox);
  radio5 = new QRadioButton(tr("&CCX"), groupBox);
  radio6 = new QRadioButton(tr("&Inducer"), groupBox);
  radio7 = new QRadioButton(tr("&Inlet chamber"), groupBox);
	radio8 = new QRadioButton(tr("&PNV"), groupBox);
  connect(radio1, SIGNAL(clicked()), this, SLOT(setComponent()));
  connect(radio2, SIGNAL(clicked()), this, SLOT(setComponent()));
  connect(radio3, SIGNAL(clicked()), this, SLOT(setComponent()));
  connect(radio4, SIGNAL(clicked()), this, SLOT(setComponent()));
  connect(radio5, SIGNAL(clicked()), this, SLOT(setComponent()));
  connect(radio6, SIGNAL(clicked()), this, SLOT(setComponent()));
  connect(radio7, SIGNAL(clicked()), this, SLOT(setComponent()));
	connect(radio8, SIGNAL(clicked()), this, SLOT(setComponent()));
  const char *sname = TObject::shortName("T1dObject");
  TObject* oned = CURRENT_STUDY->object(sname);
  TObject* machine = oned->firstChild("T1dMachine");
  T1dMachine *pMachine = dynamic_cast<T1dMachine*>(machine);
  QString MachineType = pMachine->getMachineTypeString();
  isCompressor= (MachineType == "compressor");
  isPump = (MachineType == "pump");
  isFan = (MachineType == "fan");

  QGridLayout *layout = new QGridLayout();
  layout->addWidget(radio1, 0, 1);
  layout->addWidget(radio2, 1, 1);
  diffuser_comp = tr("Vaneless");

  _allDiffuserType = QStringList() << tr("Vaneless");
  if ((pMachine->AxRad == T1dMachine::exit_type_mixed) && (pMachine->machineType == MachineType::mach_fan))
    _allDiffuserType << tr("Axial vaned");
  else if (!((o->haveBaseClass("T1dAirfoilDiffuser") || o->haveBaseClass("T1dWedgeDiffuser") || o->haveBaseClass("T1dVanedDiffuser")
    || o->haveBaseClass("T1dAxVanedDiffuser")) && position == 0)) {
    _allDiffuserType << tr("Generic vaned");
    if (!isPump)
    {
      _allDiffuserType << tr("Airfoil") << tr("Wedge");
    }
    _allDiffuserType << tr("Vaned") << tr("Axial vaned") << tr("Bend") << tr("Bend 90") << tr("Pipe");
  }
  _diffuserType = _allDiffuserType[0];
  if (wDiffuserType = w_Property::getPropertyWidget(&_diffuserType, tr("Diffuser type: "), this, &_allDiffuserType, false, false, true))
  {
    wDiffuserType->setAutoSave();
    layout->addWidget(wDiffuserType, 1, 2);
    connect(wDiffuserType, SIGNAL(valueChanged()), this, SLOT(setDiffuser()));
    wDiffuserType->hide();
  }

  _allAirfoilType = QStringList() << tr("Generic airfoil") << tr("NACA airfoil") << tr("SCA camber airfoil") << tr("DCA camber airfoil");
  if (wAirfoil = w_Property::getPropertyWidget(&airfoilType, tr("Airfoil type: "), this, &_allAirfoilType, false, false, true))
  {
    wAirfoil->setAutoSave();
    layout->addWidget(wAirfoil, 1, 3);
  }
  wAirfoil->hide();

  _dualVolute = new QCheckBox(tr("Dual Volute"), this);
  _dualVolute->hide();
  connect(_dualVolute, SIGNAL(clicked()), this, SLOT(setDualVolute()));

  _casing = new QCheckBox(tr("Casing"), this);
  _casing->hide();
  connect(_casing, SIGNAL(clicked()), this, SLOT(setCasing()));
  CBLoadFromDB = new QCheckBox(tr("Load From Database"), this);
  if (o)
  {
    if ((o->cName() == "T1dVolute" || o->cName() == "T1dReturnChannel" || o->cName() == "T1dCCX"|| o->cName() == "T1dPositiveNegativeVane") && position == 0)
    {
      radio1->hide();
      radio3->hide();
      radio4->hide();
      radio5->hide();
      radio6->hide();
      radio7->hide();
			radio8->hide();
      radio2->setChecked(true);
      wDiffuserType->show();
      t1d_component = "diffuser";
    }
    if (o->cName() == "T1dImpeller")
    {		
      if (position == 0)
      {
        radio2->hide();
        radio3->hide();
        radio4->hide();
        radio5->hide();
				radio8->hide();
        wDiffuserType->hide();
        if (isPump)
        {
          if (TObject* p = o->parent())
          {
            T1dStage* stage = dynamic_cast<T1dStage*>(p);
            if (stage->hasInduc())
            {
              radio1->hide();
              radio7->hide();
            }
            if (stage->hasIGV())
            {
              radio1->hide();
              radio6->hide();
              radio7->hide();
            }
            if (stage->hasInletChamber())
            {
              radio7->hide();
            }
          }
          radio1->setChecked(false);
          radio6->setChecked(true);
          // radio7->hide();
          radio7->setChecked(false);
          t1d_component = "inducer";
        }
        else
        {
          radio1->setChecked(true);
          radio6->hide();
          radio7->hide();
          radio6->setChecked(false);
          t1d_component = "igv";
        }
      }
      if (position == 1)
      {
        radio1->hide();
        radio6->hide();
        radio7->hide();
        radio2->setChecked(true);
        wDiffuserType->show();
        t1d_component = "diffuser";
        if (TObject * p = o->parent())
        {
          T1dStage * stage = dynamic_cast<T1dStage*>(p);
          if (!(stage->compList.last() == o))
          {
            radio3->hide();
            radio4->hide();
            radio5->hide();
						radio8->hide();
          }
        }
        if (!isPump && !isCompressor)
          radio5->hide();
      }
    }
    else if (o->cName() == "T1dInducer")
    {
      radio3->hide();
      radio4->hide();
      radio5->hide();
      if (position == 0)
      {
        radio2->hide();
        //radio7->hide();
        //radio7->setChecked(false);
        radio1->setChecked(false);
        radio6->setChecked(true);
        t1d_component = "inducer";
      }
      else if (position == 1)
      {
        radio1->hide();
        radio6->hide();
        // radio7->hide();
        radio2->setChecked(true);
        wDiffuserType->show();
        t1d_component = "diffuser";
      }
    }

    QStringList L = QStringList() << "T1dVaneless"
      << "T1dAirfoilDiffuser"
      << "T1dWedge"
      << "T1dDiffuser"
      << "T1dWedgeDiffuser"
      << "T1dVanedDiffuser"
      << "T1dVaned"
      << "T1dAxVanedDiffuser"
      << "T1dBend90"
      << "Bend";
    if (L.indexOf(o->cName()) >= 0)
    {
      radio1->hide();
      radio6->hide();
      radio7->hide();
      radio2->setChecked(true);
      wDiffuserType->show();
      t1d_component = "diffuser";
      if (position == 0)
      {
        radio3->hide();
        radio4->hide();
        radio5->hide();
        radio8->hide();
      }
      else
      {
        if (!isPump && !isCompressor)
          radio5->hide();
      }
    }
    if (isCompressor)
    {
      if (o->cName() == "T1dBend90" || o->cName() == "T1dAxVanedDiffuser")
        radio4->hide();

      radio5->hide();
      radio8->hide();
    }
  }
  layout->addWidget(radio3, 2, 1);
  layout->addWidget(_dualVolute, 2, 2);
  layout->addWidget(_casing, 2, 3);
  layout->addWidget(radio4, 3, 1);
  layout->addWidget(radio5, 4, 1);
  layout->addWidget(radio6, 5, 1);
  layout->addWidget(radio7, 6, 1);
	layout->addWidget(radio8, 7, 1);
  groupBox->setLayout(layout);
  grid->addWidget(groupBox, 0, 0);
  grid->addWidget(CBLoadFromDB);
  QPushButton *applyButton = new w_OKButton;
  connect(applyButton, SIGNAL(clicked()), this, SLOT(accept()));
  QPushButton *cancelButton = new w_CancelButton;
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

  QHBoxLayout *buttonsLayout = new QHBoxLayout;
	buttonsLayout->addStretch(1);
  buttonsLayout->addWidget(applyButton);
  buttonsLayout->addWidget(cancelButton);

	grid->addLayout(buttonsLayout);
  setLayout(grid);
  setWindowTitle(tr("Add Component"));

}

void T1dStageAddCompDlg::setDiffuser()
{
  if (t1d_component != "diffuser") {
    QMessageBox msgBox(core_Application::core());
    msgBox.setText("Please select Diffuser first.");
    msgBox.exec();
    return;
  }
  diffuser_comp = _diffuserType;
  wAirfoil->setHidden(_diffuserType != tr("Airfoil"));
}

void T1dStageAddCompDlg::setDualVolute()
{
  isDualVolute = _dualVolute->isChecked();
}

void T1dStageAddCompDlg::setCasing()
{
  isCasing = _casing->isChecked();
}

void T1dStageAddCompDlg::setComponent()
{
  wDiffuserType->hide();
  _dualVolute->hide();
  _casing->hide();
  if (radio1->isChecked())
    t1d_component = "igv";
  else if (radio2->isChecked()) {
    t1d_component = "diffuser";
    wDiffuserType->show();
  }
  else if (radio3->isChecked())
  {
    t1d_component = "volute";
    if (isPump)_dualVolute->show();
    if (isFan && (oin->haveBaseClass("T1dImpeller")))
      _casing->show();
  }
  else if (radio4->isChecked())
    t1d_component = "returnchannel";
  else if (radio5->isChecked())
	  t1d_component = "ccx";
  else if (radio6->isChecked())
    t1d_component = "inducer";
  else if (radio7->isChecked())
    t1d_component = "inlet chamber";
	else if (radio8->isChecked())
		t1d_component = "npv";
}

void T1dStageAddCompDlg::setObject(T1dStage * obj)
{
  stage = obj;  
}

T1dComponent * T1dStageAddCompDlg::getComponent()
{
  return new_obj;
}

QString T1dStageAddCompDlg::getComponentClassName(QString t1d_component, QString diffuser_comp, bool isDualVolute, bool isCasing)
{
  QString s;
  if (t1d_component == "igv")
    s = "T1dIGV";
  else if (t1d_component == "volute")
  {
    if (isCasing) s = "T1dCasing";
    else
    {
      if (isDualVolute)
        s = "T1dVoluteDualOutlet";
      else
        s = "T1dVolute";
    }
  }
  else if (t1d_component == "diffuser")
  {
	  if (diffuser_comp == tr("Vaneless"))
		  s = "T1dVaneless";
	  else if (diffuser_comp == tr("Airfoil"))
		  s = "T1dAirfoilDiffuser";
	  else if (diffuser_comp == tr("Wedge"))
		  s = "T1dWedgeDiffuser";
	  else if (diffuser_comp == tr("Generic vaned"))
		  s = "T1dVanedDiffuser";
	  else if (diffuser_comp == tr("Vaned"))
		  s = "T1dVaned";
	  else if (diffuser_comp == tr("Axial vaned"))
		  s = "T1dAxVanedDiffuser";
	  else if (diffuser_comp == tr("Bend"))
		  s = "T1dBend";
    else if (diffuser_comp == tr("Bend 90"))
		  s = "T1dBend90";
    else if (diffuser_comp == tr("Pipe"))
      s = "T1dPipeDiffuser";
	  else
		  s = "T1dVaneless";
  }
  else if (t1d_component == "returnchannel")
    s = "T1dReturnChannel";
  else if(t1d_component == "ccx")
	  s = "T1dCCX";
  else if (t1d_component == "inducer")
    s = "T1dInducer";
  else if (t1d_component == "inlet chamber")
    s = "T1dInletChamber";
	else if (t1d_component == "npv")
		s = "T1dPositiveNegativeVane";
  return s;
}

void T1dStageAddCompDlg::makeNewObj()
{
  if (t1d_component == "")
    return;
  QString className = getComponentClassName(t1d_component, diffuser_comp, isDualVolute, isCasing);
  QString objName = TObject::shortName(className.ascii());
  if (isDualVolute)
    objName = "DualVolute";
  if (stage) {
    if (stage->object(objName)) {
      forLoop(i, 1000) {//suppose the number of objects with the same class will not beyond 1000
        QString newObjName = objName + QString("%1").arg(i + 1);
        if (!(stage->object(newObjName))) {
          objName = newObjName;
          break;
        }
      }
    }
  }
  w_PropertyHolderDialog dlg(this);
  dlg.setWindowTitle(tr("Adding 1d Component"));
  w_PropertyHolderWidget* holder = dlg.getHolder(0, 0, 1, 1, tr("Component Name:"));
  holder->setHideLabel(true);
  QString displayName = TTranslator::TTRObjName(className, objName);
  if (w_Property* w = holder->addProperty(&displayName, "Display name"))
  {
    w->hideLabel();
  }
  if (dlg.exec() == QDialog::Accepted)
  {
    if (objName.isEmpty())
      objName = TObject::shortName(className.ascii());
    if (stage)
      if (stage->object(objName)) {
        QString info = tr("There are more than one object with the same name!");
        printError(qPrintable(info));
        return;
      }

    if (new_obj = (T1dComponent *)TObject::new_object(className.ascii(), objName, 0))
    {
      if (new_obj->haveBaseClass("T1dAirfoilDiffuser"))
      {
        if (new_obj->property("airfoilType"))
          new_obj->property("airfoilType")->setValue(airfoilType);
      }
      new_obj->set_display_name(displayName);
    }
    
  }
}

void T1dStageAddCompDlg::accept()
{
  makeNewObj();
  if (CBLoadFromDB->isChecked() && new_obj)
  {
    TObject *o = VARIANT2OBJECT(new_obj->execute("ImportFromDatabase"));
    if (o)
      isLoadFromDB = true;
    else
    {
      TObject::delete_object(new_obj);
      new_obj = 0;
    }
  }
#if 0
  stage->AddComponent(new_obj);
  stage->updateGUI(true);
  stage->update_link_list_from_path(true);
  stage->UpdateGeometry();
#endif
  w_QDialog::accept();
}

void T1dStageAddCompDlg::reject()
{
  w_QDialog::reject();
  close();

}

