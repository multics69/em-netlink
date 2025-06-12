// SPDX-License-Identifier: ((GPL-2.0 WITH Linux-syscall-note) OR BSD-3-Clause)
/* Do not edit directly, auto-generated from: */
/*	Documentation/netlink/specs/em.yaml */
/* YNL-GEN user source */

#include <stdlib.h>
#include <string.h>
#include "generated_header_file.h"
#include "ynl.h"
#include <linux/energy_model.h>

#include <linux/genetlink.h>

/* Enums */
static const char * const em_op_strmap[] = {
	[EM_CMD_GET_PDS] = "get-pds",
	[EM_CMD_GET_PD_TABLE] = "get-pd-table",
	[EM_CMD_PD_CREATED] = "pd-created",
	[EM_CMD_PD_UPDATED] = "pd-updated",
	[EM_CMD_PD_DELETED] = "pd-deleted",
};

const char *em_op_str(int op)
{
	if (op < 0 || op >= (int)YNL_ARRAY_SIZE(em_op_strmap))
		return NULL;
	return em_op_strmap[op];
}

/* Policies */
const struct ynl_policy_attr em_pd_policy[EM_A_PD_MAX + 1] = {
	[EM_A_PD_PAD] = { .name = "pad", .type = YNL_PT_IGNORE, },
	[EM_A_PD_PD_ID] = { .name = "pd-id", .type = YNL_PT_U32, },
	[EM_A_PD_FLAGS] = { .name = "flags", .type = YNL_PT_U64, },
	[EM_A_PD_CPUS] = { .name = "cpus", .type = YNL_PT_NUL_STR, },
};

const struct ynl_policy_nest em_pd_nest = {
	.max_attr = EM_A_PD_MAX,
	.table = em_pd_policy,
};

const struct ynl_policy_attr em_ps_policy[EM_A_PS_MAX + 1] = {
	[EM_A_PS_PAD] = { .name = "pad", .type = YNL_PT_IGNORE, },
	[EM_A_PS_PERFORMANCE] = { .name = "performance", .type = YNL_PT_U64, },
	[EM_A_PS_FREQUENCY] = { .name = "frequency", .type = YNL_PT_U64, },
	[EM_A_PS_POWER] = { .name = "power", .type = YNL_PT_U64, },
	[EM_A_PS_COST] = { .name = "cost", .type = YNL_PT_U64, },
	[EM_A_PS_FLAGS] = { .name = "flags", .type = YNL_PT_U64, },
};

const struct ynl_policy_nest em_ps_nest = {
	.max_attr = EM_A_PS_MAX,
	.table = em_ps_policy,
};

const struct ynl_policy_attr em_pds_policy[EM_A_PDS_MAX + 1] = {
	[EM_A_PDS_PD] = { .name = "pd", .type = YNL_PT_NEST, .nest = &em_pd_nest, },
};

const struct ynl_policy_nest em_pds_nest = {
	.max_attr = EM_A_PDS_MAX,
	.table = em_pds_policy,
};

const struct ynl_policy_attr em_pd_table_policy[EM_A_PD_TABLE_MAX + 1] = {
	[EM_A_PD_TABLE_PD_ID] = { .name = "pd-id", .type = YNL_PT_U32, },
	[EM_A_PD_TABLE_PS] = { .name = "ps", .type = YNL_PT_NEST, .nest = &em_ps_nest, },
};

const struct ynl_policy_nest em_pd_table_nest = {
	.max_attr = EM_A_PD_TABLE_MAX,
	.table = em_pd_table_policy,
};

/* Common nested types */
void em_pd_free(struct em_pd *obj)
{
	free(obj->cpus);
}

int em_pd_parse(struct ynl_parse_arg *yarg, const struct nlattr *nested)
{
	struct em_pd *dst = yarg->data;
	const struct nlattr *attr;

	ynl_attr_for_each_nested(attr, nested) {
		unsigned int type = ynl_attr_type(attr);

		if (type == EM_A_PD_PD_ID) {
			if (ynl_attr_validate(yarg, attr))
				return YNL_PARSE_CB_ERROR;
			dst->_present.pd_id = 1;
			dst->pd_id = ynl_attr_get_u32(attr);
		} else if (type == EM_A_PD_FLAGS) {
			if (ynl_attr_validate(yarg, attr))
				return YNL_PARSE_CB_ERROR;
			dst->_present.flags = 1;
			dst->flags = ynl_attr_get_u64(attr);
		} else if (type == EM_A_PD_CPUS) {
			unsigned int len;

			if (ynl_attr_validate(yarg, attr))
				return YNL_PARSE_CB_ERROR;

			len = strnlen(ynl_attr_get_str(attr), ynl_attr_data_len(attr));
			dst->_len.cpus = len;
			dst->cpus = malloc(len + 1);
			memcpy(dst->cpus, ynl_attr_get_str(attr), len);
			dst->cpus[len] = 0;
		}
	}

	return 0;
}

void em_ps_free(struct em_ps *obj)
{
}

