#pragma once
#include "xLog.h"
#include "xTimer.h"

std::unique_ptr<xLogFile> g_logFile;
int g_total;
FILE* g_file;

void outputFunc(const char* msg, int len)
{
	g_logFile->append(msg, len);
}

void flushFunc()
{
  	g_logFile->flush();
}


void dummyOutput(const char* msg, int len)
{
	g_total += len;
	if (g_file)
	{
		fwrite(msg, 1, len, g_file);
	}
	else if (g_logFile)
	{
		g_logFile->append(msg, len);
	}
}


void bench(const char* type)
{
	xLogger::setOutput(dummyOutput);
	xTimestamp start(xTimestamp::now());
	g_total = 0;

	int n = 1000*1000;
	const bool kLongLog = false;
	std::string empty = " ";
	std::string longStr(3000, 'X');
	longStr += " ";
	for (int i = 0; i < n; ++i)
	{
		LOG_INFO << "Hello 0123456789" << " abcdefghijklmnopqrstuvwxyz"
	         << (kLongLog ? longStr : empty)
	         << i;
	}
	xTimestamp end(xTimestamp::now());
	double seconds = timeDifference(end, start);
	printf("%12s: %f seconds, %d bytes, %10.2f msg/s, %.2f MiB/s\n",
	     type, seconds, g_total, n / seconds, g_total / seconds / (1024 * 1024));
}

int main(int argc, char* argv[])
{
	bench("nop");
	
	char buffer[64*1024];
	
	 g_file = fopen("/dev/null", "w");
	 setbuffer(g_file, buffer, sizeof buffer);
	 bench("/dev/null");
	 fclose(g_file);
	
	 g_file = fopen("/tmp/log", "w");
	 setbuffer(g_file, buffer, sizeof buffer);
	 bench("/tmp/log");
	 fclose(g_file);
	
	 g_file = NULL;
	 g_logFile.reset(new xLogFile("test_log_st", 500*1000*1000, false));
	 bench("test_log_st");
	
	 g_logFile.reset(new xLogFile("test_log_mt", 500*1000*1000, true));
	 bench("test_log_mt");
	 g_logFile.reset();

	LOG_INFO<<"---------";
	sleep(1);
	char name[256];
	strncpy(name,argv[0],256);
	g_logFile.reset(new xLogFile(::basename(name), 200*1000));
	xLogger::setOutput(outputFunc);
	xLogger::setFlush(flushFunc);
	std::string line = "1234567890 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
	for (int i = 0; i < 100000; ++i)
	{
		LOG_INFO << line << i;
	}

	 
	return 0;
}
	

