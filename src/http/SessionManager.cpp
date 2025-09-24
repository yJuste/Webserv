/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SessionManager.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: layang <layang@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 11:29:36 by layang            #+#    #+#             */
/*   Updated: 2025/09/24 15:47:26 by layang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SessionManager.hpp"
#include <iostream>

SessionManager::SessionManager()
{
    std::srand(std::time(NULL));
}

SessionManager::~SessionManager()
{
    _sessions.clear(); 
}

std::string SessionManager::generateSesssionId()
{
    std::ostringstream oss;
    for (int i = 0; i < 16; i++)
    {
        int r = std::rand() % 256;
        if (r < 16)
            oss << '0';
        oss << std::hex << r;
    }
    return oss.str();
}

std::string SessionManager::createSession(const std::string username)
{
	std::string sessionId;
	do {
		sessionId = generateSesssionId();
	} while (_sessions.find(sessionId) != _sessions.end());

	Session session;
	session.username = username;
	session.mode = "normal";
	session.expire = std::time(NULL) + 3600;
	_sessions[sessionId] = session;

	return sessionId;
}

Session* SessionManager::getSession(const std::string& sessionId)
{
	if (sessionId.empty())
		return NULL;
	std::map<std::string, Session>::iterator it = _sessions.find(sessionId);
	if (it != _sessions.end())
	{
		if (std::time(NULL) <  it->second.expire)
			return &it->second;
		else
			_sessions.erase(it);
	}
	return NULL;
}

void SessionManager::cleanupExpired()
{
	time_t now = std::time(NULL);
	std::map<std::string, Session>::iterator it = _sessions.begin();
	while (it != _sessions.end())
	{
		if (it->second.expire < now)
		{
			std::map<std::string, Session>::iterator toErase = it;
			++it;
			_sessions.erase(toErase);
		}
		else
			++it;
	} 
}

/*
1:
HTTP/1.1 200 OK
Content-Type: text/html
Set-Cookie: session_id=abc123; Max-Age=3600; HttpOnly
2:
GET /something HTTP/1.1
Host: localhost:8080
Cookie: session_id=abc123

if (_request.isComplete()) {
    HttpResponse response;

    // session 部分
    std::string sid = _request.getHeader("session_id");
    Session* s = sessionManager.getSession(sid);
    if (!s) {
        sid = sessionManager.createSession("guest");
        response.setCookie("session_id", sid, 3600, true);
        s = sessionManager.getSession(sid);
    }
    s->mode = "rainbow";

    response.buildResponse(_request, _server);
    _writebBuffer = response.toString(_request);
    _readBuffer.clear();
    return true;
}


std::string html = loadFile("index.html");
std::string mode = session->mode; 
size_t pos = html.find("{{MODE}}");
if (pos != std::string::npos) {
    html.replace(pos, 8, mode);
}
response.setBody(html);
response.setHeader("Content-Type", "text/html");


*****在 Connection.cpp 或 Server.cpp 里这样用:
std::string sid = request.getHeader("session_id");
Session* s = sessionManager.getSession(sid);
if (!s) {
    sid = sessionManager.createSession("guest");
    response.setCookie("session_id", sid, 3600, true);
    s = sessionManager.getSession(sid);
}
s->mode = "rainbow";

// Poller.cpp add cleanupExpired()
void Poller::run(const std::vector<Server*>& servers)
{
    while (true)
    {
        int ret = poll(_fds.data(), _fds.size(), 1000); // 每1秒唤醒一次，即使没事件
        if (ret < 0)
            throw std::runtime_error("poll failed");

        // ⭐ 每次 poll 完就清理一次 session
        for (size_t s = 0; s < servers.size(); ++s)
        {
            servers[s]->getSessionManager().cleanupExpired();
        }

        for (size_t i = 0; i < _fds.size(); i++)
        {
            int fd = _fds[i].fd;
*/
