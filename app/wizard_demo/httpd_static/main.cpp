#include "stdafx.h"
#include "master_service.h"
#include "http_service.h"

#if __cplusplus >= 201103L	// Support c++11 ?
# define USE_LAMBDA
#endif

static bool load_html(const char* path, HttpResponse& res)
{
	acl::string path_buf(path);
	path_buf.strip("..");
	char* end = path_buf.buf_end();
	if (end && *(end - 1) == '/') {
		path_buf.set_offset(path_buf.size() - 1);
	}

	const char* slash = strrchr(path_buf, '/');
	if (slash) {
		slash++;
		if (strrchr(slash, '.') == NULL) {
			path_buf << "/index.html";
		}
	}

	//printf("%s\n", path_buf.c_str());
	acl::string ctype("text/html");

#define	EQ	!strcasecmp

	const char* ext = strrchr(path_buf.c_str(), '.');
	if (ext && *++ext) {
		if (EQ(ext, "js")) {
			ctype = "text/js";
		} else if (EQ(ext, "zip")) {
			ctype = "application/zip";
		} else if (EQ(ext, "css")) {
			ctype = "text/css";
		} else if (EQ(ext, "png")) {
			ctype = "image/png";
		} else if (EQ(ext, "jpg")) {
			ctype = "image/jpg";
		}
	}

	acl::string filepath;
	filepath << var_cfg_html_path << "/" << path_buf;

	acl::string buf;
	if (!acl::ifstream::load(filepath, buf)) {
		logger_error("load %s error %s",
			filepath.c_str(), acl::last_serror());
		buf.format("404 %s not found\r\n", path);
		res.setStatus(404).setContentLength(buf.size());
		return res.write(buf);
	}

	res.setContentType(ctype);
	res.setContentLength(buf.size());
	return res.write(buf);
}

static bool http_get_default(const char* path, HttpRequest&, HttpResponse& res)
{
	return load_html(path, res);
}

static bool http_get_root(HttpRequest&, HttpResponse& res)
{
	return load_html("/index.html", res);
}

int main(int argc, char* argv[])
{
	// 初始化 acl 库
	acl::acl_cpp_init();

	master_service ms;
	http_service& service = ms.get_service();;

	// 设置配置参数表
	ms.set_cfg_int(var_conf_int_tab)
		.set_cfg_int64(var_conf_int64_tab)
		.set_cfg_str(var_conf_str_tab)
		.set_cfg_bool(var_conf_bool_tab);

	// Register http handlers according different url path
	service.Get("/", http_get_root)
		.Default(http_get_default);

	// 开始运行

	if (argc == 1 || (argc >= 2 && strcmp(argv[1], "alone") == 0)) {
		// 日志输出至标准输出
		acl::log::stdout_open(true);

		// 监听的地址列表，格式：ip|port1,ip|port2,...
		const char* addrs = "|8888";
		printf("listen on: %s\r\n", addrs);

		// 测试时设置该值 > 0 则指定服务器处理客户端连接过程的
		// 会话总数（一个连接从接收到关闭称之为一个会话），当
		// 处理的连接会话数超过此值，测试过程结束；如果该值设
		// 为 0，则测试过程永远不结束
		unsigned int count = 0;

		// 测试过程中指定线程池最大线程个数
		unsigned int max_threads = 100;

		// 单独运行方式

		if (argc >= 3) {
			ms.run_alone(addrs, argv[2], count, max_threads);
		} else {
			ms.run_alone(addrs, NULL, count, max_threads);
		}

		printf("Enter any key to exit now\r\n");
		getchar();
	} else {
#if defined(_WIN32) || defined(_WIN64)
		// 日志输出至标准输出
		acl::log::stdout_open(true);

		// 监听的地址列表，格式：ip:port1,ip:port2,...
		const char* addrs = "127.0.0.1:8888";
		printf("listen on: %s\r\n", addrs);

		// 测试时设置该值 > 0 则指定服务器处理客户端连接过程的
		// 会话总数（一个连接从接收到关闭称之为一个会话），当
		// 处理的连接会话数超过此值，测试过程结束；如果该值设
		// 为 0，则测试过程永远不结束
		unsigned int count = 0;

		// 测试过程中指定线程池最大线程个数
		unsigned int max_threads = 100;

		// 单独运行方式
		ms.run_alone(addrs, NULL, count, max_threads);
		printf("Enter any key to exit now\r\n");
		getchar();
#else
		// acl_master 控制模式运行
		ms.run_daemon(argc, argv);
#endif
	}

	return 0;
}
