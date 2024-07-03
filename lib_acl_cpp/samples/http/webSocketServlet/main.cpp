#include "stdafx.h"
#include "master_service.h"

int main(int argc, char* argv[])
{
	// 初始化 acl 库
	acl::acl_cpp_init();

	master_service& ms = acl::singleton2<master_service>::get_instance();

	// 设置配置参数表
	ms.set_cfg_int(var_conf_int_tab);
	ms.set_cfg_int64(var_conf_int64_tab);
	ms.set_cfg_str(var_conf_str_tab);
	ms.set_cfg_bool(var_conf_bool_tab);

	// 开始运行

	if (argc == 1 || (argc >= 2 && strcmp(argv[1], "alone") == 0)) {
		acl::log::stdout_open(true);  // 日志输出至标准输出
		const char* addr = "0.0.0.0:9004";
		const char* conf = NULL;
		if (argc >= 3) {
			conf = argv[2];
		}
		if (argc >= 4) {
			addr = argv[3];
		}

		printf("listen on: %s, conf=%s\r\n", addr, conf ? conf : "null");
		ms.run_alone(addr, conf, 0, 1000);  // 单独运行方式

		printf("Enter any key to exit now\r\n");
		getchar();
	} else {
		ms.run_daemon(argc, argv);  // acl_master 控制模式运行
	}

	return 0;
}
