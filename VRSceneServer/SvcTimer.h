#pragma once
#include <chrono>
#include <asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "PlayerManager.h"

using namespace asio;

//毫秒级定时器， 每隔30毫秒触发一次， 更新玩家的位置信息
class SvcTimer
{
public:
	SvcTimer(asio::io_service& ios, PlayerManager*& playerMgr);
	virtual ~SvcTimer();

	void handler();

private:
	asio::deadline_timer mTimer;
	PlayerManager* mPlayMgr;
	char buffer[MessagePackage::max_body_length] = { 0 };
};

