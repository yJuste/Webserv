// ************************************************************************** //
//                                                                            //
//                Exceptions.hpp                                              //
//                Created on  : xxx Jul xx xx:xx:xx 2025                      //
//                Last update : xxx Jul xx xx:xx:xx 2025                      //
//                Made by     :                                               //
//                                                                            //
// ************************************************************************** //

#ifndef EXCEPTIONS_HPP
# define EXCEPTIONS_HPP

# ifndef MSG_SIZE
#  define MSG_SIZE 256
# endif

// ************************************************************************** //
//                                  Exceptions                                //
// ************************************************************************** //

// Functions

class	FailedSocket : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: socket() failed."; } };

class	FailedSetsockopt : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: setsockopt() failed."; } };

class	FailedGetaddrinfo : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: getaddrinfo() failed."; } };

class	FailedBind : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: bind() failed."; } };

class	FailedListen : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: listen() failed."; } };

class	FailedOpen : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: open() failed."; } };

class	FailedAcstat : public std::exception { private: char _msg[MSG_SIZE]; public: FailedAcstat( const char * s ) { std::snprintf(_msg, sizeof(_msg), "\033[31merror\033[0m: `%s`: acstat() failed.", s); } const char * what() const throw() { return _msg; } };

class	FailedRealpath : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: realpath() failed."; } };

class	FailedGetcwd : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: getcwd() failed."; } };

// Parsing

class	BracketsNotClosed : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: Brackets are not closed."; } };

class	NoEndingSemicolon : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: Missing/Left with a semicolon at the end of a line."; } };

class	ValueNotGiven : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: No given value for one parameter."; } };

class	InvalidParameter : public std::exception { private: char _msg[MSG_SIZE]; public: InvalidParameter( const char * s ) { std::snprintf(_msg, sizeof(_msg), "\033[31merror\033[0m: `%s`: Invalid parameter is given.", s); } const char * what() const throw() { return _msg; } };

class	InvalidListen : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: The 'port' has to be a number."; } };

class	LocationNotGiven : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: No given value for the parameter 'location'."; } };

class	ErrorPageNotGiven : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: Error_page has to be in the format : [nb dir] [nb dir] ..."; } };

class	MaxSizeNotGiven : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: Client max body size has to be a number."; } };

class	Overflow : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: A number has overflowed, please be careful."; } };

class	MethodErrors : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: A method does not exist or is duplicated."; } };

class	RedirXDefault : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: Redirect is enabled but no target is defined."; } };

class	DefaultNotGiven : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: No given value for the parameter 'default'."; } };

class	InvalidRedirect : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: Invalid boolean value for the parameter 'redirect'."; } };

class	InvalidAutoindex : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: Invalid boolean value for the parameter 'autoindex'."; } };

class	ExtensionCgi : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: Invalid extension given for the parameter 'cgi' (should be [\033[91m.py\033[0m, \033[91m.php\033[0m])."; } };

class	ProgramCgi : public std::exception { private: char _msg[MSG_SIZE]; public: ProgramCgi( const char * s ) { std::snprintf(_msg, sizeof(_msg), "\033[31merror\033[0m: `%s`: Invalid program path for the parameter 'cgi'.", s); } const char * what() const throw() { return _msg; } };

class	DuplicateCgi : public std::exception { private: char _msg[MSG_SIZE]; public: DuplicateCgi( const char * s ) { std::snprintf(_msg, sizeof(_msg), "\033[31merror\033[0m: `%s`: This extension already exists.", s); } const char * what() const throw() { return _msg; } };

class	DuplicateParameter : public std::exception { private: char _msg[MSG_SIZE]; public: DuplicateParameter( const char * s ) { std::snprintf(_msg, sizeof(_msg), "\033[31merror\033[0m: `%s`: This parameter cannot be duplicated.", s); } const char * what() const throw() { return _msg; } };

class	OverwrittenParameter : public std::exception { private: char _msg[MSG_SIZE]; public: OverwrittenParameter( const char * s ) { std::snprintf(_msg, sizeof(_msg), "\033[38;5;220mwarning\033[0m: `%s`: Value(s) may be overwritten.", s); } const char * what() const throw() { return _msg; } };

class	DuplicateLocation : public std::exception { private: char _msg[MSG_SIZE]; public: DuplicateLocation( const char * s ) { std::snprintf(_msg, sizeof(_msg), "\033[31merror\033[0m: `%s`: The location with this path already exists.", s); } const char * what() const throw() { return _msg; } };

class	MissingImportantValues : public std::exception { private: char _msg[MSG_SIZE]; public: MissingImportantValues( const char * s ) { std::snprintf(_msg, sizeof(_msg), "\033[31mwarning\033[0m: `%s`: Important value for server operation.", s); } const char * what() const throw() { return _msg; } };

#endif
