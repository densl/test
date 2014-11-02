// sdfs.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "windows.h"

// ref: http://msdn.microsoft.com/en-us/library/ms687414(v=vs.85).aspx

// global definition***************************************
#define SVCNAME TEXT("ServiceDemo78")

SERVICE_STATUS			gSvcStatus;
SERVICE_STATUS_HANDLE	gSvcStatusHandle;
HANDLE					ghSvcStopEvent = NULL;


// function declare
void SvcInstall(void);
void WINAPI SvcCtrlHandler( DWORD dwCode);
void WINAPI SvcMain( DWORD, LPTSTR *);

void ReportSvcStatus( DWORD, DWORD, DWORD);
void SvcInit( DWORD, LPTSTR *);
void SvcReportEvent( LPTSTR);

void SvcUninstall(TCHAR *serviceName);

// service's main function***************************************
void _tmain(int argc, TCHAR *argv[])
{
	// check actions ******
	if (lstrcmpi( argv[1], TEXT("install")) == 0)
	{
		printf("Svc install.\n");
		SvcInstall();
		return;
	}

	if (lstrcmpi( argv[1], TEXT("remove")) == 0)
	{
		wprintf(_T("Svc remove %s.\n"), argv[2]);
		SvcUninstall(argv[2]);
		return;
	}

	// set dispatch table *****
	SERVICE_TABLE_ENTRY DispatchTable[] =
	{
		{ SVCNAME, (LPSERVICE_MAIN_FUNCTION)SvcMain },
		{ NULL, NULL }
	};

	// dispatch service control *****
	if ( !StartServiceCtrlDispatcher(DispatchTable) )
	{
		SvcReportEvent(TEXT("StartServiceCtrlDispatcher"));
	}
}

// install a service in the SCM database
void SvcInstall()
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	TCHAR szPath[MAX_PATH];

	// get current moduel name
	if ( !GetModuleFileName(NULL, szPath, MAX_PATH))
	{
		printf("Cannot install service (%d)\n", GetLastError());
		return;
	}

	// get a handler of SCM
	schSCManager = OpenSCManager(
					NULL,		// local computer
					NULL,		// ServicesActive database
					SC_MANAGER_ALL_ACCESS); // full access rights
	if (schSCManager == NULL)
	{
		printf("OpenSCManager failed (%d)\n", GetLastError());
		return;
	}

	// create the service
	schService = CreateService(
		schSCManager,			// SCM database
		SVCNAME,				// name of service
		SVCNAME,				// service name to display
		SERVICE_ALL_ACCESS,		// desired access
		SERVICE_WIN32_OWN_PROCESS,	// service type
		SERVICE_DEMAND_START,	// start type
		SERVICE_ERROR_NORMAL,	// error control type
		szPath,					// path to service's binary
		NULL,					// no load ordering group
		NULL,					// no tag identifier
		NULL,					// no dependencies
		NULL,					// local system account
		NULL);					// no password
	if (schService == NULL)
	{
		int err = GetLastError();
		if (err = ERROR_SERVICE_EXISTS)
		{
			schService = OpenService(schSCManager, SVCNAME, SERVICE_ALL_ACCESS);
		}
		else
			printf("CreateService failed (%d)\n", err);
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}

// entry point for the service *********************************
void WINAPI SvcMain( DWORD dwArgc, LPTSTR *lpszArgv)
{
	// first register the handler function for the service
	gSvcStatusHandle = RegisterServiceCtrlHandler(
		SVCNAME,
		SvcCtrlHandler);
	if ( !gSvcStatusHandle)
	{
		SvcReportEvent(TEXT("RegisterServiceCtrlHandler"));
		return;
	}

	// set status and report initial status
	gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	gSvcStatus.dwServiceSpecificExitCode = 0;
	ReportSvcStatus( SERVICE_START_PENDING, NO_ERROR, 3000);

	SvcInit( dwArgc, lpszArgv);
}


