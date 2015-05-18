#include "config.h"
#include "application.h"
#include "forms/select_profile.h"
#include <QDebug>

namespace
{

}

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
		qCritical() << "MAIN :" << e.title() << ':' << e.description();
	}
	catch (QException & e)
	{
		qCritical() << "MAIN : Uncaught Qt exception!";
	}
	catch (...)
	{
		qCritical() << "MAIN : Uncaught unknown exception!";
	}
	return EXIT_FAILURE;
}
