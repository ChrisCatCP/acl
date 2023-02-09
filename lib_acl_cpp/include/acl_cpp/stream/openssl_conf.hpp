#pragma once
#include "../acl_cpp_define.hpp"
#include <vector>
#include "../stdlib/thread_mutex.hpp"
#include "../stdlib/string.hpp"
#include "sslbase_conf.hpp"

typedef struct ssl_st SSL;
typedef struct ssl_ctx_st SSL_CTX;

namespace acl {

class openssl_io;

class ACL_CPP_API openssl_conf : public sslbase_conf {
public:
	openssl_conf(bool server_side = false, int timeout = 30);
	~openssl_conf(void);

	/**
	 * @override
	 */
	bool load_ca(const char* ca_file, const char* ca_path);

	/**
	 * @override
	 */
	bool add_cert(const char* crt_file, const char* key_file,
		const char* key_pass = NULL);

	/**
	 * @override
	 * @deprecate use add_cert(const char*, const char*, const char*)
	 */
	bool add_cert(const char* crt_file);

	/**
	 * @override
	 * @deprecate use add_cert(const char*, const char*, const char*)
	 */
	bool set_key(const char* key_file, const char* key_pass);

	/**
	 * @override
	 */
	void enable_cache(bool on);

public:
	/**
	 * ���ñ���������һ����̬���ȫ·��
	 * @param libcrypto {const char*} libcrypto.so ��̬���ȫ·��
	 * @param libssl {const char*} libssl.so ��̬���ȫ·��
	 */
	static void set_libpath(const char* libcrypto, const char* libssl);

	/**
	 * ��ʽ���ñ���������̬���� libssl.so ��̬��
	 * @return {bool} �����Ƿ�ɹ�
	 */
	static bool load(void);

public:
	// @override sslbase_conf
	sslbase_io* create(bool nblock);

public:
	bool setup_certs(void* ssl);

	bool is_server_side(void) const
	{
		return server_side_;
	}

	SSL_CTX* get_ssl_ctx(void) const;

private:
	friend class openssl_io;

	bool      server_side_;
	SSL_CTX* ssl_ctx_;	// The default SSL_CTX;
	bool     ssl_ctx_inited_;
	std::vector<SSL_CTX*> ssl_ctxes_;
	int      timeout_;
	string   crt_file_;
	unsigned init_status_;
	thread_mutex lock_;

	bool init_once(void);
	SSL_CTX* create_ssl_ctx(void);

	static int ssl_servername(SSL *ssl, int *ad, void *arg);
};

} // namespace acl