int em_ps_parse(struct ynl_parse_arg *yarg, const struct nlattr *nested)
{
	struct em_ps *dst = yarg->data;
	const struct nlattr *attr;

	ynl_attr_for_each_nested(attr, nested) {
		unsigned int type = ynl_attr_type(attr);

		if (type == EM_A_PS_PERFORMANCE) {
			if (ynl_attr_validate(yarg, attr))
				return YNL_PARSE_CB_ERROR;
			dst->_present.performance = 1;
			dst->performance = ynl_attr_get_u64(attr);
		} else if (type == EM_A_PS_FREQUENCY) {
			if (ynl_attr_validate(yarg, attr))
				return YNL_PARSE_CB_ERROR;
			dst->_present.frequency = 1;
			dst->frequency = ynl_attr_get_u64(attr);
		} else if (type == EM_A_PS_POWER) {
			if (ynl_attr_validate(yarg, attr))
				return YNL_PARSE_CB_ERROR;
			dst->_present.power = 1;
			dst->power = ynl_attr_get_u64(attr);
		} else if (type == EM_A_PS_COST) {
			if (ynl_attr_validate(yarg, attr))
				return YNL_PARSE_CB_ERROR;
			dst->_present.cost = 1;
			dst->cost = ynl_attr_get_u64(attr);
		} else if (type == EM_A_PS_FLAGS) {
			if (ynl_attr_validate(yarg, attr))
				return YNL_PARSE_CB_ERROR;
			dst->_present.flags = 1;
			dst->flags = ynl_attr_get_u64(attr);
		}
	}

	return 0;
}

/* ============== EM_CMD_GET_PDS ============== */
/* EM_CMD_GET_PDS - do */
void em_get_pds_rsp_free(struct em_get_pds_rsp *rsp)
{
	unsigned int i;

	for (i = 0; i < rsp->_count.pd; i++)
		em_pd_free(&rsp->pd[i]);
	free(rsp->pd);
	free(rsp);
}

int em_get_pds_rsp_parse(const struct nlmsghdr *nlh,
			 struct ynl_parse_arg *yarg)
{
	struct em_get_pds_rsp *dst;
	const struct nlattr *attr;
	struct ynl_parse_arg parg;
	unsigned int n_pd = 0;
	int i;

	dst = yarg->data;
	parg.ys = yarg->ys;

	if (dst->pd)
		return ynl_error_parse(yarg, "attribute already present (pds.pd)");

	ynl_attr_for_each(attr, nlh, yarg->ys->family->hdr_len) {
		unsigned int type = ynl_attr_type(attr);

		if (type == EM_A_PDS_PD) {
			n_pd++;
		}
	}

	if (n_pd) {
		dst->pd = calloc(n_pd, sizeof(*dst->pd));
		dst->_count.pd = n_pd;
		i = 0;
		parg.rsp_policy = &em_pd_nest;
		ynl_attr_for_each(attr, nlh, yarg->ys->family->hdr_len) {
			if (ynl_attr_type(attr) == EM_A_PDS_PD) {
				parg.data = &dst->pd[i];
				if (em_pd_parse(&parg, attr))
					return YNL_PARSE_CB_ERROR;
				i++;
			}
		}
	}

	return YNL_PARSE_CB_OK;
}

struct em_get_pds_rsp *em_get_pds(struct ynl_sock *ys)
{
	struct ynl_req_state yrs = { .yarg = { .ys = ys, }, };
	struct em_get_pds_rsp *rsp;
	struct nlmsghdr *nlh;
	int err;

	nlh = ynl_gemsg_start_req(ys, ys->family_id, EM_CMD_GET_PDS, 1);
	ys->req_policy = &em_pds_nest;
	ys->req_hdr_len = ys->family->hdr_len;
	yrs.yarg.rsp_policy = &em_pds_nest;

	rsp = calloc(1, sizeof(*rsp));
	yrs.yarg.data = rsp;
	yrs.cb = em_get_pds_rsp_parse;
	yrs.rsp_cmd = EM_CMD_GET_PDS;

	err = ynl_exec(ys, nlh, &yrs);
	if (err < 0)
		goto err_free;

	return rsp;

err_free:
	em_get_pds_rsp_free(rsp);
	return NULL;
}

/* ============== EM_CMD_GET_PD_TABLE ============== */
/* EM_CMD_GET_PD_TABLE - do */
void em_get_pd_table_req_free(struct em_get_pd_table_req *req)
{
	free(req);
}

void em_get_pd_table_rsp_free(struct em_get_pd_table_rsp *rsp)
{
	unsigned int i;

	for (i = 0; i < rsp->_count.ps; i++)
		em_ps_free(&rsp->ps[i]);
	free(rsp->ps);
	free(rsp);
}

int em_get_pd_table_rsp_parse(const struct nlmsghdr *nlh,
			      struct ynl_parse_arg *yarg)
{
	struct em_get_pd_table_rsp *dst;
	const struct nlattr *attr;
	struct ynl_parse_arg parg;
	unsigned int n_ps = 0;
	int i;

