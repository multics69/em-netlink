/* SPDX-License-Identifier: ((GPL-2.0 WITH Linux-syscall-note) OR BSD-3-Clause) */
/* Do not edit directly, auto-generated from: */
/*	Documentation/netlink/specs/em.yaml */
/* YNL-GEN user header */

#ifndef _LINUX_EM_GEN_H
#define _LINUX_EM_GEN_H

#include <stdlib.h>
#include <string.h>
#include <linux/types.h>
#include "energy_model.h"

struct ynl_sock;

extern const struct ynl_family ynl_em_family;

/* Enums */
const char *em_op_str(int op);

/* Common nested types */
struct em_pd {
	struct {
		__u32 pd_id:1;
		__u32 flags:1;
	} _present;
	struct {
		__u32 cpus;
	} _len;

	__u32 pd_id;
	__u64 flags;
	char *cpus;
};

struct em_ps {
	struct {
		__u32 performance:1;
		__u32 frequency:1;
		__u32 power:1;
		__u32 cost:1;
		__u32 flags:1;
	} _present;

	__u64 performance;
	__u64 frequency;
	__u64 power;
	__u64 cost;
	__u64 flags;
};

/* ============== EM_CMD_GET_PDS ============== */
/* EM_CMD_GET_PDS - do */

struct em_get_pds_rsp {
	struct {
		__u32 pd;
	} _count;

	struct em_pd *pd;
};

void em_get_pds_rsp_free(struct em_get_pds_rsp *rsp);

/*
 * Get the list of information for all performance domains.
 */
struct em_get_pds_rsp *em_get_pds(struct ynl_sock *ys);

/* ============== EM_CMD_GET_PD_TABLE ============== */
/* EM_CMD_GET_PD_TABLE - do */
struct em_get_pd_table_req {
	struct {
		__u32 pd_id:1;
	} _present;

	__u32 pd_id;
};

static inline struct em_get_pd_table_req *em_get_pd_table_req_alloc(void)
{
	return calloc(1, sizeof(struct em_get_pd_table_req));
}
void em_get_pd_table_req_free(struct em_get_pd_table_req *req);

static inline void
em_get_pd_table_req_set_pd_id(struct em_get_pd_table_req *req, __u32 pd_id)
{
	req->_present.pd_id = 1;
	req->pd_id = pd_id;
}

struct em_get_pd_table_rsp {
	struct {
		__u32 pd_id:1;
	} _present;
	struct {
		__u32 ps;
	} _count;

	__u32 pd_id;
	struct em_ps *ps;
};

void em_get_pd_table_rsp_free(struct em_get_pd_table_rsp *rsp);

/*
 * Get the energy model table of a performance domain.
 */
struct em_get_pd_table_rsp *
em_get_pd_table(struct ynl_sock *ys, struct em_get_pd_table_req *req);

/* EM_CMD_GET_PD_TABLE - notify */
struct em_get_pd_table_ntf {
	__u16 family;
	__u8 cmd;
	struct ynl_ntf_base_type *next;
	void (*free)(struct em_get_pd_table_ntf *ntf);
	struct em_get_pd_table_rsp obj __attribute__((aligned(8)));
};

void em_get_pd_table_ntf_free(struct em_get_pd_table_ntf *rsp);

/* EM_CMD_PD_DELETED - event */
struct em_pd_deleted_rsp {
	struct {
		__u32 pd_id:1;
	} _present;

	__u32 pd_id;
};

struct em_pd_deleted {
	__u16 family;
	__u8 cmd;
	struct ynl_ntf_base_type *next;
	void (*free)(struct em_pd_deleted *ntf);
	struct em_pd_deleted_rsp obj __attribute__((aligned(8)));
};

void em_pd_deleted_free(struct em_pd_deleted *rsp);

#endif /* _LINUX_EM_GEN_H */
