#pragma once

#include "git2.h"
#include "system.h"
#include <string>


#define GIT_ERROR_CHECK(func) \
	do{\
		if(func < 0){\
			const git_error* e = giterr_last();\
			LOG_ERROR("Error: ", err, " / ", e->klass, " : ", e->message);\
			return GITCPP_FAIL;\
		}\
	}while(0)

typedef enum {
	GITCPP_OK = 0,
	GITCPP_CONFLICT,
	GITCPP_FAIL,
	GITCPP_ERR_MAX,
} git_err_t;

class Git {
public:
	Git();

	~Git();

	git_err_t Open(const char* path);

	void SetURL(const char* _username, const char* _password, const char* url);

	git_err_t Clone(const char* path);

	git_err_t Fetch();

	git_err_t Checkout(const char* branch_name);

	git_err_t Merge(const char* ToMerge,const char* Merged);

	git_err_t Commit(const char* branch_name,
				const char* Author,
				const char* Email,
				const char* message);

	git_err_t Push(const char* branch_name);

public:

	static bool init_flag;

private:
	git_repository* rep;
	int err;

	std::string username;
	std::string password;

	std::string remote_url;
};
