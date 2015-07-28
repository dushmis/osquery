/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "osquery/events/kernel.h"

#include <osquery/logger.h>

namespace osquery {

class ProcessEventSubscriber
  : public EventSubscriber<KernelEventPublisher> {
 public:
  Status init();

  Status Callback(const TypedKernelEventContextRef<osquery_process_event_t> &ec,
                  const void *user_data);
};

REGISTER(ProcessEventSubscriber, "event_subscriber", "process_events");

Status ProcessEventSubscriber::init() {
  auto sc = createSubscriptionContext();
  sc->event_type = OSQUERY_PROCESS_EVENT;
  sc->udata = NULL;
  subscribe(&ProcessEventSubscriber::Callback, sc, NULL);

  return Status(0, "OK");
}

Status ProcessEventSubscriber::Callback(
    const TypedKernelEventContextRef<osquery_process_event_t> &ec,
    const void *user_data) {
  Row r;

  r["argc"] = BIGINT(ec->event.argc);
  r["actual_argc"] = BIGINT(ec->event.actual_argc);
  r["argv_length"] = BIGINT(ec->event.arg_length);
  r["envc"] = BIGINT(ec->event.envc);
  r["actual_envc"] = BIGINT(ec->event.actual_envc);
  r["envv_length"] = BIGINT(ec->event.env_length);

  char *argv = &(ec->flexible_data.data()[ec->event.argv_offset]);
  std::string argv_accumulator("");
  while (ec->event.argc-- > 0) {
    argv_accumulator += argv;
    argv_accumulator += " ";
    argv += strlen(argv) + 1;
  }

  char *envv = &(ec->flexible_data.data()[ec->event.envv_offset]);
  std::string envv_accumulator("");
  while (ec->event.envc-- > 0) {
    envv_accumulator += envv;
    envv_accumulator += " ";
    envv += strlen(envv) + 1;
  }

  r["argv"] = std::move(argv_accumulator);
  r["envv"] = std::move(envv_accumulator);

  r["pid"] = BIGINT(ec->event.pid);
  r["parent"] = BIGINT(ec->event.ppid);
  r["uid"] = BIGINT(ec->event.uid);
  r["euid"] = BIGINT(ec->event.euid);
  r["gid"] = BIGINT(ec->event.gid);
  r["egid"] = BIGINT(ec->event.egid);
  r["owner_uid"] = BIGINT(ec->event.owner_uid);
  r["owner_gid"] = BIGINT(ec->event.owner_gid);
  r["create_time"] = BIGINT(ec->event.create_time);
  r["access_time"] = BIGINT(ec->event.access_time);
  r["modify_time"] = BIGINT(ec->event.modify_time);
  r["change_time"] = BIGINT(ec->event.change_time);
  r["mode"] = BIGINT(ec->event.mode);
  r["path"] = ec->event.path;
  r["uptime"] = BIGINT(ec->uptime);

  add(r, ec->time);

  return Status(0, "OK");
}


}  // namespace osquery
