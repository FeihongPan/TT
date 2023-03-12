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
#include "exp_DevPlot.h"
#include "w_PropertyHolderDialog.h"
#include<ShlObj.h>
REGISTER_OBJECT_CLASS(exp_DevPlot, "exp_DevPlot", TObject);

exp_DevPlot::exp_DevPlot(QString object_n, TObject *iparent) :
  TObject(object_n, iparent)
{
  INIT_OBJECT;

  setTag("MethodOrder", "showGUI,$");
}

O_METHOD_BEGIN(exp_DevPlot, showGUI, "development plot", 0, FLG_METHOD_SKIP_TUI, "devplot")
{
  if (VARLID_ARG)
  {
    exp_DevPlot pltDev;
    pltDev.showGUI();
  }
}
O_METHOD_END;



void exp_DevPlot::getConfigData(QString fileName, QMap<QString,QVector<QString>> &configData)
{
  QString mStr;
  QFile file(fileName);
  QStringList _txt;
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
	  eprintf("Open failed %s", fileName.ascii());
  }
  QTextStream in(&file);
  int i = 0;
  while (!in.atEnd())
  {
	  in >> mStr;
    if (mStr.length()>2)_txt.push_back(mStr);
  }
  configData.clear();
  mapStringTableFromCSV(_txt, configData, seprator);
  return;
}
void exp_DevPlot::getPerformanceData(QString fileName, QMap<QString,QVector<double>>& data)
{
  QString mStr;
  QFile file(fileName);
  QStringList _txt;
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
	  eprintf("Open failed %s", fileName.ascii());
  }
  QTextStream in(&file);
  int i = 0;
  while (!in.atEnd())
  {
	  in >> mStr;
    if (mStr.length()>2)_txt.push_back(mStr);
  }
  data.clear();
  mapDataTableFromCSV(_txt, data, seprator);
  return;
}


void exp_DevPlot::mapDataTableFromCSV(QStringList txt, QMap<QString,QVector<double>>& data, QString seprator)
{
  QStringList cols = txt.first().split(seprator);
  QStringList _data;
  double x = 0.0;
  data.clear();
  for (int i = 0; i < cols.size(); i++)data[cols[i]].resize(txt.size() - 1);
  for (int i = 1; i < txt.size(); i++)
  {
    _data = txt[i].split(seprator);
    for (int j = 0; j < cols.size(); j++)
    {
      if (_data[j].size() > 0)
      {
        x= _data[j].toDouble();
        data[cols[j]][i - 1] = _data[j].toDouble();
      }
      else data[cols[j]][i - 1] = data[cols[j]][i - 2];
    }
  }
  return;
}
void exp_DevPlot::mapStringTableFromCSV(QStringList txt, QMap<QString,QVector<QString>>& data, QString seprator)
{
  QStringList cols = txt.first().split(seprator);
  QStringList _data;
  for (int i = 0; i < cols.size(); i++)data[cols[i]].resize(txt.size() - 1);
  for (int i = 1; i < txt.size(); i++)
  {
    _data = txt[i].split(seprator);
    for (int j = 0; j < cols.size(); j++)
    {
      if (_data[j].size()>0)data[cols[j]][i - 1] = _data[j];
      else data[cols[j]][i - 1] = data[cols[j]][i - 2];
    }
  }
  return;
}


#include "curve_Nurbs.h"
void exp_DevPlot::makelinePlot_base(QString tabName,QString yTitle,draw_XYInteractive* lineplot)
{

  int lineType = 1;
  int symbol = 1;
  int colorIndex = 1;

  double x = 0.0;
  double y = 0.0;
  Double2 xy = { 0.0,0.0 };

  int idx = 0;
  int dataNum = 0;
  draw_Curve* e = nullptr;
  QVector<int> ConfigIdxs;
  
  for (int i = 0; i <configData.value("Name").size(); i++)//这里根据 compressure Name(tab) 和ytitle 也即子图的名字进行分类
  {
    if (configData.value("Name")[i] == tabName && configData.value("ytitle")[i]==yTitle)
    {
      ConfigIdxs.append(i);
    }
  }
  
  for (int i = 0; i < ConfigIdxs.size(); i++)
  {
    idx = ConfigIdxs[i];
    getPerformanceData(configData.value("CurvePath")[idx], performanceData);
    data.clear();
    QVector<double> _ylabel = performanceData.value(configData.value("ylabel")[idx]);
    for (int j = 0; j < performanceData.value(configData.value("ylabel")[idx]).size(); j++)
    {
      x = performanceData.value(configData.value("xlabel")[idx])[j];
      /*QString _xlabel = configData.value("xlabel")[idx];
      QVector<double>_xs = performanceData.value(_xlabel);
      double _x = _xs[idx];*/
      y = performanceData.value(configData.value("ylabel")[idx])[j];
      xy[0] = x;
      xy[1] = y;
      data.append(xy);
    }
    e = lineplot->newPlotCurve(data, configData.value("CurveName")[idx]);
    lineType = (dataNum + 1) % 6 == 0 ? 1 : (dataNum + 1) % 6;//No pen,Solid line,Dash line,Dot line,Dash Dot line,Dash Dot Dot line
    symbol = (dataNum + 1) % 14;
    colorIndex = dataNum % RGBcolors.size();
    e->setLineWidth(3)
      .setLineType(lineType)
      .setShowInLegend(true)
      .setSymbolStyle(symbol)
      .setColor(RGBcolors[colorIndex]);
    dataNum++;
  }
  lineplot->setXTitle(configData.value("xtitile")[idx]);
  lineplot->setYTitle(configData.value("ytitle")[idx]);
  //lineplot->autoFillBackground();
  lineplot->zoomAll();
  lineplot->applySettings();
  lineplot->setTitle(configData.value("xtitile")[idx] + "-" + configData.value("ytitle")[idx]);
  return;
}

