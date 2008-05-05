//
// C++ Implementation: dataexport
//
// Description: 
//
//
// Author: Pierre Marchand <pierremarc@oep-h.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "dataexport.h"

#include "typotek.h"
#include "fontitem.h"

#include <QFile>
#include <QXmlStreamWriter>
#include <QDebug>
#include <QProgressDialog>

DataExport::DataExport(const QString &dirPath, const QString &filterTag)
{
	exDir.setPath(dirPath);
	filter = filterTag;
	typotek *typ = typotek::getInstance();
	fonts = typ->getFonts(filter,"tag");
	typ->resetFilter();
}


DataExport::~DataExport()
{
}

int DataExport::copyFiles()
{
	QProgressDialog progress ( QObject::tr ( "Copying files" ), QObject::tr ( "cancel" ), 0, fonts.count(), typotek::getInstance() );
	progress.setWindowModality ( Qt::WindowModal );
	int progressindex(0);
	
	QString preview(typotek::getInstance()->word());
	
	int copyCounter(0);
	QList<int> toRemove;
	for(int fidx( 0 ); fidx < fonts.count() ; ++fidx)
	{
		if ( progress.wasCanceled() )
			break;
		progress.setLabelText ( fonts[fidx]->fancyName() );
		progress.setValue ( ++progressindex );
		
		QFile ffile(fonts[fidx]->path());
		QFileInfo ifile(ffile);
// 		qDebug()<< exDir.absolutePath() + exDir.separator() + ifile.fileName();
		if(ffile.copy(exDir.absolutePath() + exDir.separator() + ifile.fileName()) )
		{
			++copyCounter;
			QImage itImage(fonts[fidx]->oneLinePreviewPixmap(preview, Qt::white).toImage());
			if(!itImage.save(exDir.absolutePath() + exDir.separator() + ifile.fileName() + ".png"))
				qDebug()<<"Unable to save "<< exDir.absolutePath() + exDir.separator() + ifile.fileName() + ".png";
		}
		else
		{
			typotek::getInstance()->showStatusMessage(QObject::tr("Unable to copy")+" "+fonts[fidx]->path());
			toRemove << fidx;
		}
	}
	
	for(int i(toRemove.count() - 1); i >= 0;--i)
		fonts.removeAt(toRemove[i]);
	return copyCounter;
}

int DataExport::buildIndex()
{
	QFile file(exDir.absolutePath() + exDir.separator() +"fontmatrix.data");
	QXmlStreamWriter xmlStream(&file);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		qDebug() << "Export Warning : Can't open " << file.fileName();
		return 0;
	}
	else
	{
		xmlStream.setAutoFormatting(true);
	}
	
	xmlStream.writeStartDocument();
	xmlStream.writeStartElement("fontmatrix");
	xmlStream.writeAttribute("version", "1.0");
	
	for(int fidx( 0 ); fidx < fonts.count() ; ++fidx)
	{
		FontItem* fitem(fonts[fidx]);
		{
			xmlStream.writeStartElement("fontfile");
			xmlStream.writeAttribute("family", fitem->family());
			xmlStream.writeAttribute("variant",fitem->variant());
			xmlStream.writeAttribute("type",fitem->type());
			xmlStream.writeStartElement("file");
			xmlStream.writeCharacters( QFileInfo(fitem->path()).fileName() );
			xmlStream.writeEndElement();
			xmlStream.writeStartElement("info");
			xmlStream.writeCharacters( fitem->infoText() );
			xmlStream.writeEndElement();
			QStringList tl = fitem->tags();
			tl.removeAll("Activated_On");
			tl.removeAll("Activated_Off");
			foreach(QString tag, tl)
			{
				xmlStream.writeStartElement("tag");
				xmlStream.writeCharacters( tag );
				xmlStream.writeEndElement();
			}
			xmlStream.writeEndElement();
		}		
	}
	
	xmlStream.writeEndElement();//fontmatrix
	xmlStream.writeEndDocument();
	file.close();
	return fonts.count();
}

int DataExport::doExport()
{
	int bd (buildIndex());
	if( bd == 0 )
		return 0;
	
	int cp (copyFiles());
	
	buildHtml();
	
	return cp;
}

int DataExport::buildHtml()
{
	QFile file(exDir.absolutePath() + exDir.separator() +"index.html");
	QXmlStreamWriter xmlStream(&file);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		qDebug() << "Export Warning : Can't open " << file.fileName();
		return 0;
	}
	else
	{
		xmlStream.setAutoFormatting(true);
	}
	
	xmlStream.writeStartDocument();
	xmlStream.writeStartElement("html");
	
	// header
	xmlStream.writeStartElement("head");
	xmlStream.writeStartElement("title");
	xmlStream.writeCharacters( "Fontmatrix - " + filter);
	xmlStream.writeEndElement();
	xmlStream.writeEndElement();
	
	xmlStream.writeStartElement("body");
	for(int fidx( 0 ); fidx < fonts.count() ; ++fidx)
	{
		FontItem* fitem(fonts[fidx]);
		{
			QFileInfo ffile(fitem->path());
			
			xmlStream.writeStartElement("div");
			xmlStream.writeAttribute("class", "fontbox");
			
			xmlStream.writeStartElement("div");
			xmlStream.writeAttribute("class", "namebox");
			
			xmlStream.writeStartElement("a");
			xmlStream.writeAttribute("href", ffile.fileName() );
			xmlStream.writeCharacters( fitem->fancyName() );
			xmlStream.writeEndElement();// a
			
			xmlStream.writeEndElement();// div.namebox
			
			xmlStream.writeStartElement("img");
			xmlStream.writeAttribute("class", "imgbox");
			xmlStream.writeAttribute("src", ffile.fileName() + ".png");
			xmlStream.writeEndElement();// img.imgbox
			
			xmlStream.writeStartElement("div");
			xmlStream.writeAttribute("class", "infobox");
			
			QStringList tl = fitem->tags();
			tl.removeAll("Activated_On");
			tl.removeAll("Activated_Off");
			foreach(QString tag, tl)
			{
				xmlStream.writeStartElement("div");
				xmlStream.writeAttribute("class", "tagbox");
				xmlStream.writeCharacters( tag );
				xmlStream.writeEndElement(); //div.tagbox
			}
			xmlStream.writeEndElement();// div.info
			xmlStream.writeEndElement();// div.fontbox
		}		
	}
	xmlStream.writeEndElement();// body
	xmlStream.writeEndElement();// html
	xmlStream.writeEndDocument();
	file.close();
	return fonts.count();
}


