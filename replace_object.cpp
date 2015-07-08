#include <QtCore/QCoreApplication>
#include <cstdio>
#include <cstdlib>
#include <QFile>
#include <QTextStream>
#include <QMap>
#include <QStringList>
#include <QDebug>
#include "replace_object.h"

// constructor
Replace_Object::Replace_Object()
{
}


void Replace_Object::RunMe(int argc, char *argv[])
{
	// show header first and foremorest
	Header();

	if (argc < 4)
	{
		Showhelp();
		exit(0);
	}

	qDebug() << "Source WRP: " << argv[1];
	qDebug() << "Destination WRP: " << argv[2];
	qDebug() << "Config file: " << argv[3];

	// open source WRP
	QFile file(argv[1]);
	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Unable to open " << file.fileName();
		exit(1);
	}

	qDebug() << "Opened file: " << file.fileName();

	// open destination WRP
	QFile file_dest(argv[2]);
	if (!file_dest.open(QIODevice::WriteOnly))
	{
		qDebug() << "Unable to open " << file_dest.fileName();
		exit(1);
	}

	qDebug() << "Opened file: " << file_dest.fileName();

	Config_Read(argv);

	// WRP signature
	char magic[4];
	file.read( magic, 4 );
	file_dest.write( magic, 4);

	// map size in cells
	file.read( reinterpret_cast<char*>(&texturegrid), 4 );
	file.read( reinterpret_cast<char*>(&texturegrid), 4 );

	if (texturegrid > 4096)
	{
		qDebug() << "Cells " << texturegrid << " is too large! exiting!";
		exit(1);
	};

	//qDebug() << "Cells: " << texturegrid;
	file_dest.write( reinterpret_cast<char*>(&texturegrid), 4 );
	file_dest.write( reinterpret_cast<char*>(&texturegrid), 4 );

	short materialindex = 0;

	// elevations
	for (int i = 0; i < texturegrid * texturegrid; i++)
	{
		file.read( reinterpret_cast<char*>(&materialindex), 2 );
		file_dest.write( reinterpret_cast<char*>(&materialindex), 2 );
	}

	//qDebug() << "Reading texture indexes...";

	// textures indexes
	for (int i = 0; i < texturegrid * texturegrid; i++)
	{
		short index;
		file.read( reinterpret_cast<char*>(&index), 2 );
		file_dest.write( reinterpret_cast<char*>(&index), 2 );
	}

	//qDebug() << "Reading texture names...";

	// textures 32 char length and total of 512
	for (int ix = 0; ix < 512; ix++)
	{
		char texture[32];
		file.read( texture, 32 );
		file_dest.write( texture, 32 );
	}

	//qDebug() << "Reading 3d objects...";

	long ConvertedObjects = 0, TotalObjects = 0;

	// objects
	while (!file.atEnd())
	{
		Object obj;
		file.read( reinterpret_cast<char*>(&obj), sizeof(Object) );

		// compare and if match found, replace object...
		ConvertedObjects = Compare_Strings(obj, ConvertedObjects);

		file_dest.write( reinterpret_cast<char*>(&obj), sizeof(Object) );

		// add one more read object to totals
		TotalObjects++;
	}

	qDebug() << "Total object count: " << TotalObjects << " Converted objects: " << ConvertedObjects << "\nAll done, exiting. Have a nice day!";

	file.close();
	file_dest.close();
}


// read our replace list from config file
void Replace_Object::Config_Read(char *argv[])
{
	QFile file(argv[3]);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "Unable to open " << file.fileName();
		exit(1);
	}
	QTextStream in(&file);

	qDebug() << "Opened config file: " << file.fileName();

	QString line;
	QStringList list;

	while (!in.atEnd())
	{
		line = in.readLine();
		line = line.toLower();
		list = line.split(",");

		for (int i = 0; i < list.size(); i++)
		{
			coll.insert( list[0], QStringList() << list[i] );
		}
	}
	qDebug() << "Read " << coll.size() << " config lines.";

	file.close();
}


// compare strings and do replace if found a match
int Replace_Object::Compare_Strings(Object &obj, long ConvertedObjects)
{
	//printf("obj.path: %s\n", obj.path);
	QString line(obj.path);
	line = line.toLower();
	QMap<QString, QStringList>::iterator it;

	//qDebug() << "compare_strings line: " << line;

	for ( it=coll.begin(); it != coll.end(); ++it )
	{
		//qDebug() << "coll: " << it.key();
		if ( line.compare( it.key() ) == 0)
		{
			//qDebug() << "Replaced line: " << line;
			int idx = qrand() % it.value().count();
			line.replace(it.key(), it.value()[idx] );
			//qDebug() << " with: " << line;
			strcpy(obj.path, line.toUtf8());
			ConvertedObjects++;
			// break the loop, we found our match.
			break;
		}
	}
	return ConvertedObjects;
}


void Replace_Object::Header()
{
	printf("\nReplace Objects (formely known as 'ArmATool') (c) PMC 2006-2015.\n\n");
};


// Show some help for users who did not give command line.
void Replace_Object::Showhelp()
{
	printf("Usage:\nReplace_Objects Source.WRP Destination.WRP Config_File\n\n");
}
