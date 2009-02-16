// PythonQt wrapper implementation

#include "fontitem.h"

#include "fmpython_w.h"

#include "typotek.h"
#include "mainviewwidget.h"
#include "listdockwidget.h"
#include "fmfontdb.h"
#include "fmscriptconsole.h"

#include <QFile>
#include <QDebug>


FMPythonW *FMPythonW::instance = 0;
const QStringList FMPythonW::exposedClasses = (
					       QStringList()    << "FontItem" 
								<< "FMFontDb");

FMPythonW::FMPythonW()
	:tk ( typotek::getInstance() )
{

}


FMPythonW * FMPythonW::getInstance()
{
	if(!instance)
	{
		instance = new FMPythonW;
		Q_ASSERT(instance);
		instance->init();
		instance->doConnect();
	}
	
	return instance;
}

void FMPythonW::init()
{
	PythonQt::init(PythonQt::RedirectStdOut);
	connect(PythonQt::self(), SIGNAL(pythonStdOut(const QString&)), this, SLOT(catchStdOut(const QString&)));
	connect(PythonQt::self(), SIGNAL(pythonStdErr(const QString&)), this, SLOT(catchStdErr(const QString&)));
}

void FMPythonW::doConnect()
{
// 	connect ( ListDockWidget::getInstance(),
// 	          SIGNAL ( currentChanged() ),
// 	          SIGNAL ( currentChanged() ) );
}
void FMPythonW::runFile(const QString & pyScript)
{
	QFile sf(pyScript);
	QString script;
	if(sf.open(QIODevice::ReadOnly))
	{
		
		script = QString::fromUtf8(sf.readAll());
	}
	else
		qDebug()<<"Error: Cannot open"<<pyScript;
	
// 	mainContext.evalScript(script, Py_file_input);
	runString(script);

}

void FMPythonW::runString(const QString & pyScript)
{
	PythonQtObjectPtr mainContext = PythonQt::self()->getMainModule();
	PythonQt::self()->registerQObjectClassNames(exposedClasses);
	mainContext.addObject("Fontmatrix", this );
	
	QString pHead("from PythonQt import *\n");
	mainContext.evalScript(pHead + pyScript, Py_file_input);
}


void FMPythonW::catchStdOut(const QString & s)
{
	FMScriptConsole::getInstance()->Out(s);
}

void FMPythonW::catchStdErr(const QString & s)
{
	FMScriptConsole::getInstance()->Err(s);
}


/// "exported" methods

void FMPythonW::nextFace()
{
// 	qDebug()<<"FMPythonW::nextFace";
	ListDockWidget::getInstance()->fontTree->slotNextFont();
}

void FMPythonW::previousFace()
{
// 	qDebug()<<"FMPythonW::previousFace";
	ListDockWidget::getInstance()->fontTree->slotPreviousFont();
}

void FMPythonW::nextFamily()
{
// 	qDebug()<<"FMPythonW::nextFamily";
	ListDockWidget::getInstance()->fontTree->slotNextFamily();
}

void FMPythonW::previousFamily()
{
// 	qDebug()<<"FMPythonW::previousFamily";
	ListDockWidget::getInstance()->fontTree->slotPreviousFamily();
}

QString FMPythonW::currentFontPath()
{
	if ( !tk->getSelectedFont() )
		return QString();
	return tk->getSelectedFont()->path();
}

QString FMPythonW::currentFontFamily()
{
	if ( !tk->getSelectedFont() )
		return QString();
	return tk->getSelectedFont()->family();
}

QString FMPythonW::currentFontStyle()
{
	if ( !tk->getSelectedFont() )
		return QString();
	return tk->getSelectedFont()->variant();
}

void FMPythonW::Debug(QVariant var)
{
	qDebug()<<var;
}


FMFontDb* FMPythonW::DB()
{
	return FMFontDb::DB();
}

void FMPythonW::updateTree()
{
	typotek::getInstance()->getTheMainView()->slotUpdateTree();
}



