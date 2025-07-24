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

// ************************************************************************** //
//                                  Exceptions                                //
// ************************************************************************** //

// Functions

class	FailedSocket : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: socket() failed."; } };

class	FailedSetsockopt : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: setsockopt() failed."; } };

class	FailedBind : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: bind() failed."; } };

class	FailedListen : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: listen() failed."; } };

class	FailedOpen : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: open() failed."; } };

// Parsing

class	BracketsNotClosed : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: Brackets are not closed."; } };

class	ListenNotGiven : public std::exception { public : const char * what() const throw() { return "\033[31merror\033[0m: No given value for the parameter 'listen'."; } };

#endif
