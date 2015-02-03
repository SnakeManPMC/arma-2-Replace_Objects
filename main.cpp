#include <QtCore/QCoreApplication>
#include "replace_object.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	Replace_Object rep;
	rep.RunMe(argc, argv);

	return 0;
}
