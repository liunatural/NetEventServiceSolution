#pragma once
#include <chrono>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "PlayerManager.h"

using namespace boost::asio;
using boost::asio::io_service;

//毫秒级定时器， 每隔30毫秒触发一次， 更新玩家的位置信息
class SvcTimer
{
public:
	SvcTimer(io_service& ios, PlayerManager*& playerMgr);
	virtual ~SvcTimer();

	void handler();

	void SendTransformDataByUserType(UserType usrType);

private:
	deadline_timer mTimer;
	PlayerManager* mPlayMgr;

	char buffer[MessagePackage::max_body_length] = { 0 };

};