	dst = yarg->data;
	parg.ys = yarg->ys;

	if (dst->ps)
		return ynl_error_parse(yarg, "attribute already present (pd-table.ps)");

	ynl_attr_for_each(attr, nlh, yarg->ys->family->hdr_len) {
		unsigned int type = ynl_attr_type(attr);

		if (type == EM_A_PD_TABLE_PD_ID) {
			if (ynl_attr_validate(yarg, attr))
				return YNL_PARSE_CB_ERROR;
			dst->_present.pd_id = 1;
			dst->pd_id = ynl_attr_get_u32(attr);
		} else if (type == EM_A_PD_TABLE_PS) {
			n_ps++;
		}
	}

	if (n_ps) {
		dst->ps = calloc(n_ps, sizeof(*dst->ps));
		dst->_count.ps = n_ps;
		i = 0;
		parg.rsp_policy = &em_ps_nest;
		ynl_attr_for_each(attr, nlh, yarg->ys->family->hdr_len) {
			if (ynl_attr_type(attr) == EM_A_PD_TABLE_PS) {
				parg.data = &dst->ps[i];
				if (em_ps_parse(&parg, attr))
					return YNL_PARSE_CB_ERROR;
				i++;
			}
		}
	}

	return YNL_PARSE_CB_OK;
}

struct em_get_pd_table_rsp *
em_get_pd_table(struct ynl_sock *ys, struct em_get_pd_table_req *req)
{
	struct ynl_req_state yrs = { .yarg = { .ys = ys, }, };
	struct em_get_pd_table_rsp *rsp;
	struct nlmsghdr *nlh;
	int err;

	nlh = ynl_gemsg_start_req(ys, ys->family_id, EM_CMD_GET_PD_TABLE, 1);
	ys->req_policy = &em_pd_table_nest;
	ys->req_hdr_len = ys->family->hdr_len;
	yrs.yarg.rsp_policy = &em_pd_table_nest;

	if (req->_present.pd_id)
		ynl_attr_put_u32(nlh, EM_A_PD_TABLE_PD_ID, req->pd_id);

	rsp = calloc(1, sizeof(*rsp));
	yrs.yarg.data = rsp;
	yrs.cb = em_get_pd_table_rsp_parse;
	yrs.rsp_cmd = EM_CMD_GET_PD_TABLE;

	err = ynl_exec(ys, nlh, &yrs);
	if (err < 0)
		goto err_free;

	return rsp;

err_free:
	em_get_pd_table_rsp_free(rsp);
	return NULL;
}

/* EM_CMD_GET_PD_TABLE - notify */
void em_get_pd_table_ntf_free(struct em_get_pd_table_ntf *rsp)
{
	unsigned int i;

	for (i = 0; i < rsp->obj._count.ps; i++)
		em_ps_free(&rsp->obj.ps[i]);
	free(rsp->obj.ps);
	free(rsp);
}

/* EM_CMD_PD_DELETED - event */
int em_pd_deleted_rsp_parse(const struct nlmsghdr *nlh,
			    struct ynl_parse_arg *yarg)
{
	struct em_pd_deleted_rsp *dst;
	const struct nlattr *attr;

	dst = yarg->data;

	ynl_attr_for_each(attr, nlh, yarg->ys->family->hdr_len) {
		unsigned int type = ynl_attr_type(attr);

		if (type == EM_A_PD_TABLE_PD_ID) {
			if (ynl_attr_validate(yarg, attr))
				return YNL_PARSE_CB_ERROR;
			dst->_present.pd_id = 1;
			dst->pd_id = ynl_attr_get_u32(attr);
		}
	}

	return YNL_PARSE_CB_OK;
}

void em_pd_deleted_free(struct em_pd_deleted *rsp)
{
	free(rsp);
}

static const struct ynl_ntf_info em_ntf_info[] =  {
	[EM_CMD_PD_CREATED] =  {
		.alloc_sz	= sizeof(struct em_get_pd_table_ntf),
		.cb		= em_get_pd_table_rsp_parse,
		.policy		= &em_pd_table_nest,
		.free		= (void *)em_get_pd_table_ntf_free,
	},
	[EM_CMD_PD_UPDATED] =  {
		.alloc_sz	= sizeof(struct em_get_pd_table_ntf),
		.cb		= em_get_pd_table_rsp_parse,
		.policy		= &em_pd_table_nest,
		.free		= (void *)em_get_pd_table_ntf_free,
	},
	[EM_CMD_PD_DELETED] =  {
		.alloc_sz	= sizeof(struct em_pd_deleted),
		.cb		= em_pd_deleted_rsp_parse,
		.policy		= &em_pd_table_nest,
		.free		= (void *)em_pd_deleted_free,
	},
};

const struct ynl_family ynl_em_family =  {
	.name		= "em",
	.hdr_len	= sizeof(struct genlmsghdr),
	.ntf_info	= em_ntf_info,
	.ntf_info_size	= YNL_ARRAY_SIZE(em_ntf_info),
};
