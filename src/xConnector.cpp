#include "xConnector.h"

xConnector::xConnector(xEventLoop *loop)
 :loop(loop),
  state(kDisconnected),
  isconnect(false)
{

}


xConnector::~xConnector()
{

}


void xConnector::start(const char *ip, int16_t port)
{
	isconnect = true;
	loop->runInLoop(std::bind(&xConnector::startInLoop, this,ip,port));
}



void xConnector::startInLoop(const char *ip, int16_t port)
{
	loop->assertInLoopThread();
	if (isconnect)
	{
		connect(ip,port);
	}
	else
	{
		LOG_WARN<<"do not connect";
	}
}

void xConnector::stop()
{
	isconnect= false;
	loop->queueInLoop(std::bind(&xConnector::stopInLoop, this));
}

void xConnector::stopInLoop()
{
	loop->assertInLoopThread();
	if (state == kConnecting)
	{
		setState(kDisconnected);
		int sockfd = removeAndResetChannel();

	}
}

int  xConnector::removeAndResetChannel()
{
	return -1;
}

void xConnector::connecting(int sockfd)
{
	if(state == kConnecting)
	{
		newConnectionCallback(sockfd);
	}
	else
	{
		//TRACE("connect error\n");
	}
}


void xConnector::connect(const char *ip, int16_t port)
{
  int sockfd = socket.createSocket();
  int ret = socket.connect(sockfd, ip,port);
  int savedErrno = (ret == 0) ? 0 : errno;
  switch (savedErrno)
  {
    case 0:
    case EINPROGRESS:
    case EINTR:
    case EISCONN:
	socket.setSocketNonBlock(sockfd);
	setState(kConnecting);
	connecting(sockfd);
	socket.setkeepAlive(sockfd,3);
      break;
    default:
      LOG_WARN<<strerror(savedErrno);
      ::close(sockfd);
      setState(kDisconnected);
      if(errorConnectionCallback)
      {
    	 errorConnectionCallback();
      }
      break;
  }
}
