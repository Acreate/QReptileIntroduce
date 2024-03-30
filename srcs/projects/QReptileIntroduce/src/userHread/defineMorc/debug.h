#ifndef DEBUG_H_H_HEAD__FILE__
#define DEBUG_H_H_HEAD__FILE__
#pragma once

// 总是执行该代码输出
#define DEBUG_RUN( run_code ) \
	do{ \
		run_code; \
	} while(false)

// 不相等则运行该规则代码
#define DEBUG_RUN_IF_NOT_EQU_PTR( ptr, not_pur ,run_code ) \
	do{ \
	if(ptr != not_pur)\
		run_code; \
	} while(false)

// 相等则运行该规则代码
#define DEBUG_RUN_IF_IS_EQU_PTR( ptr, not_pur ,run_code ) \
	do{ \
	if(ptr == not_pur)\
		run_code; \
	} while(false)


#endif // DEBUG_H_H_HEAD__FILE__
