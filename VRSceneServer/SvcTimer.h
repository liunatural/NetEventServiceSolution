#pragma once
#include <chrono>
#include <asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "PlayerManager.h"

using namespace asio;

//���뼶��ʱ���� ÿ��30���봥��һ�Σ� ������ҵ�λ����Ϣ
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

