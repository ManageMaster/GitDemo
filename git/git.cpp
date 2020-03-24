#include "git.hpp"


bool Git::init_flag = false;

Git::Git() :
	rep(NULL){
	if (init_flag == false) {
		::git_libgit2_init();
		init_flag = true;
	}
}

Git::~Git(){
	if(rep) ::git_repository_free(rep);
}

git_err_t Git::Open(const char* path) {
	assert(rep == NULL);
	GIT_ERROR_CHECK(::git_repository_open(&rep, path));
	return GITCPP_OK;
}


void Git::SetURL(const char* _username, const char* _password, const char* url) {
	assert(url != NULL);

	this->remote_url.clear();
	this->username.clear();
	this->password.clear();

	if (_username  && _password) {
		this->username = _username;
		this->password = _password;
		char* url_position = ::strstr((char*)url, "https://");
		if (url_position == NULL) {
			LOG_ERROR("input error url!");
			return;
		}
		url_position += strlen("https://");
		this->remote_url += "https://";
		this->remote_url += this->username;
		this->remote_url += ":";
		this->remote_url += this->password;
		this->remote_url += "@";
		this->remote_url += url_position;
	}
	else {
		this->remote_url = url;
	}
}
git_err_t Git::Clone(const char* path) {
	assert(rep == NULL);
	if (this->remote_url.empty()) {
		LOG_ERROR("URL has not set");
		return GITCPP_FAIL;
	}
	git_clone_options opt = GIT_CLONE_OPTIONS_INIT;
	GIT_ERROR_CHECK(::git_clone(&rep, this->remote_url.c_str(), path, &opt));
	return GITCPP_OK;
}

git_err_t Git::Fetch() {
	assert(rep != NULL);
	git_remote* remote = NULL;
	GIT_ERROR_CHECK(::git_remote_lookup(&remote, rep, "origin"));
	GIT_ERROR_CHECK(::git_remote_fetch(remote, NULL, NULL, NULL));
	return GITCPP_OK;
}

git_err_t Git::Checkout(const char* branch_name) {
	assert(rep != NULL);

	git_reference * checkout_branch = NULL;

	//1.try to checkout int local branch
	err = ::git_branch_lookup(&checkout_branch, rep, branch_name, GIT_BRANCH_LOCAL);
	if (err == 0) {
		git_repository_set_head(rep, git_reference_name(checkout_branch));
		return GITCPP_OK;
	}	
	
	//2.cannot find out direct branch in local,try to find in remote
	int remote_ref_size = strlen("refs/remotes/origin/") + strlen(branch_name) + 1;
	char remote_ref_name[remote_ref_size];
	MEMCLR(remote_ref_name, remote_ref_size);

	memcpy(remote_ref_name, "refs/remotes/origin/", strlen("refs/remotes/origin/"));
	memcpy(remote_ref_name + strlen("refs/remotes/origin/"), branch_name, strlen(branch_name));

	LOG_INFO("remote fer name ", remote_ref_name);

	GIT_ERROR_CHECK(::git_reference_lookup(&checkout_branch, rep, remote_ref_name));

	git_annotated_commit* commit = NULL;
	GIT_ERROR_CHECK(::git_annotated_commit_from_ref(&commit, rep, checkout_branch));

	git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
	checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
	checkout_opts.checkout_strategy = GIT_CHECKOUT_FORCE;

	git_reference* ref = NULL, * branch = NULL;
	git_commit* target_commit = NULL;

	GIT_ERROR_CHECK(::git_commit_lookup(&target_commit, rep, git_annotated_commit_id(commit)));
	GIT_ERROR_CHECK(::git_checkout_tree(rep, (const git_object*)target_commit, &checkout_opts));

	if (git_annotated_commit_ref(commit)) {
		const char* target_head;

		GIT_ERROR_CHECK(::git_reference_lookup(&ref, rep, git_annotated_commit_ref(commit)));

		if (git_reference_is_remote(ref)) {
			GIT_ERROR_CHECK(::git_branch_create_from_annotated(&branch, rep, branch_name, commit, 0));
			target_head = git_reference_name(branch);
		}
		else {
			target_head = git_annotated_commit_ref(commit);
		}
		err = git_repository_set_head(rep, target_head);
	}
	else {
		err = git_repository_set_head_detached_from_annotated(rep, commit);
	}

	
	return GITCPP_OK;
}

git_err_t Git::Merge(const char* ToMerge, const char* Merged) {
	assert(rep != NULL);
	git_reference* merge = NULL;
	git_reference* branch = NULL;
	git_merge_options merge_opt = GIT_MERGE_OPTIONS_INIT;
	git_checkout_options checkout_opt = GIT_CHECKOUT_OPTIONS_INIT;

	git_branch_lookup(&merge, rep, ToMerge, GIT_BRANCH_LOCAL);
	git_branch_lookup(&branch, rep, Merged, GIT_BRANCH_LOCAL);
	git_repository_set_head(rep, git_reference_name(merge));

	const git_annotated_commit* head = NULL;
	git_annotated_commit_from_ref((git_annotated_commit**)&head, rep, branch);

	GIT_ERROR_CHECK(::git_merge(rep, &head, 1, &merge_opt, &checkout_opt));

	git_index* index = NULL;
	git_repository_index(&index, rep);
	if (git_index_has_conflicts(index)) {
		return GITCPP_CONFLICT;
	}
	return GITCPP_OK;
}

git_err_t Git::Commit(const char* branch_name,
				const char* Author,
				const char* Email,
				const char* message){
	assert(rep != NULL);

	if (!Author || !Email || !message) {
		LOG_ERROR("lack of commit message");
		return GITCPP_FAIL;
	}
	git_reference* branch = NULL;

	GIT_ERROR_CHECK(::git_branch_lookup(&branch, rep, branch_name, GIT_BRANCH_LOCAL));

	git_commit* commit = NULL;
	::git_commit_lookup(&commit, rep, ::git_reference_target(branch));

	git_index* index = NULL;
	git_repository_index(&index, rep);
	git_index_write(index);
	git_oid new_tree_id;
	err = git_index_write_tree(&new_tree_id, index);

	git_tree* new_tree = NULL;
	git_tree_lookup(&new_tree, rep, &new_tree_id);

	git_signature* sig = NULL;
	::git_signature_now(&sig, Author, Email);

	git_oid new_commit;
	GIT_ERROR_CHECK(::git_commit_create(&new_commit, rep, "HEAD", sig, sig, "UTF-8", message, new_tree, 1,
		(const git_commit**)&commit));
	return GITCPP_OK;
}

git_err_t Git::Push(const char* branch_name) {
	assert(rep != NULL);
	assert(!this->remote_url.empty());
	git_remote* remote = NULL;
	git_push_options opts = GIT_PUSH_OPTIONS_INIT;
	
	GIT_ERROR_CHECK(::git_remote_set_pushurl(rep, "origin", this->remote_url.c_str()));
	GIT_ERROR_CHECK(::git_remote_lookup(&remote, rep, "origin"));

	std::string head;
	head += "refs/heads/";
	head += branch_name;
	head += ":refs/heads/";
	head += branch_name;

	const char* refs[] = { head.c_str() };
	git_strarray strarr = { (char**)refs, 1 };
	GIT_ERROR_CHECK(::git_remote_push(remote, &strarr, &opts));
	return GITCPP_OK;
}