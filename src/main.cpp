#include "config.h"
#include <QApplication>

int main(int argc, char **argv)
{
	QApplication qapp(argc, argv);
	qapp.setOrganizationDomain("astralkey.nl");
	qapp.setOrganizationName("AstralKey");
	qapp.setApplicationName(APP_NAME);

	return qapp.exec();
}
