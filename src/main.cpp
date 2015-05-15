#include "config.h"
#include "core/application.h"
#include "forms/select_profile.h"
#include <QDebug>

int main(int argc, char **argv)
{
	try
	{
		Application qapp(argc, argv);
		qapp.setOrganizationDomain("astralkey.nl");
		qapp.setOrganizationName("AstralKey");
		qapp.setApplicationName(APP_NAME);

		forms::SelectProfile * lWindow = new forms::SelectProfile();
		lWindow->setAttribute(Qt::WA_DeleteOnClose);
		lWindow->show();

		return qapp.exec();
	}
	catch (Exception & e)
	{
		qCritical() << e.title() << ':' << e.description();
	}
	catch (QException & e)
	{
		qCritical() << "Uncaught Qt exception!";
	}
	catch (...)
	{
		qCritical() << "Uncaught unknown exception!";
	}
	return EXIT_FAILURE;
}
