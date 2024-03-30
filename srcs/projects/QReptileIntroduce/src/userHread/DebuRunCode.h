#ifndef DEBURUNCODE_H_H_HEAD__FILE__
#define DEBURUNCODE_H_H_HEAD__FILE__
#pragma once

#ifndef DEBUG_OUT_CONSOLE
#define DEBUG_RUN( run_code ) 
#else
#define DEBUG_RUN( run_code ) \
	do{ \
		run_code; \
	} while(false)
#endif


#ifndef DEBUG_OUT_CONSOLE
#define DEBU_RUN_IF_PTR( ptr, not_pur ,run_code  ) 
#else
#define DEBUG_RUN_IF_PTR( ptr, not_pur ,run_code ) \
	do{ \
	if(ptr != not_pur)\
		run_code; \
	} while(false)
#endif


#endif // DEBURUNCODE_H_H_HEAD__FILE__
