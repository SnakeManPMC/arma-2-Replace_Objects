#ifndef REPLACE_OBJECT_H
#define REPLACE_OBJECT_H

#include <QFile>
#include <QMap>
#include <QStringList>

class Replace_Object
{
public:
	Replace_Object();

	// for the replace objects string search
	QMap<QString, QStringList> coll;

	void RunMe(int argc, char *argv[]);

	struct Object
	{
		float rotation[9];	// rotation matrix
		float x, y, z;		// position on island
		unsigned int index;	// object index
		char path[76];		// path to object p3d
	};

private:
	int texturegrid;
	void Header();
	void Showhelp();
	void Config_Read(char *argv[]);
	int Compare_Strings(Object &obj, long ConvertedObjects);
};

#endif // REPLACE_OBJECT_H
