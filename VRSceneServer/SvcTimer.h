#pragma once
#include <chrono>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "PlayerManager.h"

using namespace boost::asio;
using boost::asio::io_service;

//���뼶��ʱ���� ÿ��30���봥��һ�Σ� ������ҵ�λ����Ϣ
class SvcTimer
{
public:
	SvcTimer(io_service& ios, PlayerManager*& playerMgr);
	virtual ~SvcTimer();

	void handler();

private:
	deadline_timer mTimer;
	PlayerManager* mPlayMgr;
	char buffer[MessagePackage::max_body_length] = { 0 };
};