// service code *********************************
void SvcInit( DWORD dwArgc, LPTSTR *lpszArgv)
{

	//create an event, receives the stop control code *******************
	ghSvcStopEvent = CreateEvent(
						NULL,	//default security attributes
						TRUE,	//manual reset event
						FALSE,	//not signaled
						NULL);	//no name

	if (ghSvcStopEvent == NULL)
	{
		ReportSvcStatus( SERVICE_STOP, NO_ERROR, 0);
		return;
	}

	//go on init **************************************

	//report running status when initialization is complete
	ReportSvcStatus( SERVICE_RUNNING, NO_ERROR, 0);


	while(1)
	{
		// do something *******************
		FILE *fp;
		fp = fopen("d:\\service.txt",  "w+");
		char * buf = "hello 5";
		fprintf(fp, "Create success . \n %s --- time: %ld", buf, GetCurrentTime());
		fclose(fp);


		// check whether to stop the service
		WaitForSingleObject(ghSvcStopEvent, INFINITE);
		ReportSvcStatus( SERVICE_STOPPED, NO_ERROR, 0);
		return;
	}
}


// set the current service status and reports it to the service control manager.
void ReportSvcStatus(	DWORD dwCurrentState,
						DWORD dwWin32ExitCode,
						DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;

	// Fill in the SERVICE_STATUS structure
	gSvcStatus.dwCurrentState = dwCurrentState;
	gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
	gSvcStatus.dwWaitHint = dwWaitHint;

	// set control state
	if (dwCurrentState == SERVICE_RUNNING)
		gSvcStatus.dwControlsAccepted = 0;
	else
		gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	// set check point
	if ( (dwCurrentState == SERVICE_RUNNING) ||
			(dwCurrentState == SERVICE_STOPPED))
		gSvcStatus.dwCheckPoint = 0;
	else
		gSvcStatus.dwCheckPoint = dwCheckPoint++;

	SetServiceStatus( gSvcStatusHandle, &gSvcStatus);
}




// Logs messages to the event log *****************
void SvcReportEvent(LPTSTR szFunction)
{
	HANDLE hEventSource;
	LPCTSTR lpszString[2];
	TCHAR Buffer[80];
	memset(Buffer, 0, 80 * sizeof(TCHAR));
	// register event source *****
	hEventSource = RegisterEventSource(NULL, SVCNAME);

	if (hEventSource == NULL)
		return;

	
	wsprintf(Buffer, TEXT("%s failed with %d"), szFunction, GetLastError());
	
	//report event *****
	lpszString[0] = SVCNAME;
	lpszString[1] = Buffer;
	ReportEvent(hEventSource,			// event log handle
				EVENTLOG_ERROR_TYPE,	// event type
				0,						// category
				0,//SVC_ERROR,				// event identifier
				NULL,					// no security identifier
				2,						// size of lpszString array
				0,						// no binary data
				lpszString,				// array of strings
				NULL);					// no binary data

	//release event source
	DeregisterEventSource(hEventSource);
}

// called by SCM whenever a control code is send to the service
void WINAPI SvcCtrlHandler( DWORD dwCtrl)
{
	// Handle the requested control code.
	switch(dwCtrl)
	{
	case SERVICE_CONTROL_STOP:
		ReportSvcStatus( SERVICE_STOP_PENDING, NO_ERROR, 0);

		SetEvent(ghSvcStopEvent);
		ReportSvcStatus(gSvcStatus.dwCurrentState, NO_ERROR, 0);
		return;
	default:
		break;
	}
}


// uninstall a service in the SCM database
void SvcUninstall(TCHAR *serviceName)
{
	int ret;
	SC_HANDLE schSCManager;
	SC_HANDLE schService;

	if (serviceName == NULL)
	{
		return;
	
	}
	
	// get a handler of SCM *********************************
	schSCManager = OpenSCManager(
		NULL,		// local computer
		NULL,		// ServicesActive database
		SC_MANAGER_ALL_ACCESS); // full access rights
	if (schSCManager == NULL)
	{
		printf("OpenSCManager failed (%d)\n", GetLastError());
		return;
	}

	// open service *********************************
	schService = OpenService(schSCManager, serviceName, SERVICE_STOP | DELETE);
	if (schService == NULL)
	{
		int err = GetLastError();
		printf("Open service failed (%d)\n", err);
		return;
	}

	// stop and delete service *********************************
	SERVICE_STATUS mSvcStatus;
	ret = ControlService( schService, SERVICE_CONTROL_STOP, &mSvcStatus);
	ret = DeleteService(schService);
	if (ret)
	{
		printf("delete service success.\n");
	}
	else
	{
		printf("Delete service failed (%d)\n", GetLastError());
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}