void exp_DevPlot::makelinePlot_tabbase(QString tabName,w_PropertyHolderWidget* pltHolder)
{
  pltHolder->getDisplayName();
  int idx = 0;
  lineMultiplot = new draw_XYMultiWidget(pltHolder);
  QVector<int > ConfigIdxs;
  for (int i = 0; i < configData.value("Name").size(); i++)//找到同一个tab下面所有的曲线(根据ytitile进行划分)
  {
    if (configData.value("Name")[i] == tabName)
    {
      ConfigIdxs.append(i);
    }
  }
  
  int nPic = 0;
  QString _ytitle = "xxxxxxxx";
  for (int i = 0; i < ConfigIdxs.size(); i++)
  {
    idx = ConfigIdxs[i];
    if (_ytitle == configData.value("ytitle")[idx])continue;
    else
    {
      _ytitle = configData.value("ytitle")[idx];
      nPic++;
    }
  }
  //根据ytitle绘制子图
 _ytitle = "xxxxxxxx";
 int pltIdx = 0;
  for (int i = 0; i < ConfigIdxs.size(); i++)
  {
    idx = ConfigIdxs[i];
    if (_ytitle == configData.value("ytitle")[idx])continue;
    pltIdx++;
    _ytitle = configData.value("ytitle")[idx];
    calculateFigNums(nPic, pltIdx);
    draw_XYInteractive* lineplot = lineMultiplot->getPlot(nrow,ncol);
    pltHolder->placeWidget(lineMultiplot);
    makelinePlot_base(tabName,_ytitle, lineplot);
  }
}

void exp_DevPlot::makeDevPlots(QString configFileName,w_PropertyHolderWidget* Mainholder)
{
  getConfigData(configFileName, configData);
  QString tabName = "_xxxxxx";
  for (int i = 0; i < configData.value("Name").size(); i++)//找到同一个tab下面所有的曲线(根据ytitile进行划分)
  {
    if (configData.value("Name")[i] == tabName)continue;
    tabName = configData.value("Name")[i];
    w_PropertyHolderWidget* pltHolder = Mainholder->getHolder(0, 0, 1, 1, "", false, tabName);
    makelinePlot_tabbase(tabName, pltHolder);
  }
  Mainholder->showMaximized();

  return;
}

void exp_DevPlot::calculateFigNums(int totalFigs, int nowIdx)
{
  if (totalFigs < 5)
  {
    nrow = 1;//row
    ncol = totalFigs;//col
  }
  else if(totalFigs < 7)
  {
      nrow = 2;//row
      ncol = 3;//col
  }
  else if(totalFigs < 9)
  {
      nrow = 2;//row
      ncol = 4;//col
  }
  else if(totalFigs < 10)
  {
      nrow = 3;//row
      ncol = 3;//col
  }
  else
  {
    if(totalFigs/4 *4==totalFigs)nrow = totalFigs/4;//row
    else nrow = totalFigs / 4 + 1;
    ncol = 4;//col
  }
 
  if (nowIdx / ncol * ncol == nowIdx)
  {
    nrow = nowIdx / ncol;
  }
  else
  {
    
    nrow = nowIdx / ncol + 1;
    ncol = nowIdx -  ncol*(nrow-1);
    
  }
  return;

}


void exp_DevPlot::showGUI()
{
  showDevPlot();
}

void exp_DevPlot::showDevPlot()
{
  w_PropertyHolderDialog dlg;
  dlg.setWindowTitle("devPlot");
  holderNum = 0;
  holder = dlg.getHolder(holderNum, 0, 1, 1);
  w_PropertyHolderWidget* holder1 = holder->getHolder(0, 0, 1, 1);
  w_PropertyHolderWidget* holder2 = holder->getHolder(1, 0, 1, 1);
  QStringList desktop = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation);
  fileName = desktop[0] + "/Inbox/plotDev/configFile.csv";
  fileName = "Z:/Development/Users/YihangGuo/configFile.csv";
  wGetFilName = holder1->addProperty(&fileName, "File path", 0, false, false, false, "TCallbackOpenFileBrowser");
  apply = holder2->addButton("add to plot", 0, 0, 1, 2);
  /*clcPlot = holder2->addButton("clear plot", 0, 1, 1, 1);*/
  w_PropertyHolderWidget* pltHolder = holder->getHolder(3, 0, 1, 1);
  
  QObject::connect(apply, &w_QPushButton::clicked, [this,&dlg]()
    {
      this->fileName = this->wGetFilName->getValue().toString();
      pltholder = holder->getHolder(2, 0, 1, 1);
      this->makeDevPlots(fileName,this->pltholder);
      dlg.showMaximized();


      
      
    }
  );
  //QObject::connect(clcPlot, &w_QPushButton::clicked, [this]()
  //  {
  //    pltholder->close();
  //    this->holder->parentWidget()->showNormal();
  //   
  //  }
  //);
  if (dlg.exec() == QDialog::Accepted)
    {

    }
}




