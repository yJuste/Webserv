/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file_utils.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: layang <layang@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 10:39:02 by layang            #+#    #+#             */
/*   Updated: 2025/09/24 12:10:57 by layang           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILE_UTILS_HPP
# define FILE_UTILS_HPP

# include "Location.hpp"
# include "HttpRequest.hpp"
# include <string>
# include <sstream>

// ----- Responding -----
// file_utils.cpp
std::string getCurrentWorkingDirectory();
long getFileSize(const std::string &path);
std::string combineIndexPath(const std::string &dirPath, const std::string &index, const std::string &root);
std::string generateDirectoryListing(const std::string &dirPath, const std::string &reqPath);
bool userExists(const std::string &username);
std::string getFormValue(const std::string &body, const std::string &key);
bool saveUser(const std::string &username, const std::string &password);
bool checkUser(const std::string &username, const std::string &password);
bool saveUploadedFile(const HttpRequest &req, const std::string &uploadDir);
std::string resolvePath(const Location* loc, const std::string &reqPath);
std::string readFile(const std::string &path);
std::string getContentType(const std::string &path);
template <typename T>
std::string pushToString(T value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}
std::string portsToString(const std::vector<int> &ports);
std::vector<std::string> buildCgiEnv(const HttpRequest &req,
                               const std::string &filePath,
                               const Server &server);
int	acstat_file( const char * path, int mode );
std::string getSessionIdFromCookie(const std::string &cookie);

#endif
