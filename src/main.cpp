#include "config.h"
#include "forms/select_profile.h"
#include <QApplication>

int main(int argc, char **argv)
{
	QApplication qapp(argc, argv);
	qapp.setOrganizationDomain("astralkey.nl");
	qapp.setOrganizationName("AstralKey");
	qapp.setApplicationName(APP_NAME);

	forms::SelectProfile lWindow;
	lWindow.show();

	return qapp.exec();
}
