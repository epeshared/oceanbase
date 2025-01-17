// Copyright (c) 2022-present Oceanbase Inc. All Rights Reserved.
// Author:
//   yuya.yu <>

#define USING_LOG_PREFIX SERVER

#include "observer/table_load/ob_table_load_exec_ctx.h"
#include "observer/table_load/ob_table_load_service.h"
#include "sql/engine/ob_exec_context.h"

namespace oceanbase
{
namespace observer
{
using namespace common;
using namespace sql;

/**
 * ObTableLoadSqlExecCtx
 */

ObIAllocator *ObTableLoadSqlExecCtx::get_allocator()
{
  ObIAllocator *allocator = nullptr;
  if (nullptr != exec_ctx_) {
    allocator = &exec_ctx_->get_allocator();
  }
  return allocator;
}

ObSQLSessionInfo *ObTableLoadSqlExecCtx::get_session_info()
{
  ObSQLSessionInfo *session_info = nullptr;
  if (nullptr != exec_ctx_) {
    session_info = exec_ctx_->get_my_session();
  }
  return session_info;
}

int ObTableLoadSqlExecCtx::check_status()
{
  int ret = OB_SUCCESS;
  if (OB_UNLIKELY(SS_STOPPING == GCTX.status_ || SS_STOPPED == GCTX.status_)) {
    ret = OB_SERVER_IS_STOPPING;
    LOG_WARN("observer is stopped", KR(ret), K(GCTX.status_));
  } else if (OB_FAIL(ObTableLoadService::check_tenant())) {
    LOG_WARN("fail to check tenant", KR(ret));
  } else if (OB_FAIL(exec_ctx_->check_status())) {
    LOG_WARN("fail to check exec ctx status", KR(ret));
  }
  return ret;
}

/**
 * ObTableLoadClientExecCtx
 */

int ObTableLoadClientExecCtx::check_status()
{
  int ret = OB_SUCCESS;
  if (OB_UNLIKELY(SS_STOPPING == GCTX.status_ || SS_STOPPED == GCTX.status_)) {
    ret = OB_SERVER_IS_STOPPING;
    LOG_WARN("observer is stopped", KR(ret), K(GCTX.status_));
  } else if (OB_FAIL(ObTableLoadService::check_tenant())) {
    LOG_WARN("fail to check tenant", KR(ret));
  } else if (OB_UNLIKELY(timeout_ts_ < ObTimeUtil::current_time())) {
    ret = OB_TIMEOUT;
    LOG_WARN("table load is timeout", KR(ret), K_(timeout_ts));
  }
  return ret;
}

} // namespace observer
} // namespace oceanbase