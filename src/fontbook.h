//
// C++ Interface: fontbook
//
// Description:
//
//
// Author: Pierre Marchand <pierremarc@oep-h.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FONTBOOK_H
#define FONTBOOK_H

#include <QDomDocument>
#include <QColor>
#include <QPrinter>
#include <QMap>
#include <QPainter>
#include <QRectF>
#include <QPrinter>
#include <QObject>


class ProgressBarDuo;
/**
	@author Pierre Marchand <pierremarc@oep-h.com>

	It’s time to write something more "definitive" :)
*/

struct TextElementStyle
{
	QString name;
	QString font; // _FONTMATRIX_ is reserved
	double fontsize;
	double lineheight;
	QColor color;
	double margin_top,margin_left,margin_bottom,margin_right;
	TextElementStyle () {}
	TextElementStyle ( QString n, QString f, double fs, double lh, double mt, double ml, double mb, double mr, QColor co) :
			name(n),
			font ( f ),
			fontsize ( fs ),
			lineheight ( lh ),
			margin_top ( mt ),
			margin_left ( ml ),
			margin_bottom ( mb ),
			margin_right ( mr ),
			color ( co ) {}
};

struct TextElement
{
	QString e;
	bool valid;
	/**
	Has to be set if "e" must be substituted with a contextual info
	available infos depend of level and are :
	- Family
	- SubFamily
	- Encoding
	- PageNumber
	- ...
	At some point, I’ll provide a reference.
	*/
	// 	bool internal; OBSOLETE - substitution will be regexpizated ##KEYWORD##
	TextElement():valid(false){}
	TextElement ( QString elem) :e ( elem ), valid(true){}
};

/**
GraphicElement will be made by
<graphic xpos="0.0" ypos="0.0"><svg></svg></graphic>
where svg comes from regular svg file.
*/
struct GraphicElement
{
	QString name;
	double x,y;
	QString svg; // OBSOLETE :)
	bool valid;
	GraphicElement(QString aName, QString svgstring, double xpos,double ypos):name(aName),svg(svgstring.startsWith("<?xml") ? svgstring : ("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n" + svgstring)),x(xpos),y(ypos),valid(true){}
	GraphicElement():name("noname"),svg(""),x(0.0),y(0.0),valid(false){}
};

struct FontBookContext
{
	TextElement textElement;
	TextElementStyle textStyle;
	GraphicElement graphic;
	/*
	enum FBCLevel{PAGE, FAMILY, SUBFAMILY};
	
	FBCLevel level;*/
	
};

class FontBook : public QObject
{
public:
	enum Style
	{
		Full,
		OneLiner
	};
	FontBook();

	~FontBook();
	void doBook(Style s);
private:

	void doFullBook();
	void doFullBookCover();
	// return true if uses only its page, false if it spreads over the 2 pages
	bool doFullBookPageLeft(const QString& family);
	void doFullBookPageRight(const QString& family);
	void doOneLinerBook();

	QPrinter * printer;
	QPainter * painter;
	QRectF printerRect;
	QStringList stringList;
	ProgressBarDuo * progress;

	QString outputFilePath;
	QMap<QString, QPrinter::PageSize > mapPSize;
	void doBookFromTemplate ( const QDomDocument &aTemplate );
};

#endif
