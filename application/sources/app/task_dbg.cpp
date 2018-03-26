#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "sys_ctrl.h"
#include "sys_io.h"
#include "sys_dbg.h"

#include "utils.h"

#include "app.h"
#include "app_if.h"
#include "app_dbg.h"

#include "task_list.h"
#include "task_list_if.h"
#include "task_dbg.h"

#include "link_sig.h"
#include "link.h"

void task_dbg(ak_msg_t* msg) {
	switch (msg->sig) {
	case AC_DBG_TEST_1: {
		APP_DBG_SIG("AC_DBG_TEST_1\n");
	}
		break;
	default:
		break;
	}
}
